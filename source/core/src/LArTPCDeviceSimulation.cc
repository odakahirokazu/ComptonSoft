#include "LArTPCDeviceSimulation.hh"
#include "AstroUnits.hh"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4ParticleTable.hh"
#include "LArEFieldModel.hh"
#include "TFile.h"
#include "TSpline.h"
#include "TTree.h"

#include "G4IonisParamMat.hh"
#include "G4Material.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4ProcessManager.hh"
#include "G4ProductionCutsTable.hh"
#include "G4VEnergyLossProcess.hh"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iostream>

using namespace anlgeant4::unit;
namespace comptonsoft {

namespace {

G4VEnergyLossProcess* ionisationProcess(const G4ParticleDefinition* particle)
{
  const G4ProcessManager* processManager = particle->GetProcessManager();
  if (processManager == nullptr) {
    return nullptr;
  }
  const G4ProcessVector* processes = processManager->GetProcessList();
  if (processes == nullptr) {
    return nullptr;
  }
  for (std::size_t i = 0; i < processes->size(); i++) {
    G4VEnergyLossProcess* process = dynamic_cast<G4VEnergyLossProcess*>((*processes)[i]);
    if (process != nullptr) {
      return process;
    }
  }
  return nullptr;
}

} /* anonymous namespace */

LArTPCDeviceSimulation::LArTPCDeviceSimulation()
  : DeviceSimulation(),
    recombinationModel_(nullptr)
{
  setEFieldModel(std::make_unique<LArEFieldModel>());
  // setDiffusionDivisionNumber(1000);
}
void LArTPCDeviceSimulation::printSimulationParameters(std::ostream& os) const
{
  DeviceSimulation::printSimulationParameters(os);
  os << "LArTPC Device Simulation Parameters:\n";
  os << "  Drift velocity: " << driftVelocity_ / (CLHEP::mm / CLHEP::microsecond) << " mm/us\n";
  os << "  Longitudinal diffusion coefficient: " << longitudinalDiffusionCoefficient_ / (CLHEP::cm2 / CLHEP::s)
     << " cm^2/s\n";
  os << "  Transverse diffusion coefficient: " << transverseDiffusionCoefficient_ / (CLHEP::cm2 / CLHEP::s)
     << " cm^2/s\n";
  if (recombinationModel_) {
    recombinationModel_->printInfo(os);
    os << "  Photon efficiency: " << PhotonEfficiency(0, 0, 0) << "\n";
    os << "  Photon noise parameters: param0 = " << PhotonNoiseParam0() << ", param1 = " << PhotonNoiseParam1()
       << ", param2 = " << PhotonNoiseParam2() << "\n";
  }
  else {
    os << "  No recombination model is set.\n";
  }
  os << "  dEdx mode: "
     << (dEdxMode() == 0
             ? "from step information"
             : (dEdxMode() == 1 ? "from kinetic energy" : (dEdxMode() == 2 ? "from G4EmCalculator" : "unknown")))
     << "\n";
  os << "  dEdx spline: " << (dedxSpline_ ? dedxSpline_->GetName() : "not set") << "\n";
}
double LArTPCDeviceSimulation::DiffusionSigmaAnode3D(double z, double& longitudinal, double& transverse)
{
  if (DiffusionMode() == 3) { // Calculate from drift time and diffusion coefficients
    double zAnode;
    if (isUpSideAnode()) {
      zAnode = 0.5 * getThickness();
    }
    else {
      zAnode = -0.5 * getThickness();
    }
    if (driftVelocity_ <= 0.0) {
      throw std::runtime_error("LArTPCDeviceSimulation::DiffusionSigmaAnode3D: Drift velocity is not set or invalid.");
    }
    if (longitudinalDiffusionCoefficient_ < 0.0 || transverseDiffusionCoefficient_ < 0.0) {
      throw std::runtime_error(
          "LArTPCDeviceSimulation::DiffusionSigmaAnode3D: Diffusion coefficients are not set or invalid.");
    }
    const double t = std::abs((zAnode - z) / driftVelocity_);
    const double sigma_t = std::sqrt(2. * transverseDiffusionCoefficient_ * t) * DiffusionSpreadFactorAnode();
    const double sigma_l = std::sqrt(2. * longitudinalDiffusionCoefficient_ * t) * DiffusionSpreadFactorAnode();
    longitudinal = sigma_l;
    transverse = sigma_t;
    return sigma_t;
  }
  else {
    return DeviceSimulation::DiffusionSigmaAnode(z);
  }
}
double LArTPCDeviceSimulation::DiffusionSigmaCathode(double)
{
  return 0.0; // hole diffusion is not considered
}

void LArTPCDeviceSimulation::instantiateRecombinationModel(const std::string& config_filename)
{
  recombinationModel_ = LArRecombinationModelFactory::load(config_filename);
}

void LArTPCDeviceSimulation::applyRecombination(DetectorHit_sptr& hit)
{
  const double edep = hit->EnergyDeposit();
  if (!recombinationModel_) {
    hit->setPhotonCount(0.0);
    hit->setEnergyCharge(edep);
    hit->addFlags(flag::RecombinationNotApplied);
    return;
  }
  if (edep <= 0.0) {
    hit->setPhotonCount(0.0);
    hit->setEnergyCharge(0.0);
    hit->addFlags(flag::RecombinationNotApplied);
    return;
  }
  auto particle = hit->Particle();
  auto partDef = G4ParticleTable::GetParticleTable()->FindParticle(particle);
  if (partDef->GetPDGCharge() == 0) {
    hit->setPhotonCount(recombinationModel_->lightYield(edep, 1.0) *
                        PhotonEfficiency(hit->LocalPositionX(), hit->LocalPositionY(), hit->LocalPositionZ()));
    hit->setEnergyCharge(edep);
    hit->addFlags(flag::RecombinationNotApplied);
    return;
  }

  const double step_length = hit->StepLength();
  const double efield = BiasVoltage() / getThickness();
  RecombinationResult result;

  if (dEdxMode() == 0) {
    if (step_length <= 0.0) {
      hit->setPhotonCount(recombinationModel_->lightYield(edep, 1.0) *
                          PhotonEfficiency(hit->LocalPositionX(), hit->LocalPositionY(), hit->LocalPositionZ()));
      hit->setEnergyCharge(edep);
      hit->addFlags(flag::RecombinationNotApplied);
      std::cout << "Warning: step length is zero or negative for kinetic energy: " << step_length / cm
                << " cm. Recombination is not applied for this hit." << std::endl;

      return;
    }
    result = recombinationModel_->calculateRecombination(edep, step_length, efield);
  }
  else if (dEdxMode() == 1 || dEdxMode() == 2) {
    const double dedx = getdEdxFromKineticEnergy(hit);
    if (dedx <= 0.0) {
      hit->setPhotonCount(recombinationModel_->lightYield(edep, 1.0) *
                          PhotonEfficiency(hit->LocalPositionX(), hit->LocalPositionY(), hit->LocalPositionZ()));
      hit->setEnergyCharge(edep);
      hit->addFlags(flag::RecombinationNotApplied);
      printdEdxDiagnostics(hit, dedx, std::cerr);
      return;
    }
    result = recombinationModel_->calculateRecombination(edep, step_length, efield, dedx);
  }
  else {
    throw std::runtime_error("LArTPCDeviceSimulation::applyRecombination: Invalid dEdx mode: " +
                             std::to_string(dEdxMode()));
  }

  hit->setPhotonCount(result.photonYield *
                      PhotonEfficiency(hit->LocalPositionX(), hit->LocalPositionY(), hit->LocalPositionZ()));
  hit->setEnergyCharge(result.electronEnergy);
  hit->addFlags(flag::RecombinationApplied);
}

double LArTPCDeviceSimulation::getdEdxFromKineticEnergy(const DetectorHit_sptr& hit) const
{
  const double kineticEnergy = hit->KineticEnergy();
  if (kineticEnergy <= 0.0) {
    return 0.0;
  }
  if (dEdxMode() == 1) {
    if (!dedxSpline_) {
      throw std::runtime_error("LArTPCDeviceSimulation::getdEdxFromKineticEnergy: dEdx spline is not set.");
    }
    return dedxSpline_->Eval(kineticEnergy / keV) * keV / cm;
  }
  else if (dEdxMode() == 2) {
    const double value = getdEdxFromEmCalculator(hit);
#ifdef VERBOSE
    if (dedxSpline_ && hit->EnergyDeposit() > 10.0 * keV) {
      const double ref = dedxSpline_->Eval(kineticEnergy / keV) * keV / cm;
      std::cout << "edep: " << hit->EnergyDeposit() / keV << " keV ref : " << ref / keV * cm
                << " keV/cm dedx: " << value / keV * cm << " keV/cm" << "" << std::endl;
    }
#endif
    return value;
  }
  throw std::runtime_error("LArTPCDeviceSimulation::getdEdxFromKineticEnergy: invalid dedx mode");
}

double LArTPCDeviceSimulation::getdEdxFromEmCalculator(const DetectorHit_sptr& hit) const
{
  const G4MaterialCutsCouple* couple = hit->MaterialCutsCouple();
  if (couple == nullptr) {
    std::cout << "couple is nullptr" << std::endl;
    return 0.0;
  }
  const G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(hit->Particle());
  if (particle == nullptr || particle->GetPDGCharge() == 0.0) {
    std::cout << "particle is nullptr or charge is 0" << std::endl;
    return 0.0;
  }

  auto it = ionisationProcessCache_.find(particle);
  if (it == ionisationProcessCache_.end()) {
    it = ionisationProcessCache_.emplace(particle, ionisationProcess(particle)).first;
  }
  G4VEnergyLossProcess* process = it->second;
  if (process == nullptr) {
    std::cout << "process is nullptr" << std::endl;
    return 0.0;
  }

  double kineticEnergy = hit->KineticEnergy();
  if (kineticEnergy <= 0.0) {
    std::cout << "kinetic energy is 0 or negative: " << kineticEnergy / keV << " keV" << std::endl;
    return 0.0;
  }

  kineticEnergy = std::max(kineticEnergy, modelLowEnergyLimit(process, couple, kineticEnergy));

  return calculator_.ComputeDEDX(kineticEnergy, particle, process->GetProcessName(), couple->GetMaterial(), DBL_MAX);
}

double LArTPCDeviceSimulation::modelLowEnergyLimit(const G4VEnergyLossProcess* process,
                                                  const G4MaterialCutsCouple* couple,
                                                  double kineticEnergy)
{
  std::size_t coupleIndex = static_cast<std::size_t>(couple->GetIndex());
  const G4VEmModel* model = process->SelectModelForMaterial(kineticEnergy, coupleIndex);
  return (model != nullptr) ? model->LowEnergyLimit() : 0.0;
}

void LArTPCDeviceSimulation::printdEdxDiagnostics(const DetectorHit_sptr& hit, double dedx, std::ostream& os) const
{
  os << "### LArTPCDeviceSimulation: dEdx is not positive; recombination is not applied ###\n";
  os << "  dEdx returned:  " << dedx / (keV / cm) << " keV/cm\n";
  os << "  kinetic energy: " << hit->KineticEnergy() / keV << " keV\n";
  os << "  energy deposit: " << hit->EnergyDeposit() / keV << " keV\n";
  os << "  step length:    " << hit->StepLength() / CLHEP::micrometer << " um\n";

  const G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(hit->Particle());
  if (particle == nullptr) {
    os << "  particle: (not found for PDG code " << hit->Particle() << ")" << std::endl;
    return;
  }
  os << "  particle: " << particle->GetParticleName() << " (PDG " << hit->Particle() << ", type "
     << particle->GetParticleType() << ", charge " << particle->GetPDGCharge() / CLHEP::eplus << ")\n";

  const G4MaterialCutsCouple* couple = hit->MaterialCutsCouple();
  if (couple == nullptr) {
    os << "  material-cuts-couple: (null)" << std::endl;
    return;
  }
  const G4Material* material = couple->GetMaterial();
  if (material == nullptr) {
    os << "  material: (null)" << std::endl;
    return;
  }
  os << "  material: " << material->GetName() << " (Zeff " << material->GetIonisation()->GetZeffective()
     << ", mean excitation energy " << material->GetIonisation()->GetMeanExcitationEnergy() / CLHEP::eV << " eV)\n";

  auto it = ionisationProcessCache_.find(particle);
  G4VEnergyLossProcess* process = (it != ionisationProcessCache_.end()) ? it->second : ionisationProcess(particle);
  if (process == nullptr) {
    os << "  ionisation process: (none registered for this particle)" << std::endl;
    return;
  }
  const G4String& processName = process->GetProcessName();
  os << "  ionisation process: " << processName << "\n";

  const double e = hit->KineticEnergy();
  const double lowLimit = modelLowEnergyLimit(process, couple, e);
  os << "  model low-energy limit: " << lowLimit / keV << " keV" << (e < lowLimit ? "  <-- BELOW LIMIT" : "") << "\n";

  // getdEdxFromEmCalculator already clips to the low-energy limit, so reaching
  // this point means the model returned zero even inside its validity range.
  os << "  unrestricted ComputeDEDX at E:          "
     << calculator_.ComputeDEDX(e, particle, processName, material, DBL_MAX) / (keV / cm) << " keV/cm\n";
  os << "  unrestricted ComputeDEDX at low limit:  "
     << calculator_.ComputeDEDX(std::max(e, lowLimit), particle, processName, material, DBL_MAX) / (keV / cm)
     << " keV/cm\n";
  // Same route but with the real production cut instead of DBL_MAX, to tell
  // apart a failure caused by asking for an unrestricted value.
  const auto* cutsTable = G4ProductionCutsTable::GetProductionCutsTable();
  const double electronCutEnergy =
      cutsTable->GetEnergyCutsVector(idxG4ElectronCut)->at(cutsTable->GetCoupleIndex(couple));
  os << "  electron production cut:                " << electronCutEnergy / keV << " keV\n";
  os << "  restricted ComputeDEDX at low limit:    "
     << calculator_.ComputeDEDX(std::max(e, lowLimit), particle, processName, material, electronCutEnergy) /
            (keV / cm)
     << " keV/cm\n";
  os << "  restricted GetDEDX (table) at E:        " << process->GetDEDX(e, couple) / (keV / cm) << " keV/cm"
     << std::endl;
}

void LArTPCDeviceSimulation::setdEdxFile(const std::string& filename, const std::string& spline_name)
{
  if (filename.empty()) {
    return;
  }
  TFile* file = TFile::Open(filename.c_str(), "READ");
  if (!file || file->IsZombie()) {
    throw std::runtime_error("LArTPCDeviceSimulation::setdEdxFile: Cannot open file: " + filename);
  }
  dedxFile_ = file;
  dedxSpline_ = (TSpline*)(file->Get(spline_name.c_str()));
  if (!dedxSpline_) {
    throw std::runtime_error("LArTPCDeviceSimulation::setdEdxFile: Cannot find spline: " + spline_name);
  }
}
void LArTPCDeviceSimulation::makeRawDetectorHits()
{
  auto& raw_hits = getRawHits();
  for (auto& hit : raw_hits) {
    // hit->setdEdx(hit->EnergyDeposit() / hit->StepLength());
    applyRecombination(hit);
    insertDetectorHit(hit);
  }
}
} // namespace comptonsoft