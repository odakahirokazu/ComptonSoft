#include "LArTPCDeviceSimulation.hh"
#include "AstroUnits.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandGauss.h"
#include "LArEFieldModel.hh"
#include "TFile.h"
#include "TSpline.h"
#include "TTree.h"
using namespace anlgeant4::unit;
namespace comptonsoft {
LArTPCDeviceSimulation::LArTPCDeviceSimulation()
    : DeviceSimulation(),
      recombinationModel_(nullptr) {
  setEFieldModel(std::make_unique<LArEFieldModel>());
  //setDiffusionDivisionNumber(1000);
}
void LArTPCDeviceSimulation::printSimulationParameters(std::ostream &os) const {
  DeviceSimulation::printSimulationParameters(os);
  os << "LArTPC Device Simulation Parameters:\n";
  os << "  Drift velocity: " << driftVelocity_ / (CLHEP::mm / CLHEP::microsecond) << " mm/us\n";
  os << "  Longitudinal diffusion coefficient: " << longitudinalDiffusionCoefficient_ / (CLHEP::cm2 / CLHEP::s) << " cm^2/s\n";
  os << "  Transverse diffusion coefficient: " << transverseDiffusionCoefficient_ / (CLHEP::cm2 / CLHEP::s) << " cm^2/s\n";
  if (recombinationModel_) {
    recombinationModel_->printInfo(os);
    os << "  Photon efficiency: " << PhotonEfficiency(0, 0, 0) << "\n";
    os << "  Photon noise parameters: param0 = " << PhotonNoiseParam0() << ", param1 = " << PhotonNoiseParam1() << ", param2 = " << PhotonNoiseParam2() << "\n";
  }
  else {
    os << "  No recombination model is set.\n";
  }
  os << "  Response graph list for EPI compensation: " << responseGraphListForEPICompensation_.size() << " graphs loaded. Name header is " << (responseGraphListForEPICompensation_.empty() ? "none" : responseGraphListForEPICompensation_[0]->GetName()) << "\n";
  os << "  dEdx spline: " << (dedxSpline_ ? dedxSpline_->GetName() : "not set") << "\n";
}
double LArTPCDeviceSimulation::DiffusionSigmaAnode3D(double z, double &longitudinal, double &transverse) {
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
      throw std::runtime_error("LArTPCDeviceSimulation::DiffusionSigmaAnode3D: Diffusion coefficients are not set or invalid.");
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
double LArTPCDeviceSimulation::DiffusionSigmaCathode(double) {
  return 0.0; // hole diffusion is not considered
}

std::tuple<double, double> LArTPCDeviceSimulation::compensateEPI(const PixelID &sp, std::tuple<double, double> ePI) const {
  const auto graph = getResponseGraphListForEPICompensation();

  auto new_epi = DeviceSimulation::compensateEPI(sp, ePI);
  if (graph.size() > 0) {
    int iz = sp.Z();
    if (iz >= 0 && zIndicesForEPI_[iz] < static_cast<int>(graph.size())) {
      const double ePI_value = std::get<0>(new_epi);
      const double ePI_error = std::get<1>(new_epi);
      double new_e = graph[zIndicesForEPI_[iz]]->Eval(ePI_value / keV, nullptr, "S") * keV;
      if (new_e > 0.0) {
        double rate = ePI_value / new_e;
        double compensated_ePI_error = ePI_error / rate;
        new_epi = std::make_tuple(new_e, compensated_ePI_error);
      }
    }
    else {
      std::cerr << "LArTPCDeviceSimulation::compensateEPI: Warning! Z index out of range for spline list: " << iz << " " << zIndicesForEPI_[iz] << std::endl;
    }
  }
  return new_epi;
}

void LArTPCDeviceSimulation::setResponseFileForEPICompensation(const std::string &filename, const std::string &graph_name) {
  if (filename.empty()) {
    return;
  }
  TFile *file = TFile::Open(filename.c_str(), "READ");
  if (!file || file->IsZombie()) {
    throw std::runtime_error("LArTPCDeviceSimulation::setResponseFileForEPICompensation: Cannot open file: " + filename);
  }
  int nz = 0, nb = 0;
  std::vector<int> *pixelz_map = nullptr;
  if (auto meta = (TTree *)file->Get("meta")) {
    meta->SetBranchAddress("N_pixelz", &nz);
    meta->SetBranchAddress("N_bins", &nb);
    meta->SetBranchAddress("pixelz_mapping", &pixelz_map);
    meta->GetEntry(0);
    if (!pixelz_map) {
      throw std::runtime_error("LArTPCDeviceSimulation::setResponseFileForEPICompensation: pixelz_mapping not found in meta tree.");
    }
    zIndicesForEPI_ = *pixelz_map;
  }
  responseFileForEPICompensation_ = file;
  const auto num_div = zIndicesForEPI_.back() + 1;
  for (int iz = 0; iz < num_div; ++iz) {
    auto graph = dynamic_cast<TGraph *>(file->Get(Form("%s%d", graph_name.c_str(), iz)));
    if (!graph) {
      throw std::runtime_error(Form("LArTPCDeviceSimulation::setResponseFileForEPICompensation: Cannot find spline: %s%d", graph_name.c_str(), iz));
    }
    responseGraphListForEPICompensation_.push_back(graph);
  }
  if (getNumVoxelZ() > nz) {
    std::cerr << "LArTPCDeviceSimulation::setResponseFileForEPICompensation: Warning! Number of Z voxels (" << getNumVoxelZ()
              << ") is larger than number of splines loaded (" << nz << ")." << std::endl;
  }
}

void LArTPCDeviceSimulation::instantiateRecombinationModel(const std::string &config_filename) {
  recombinationModel_ = VLArRecombinationModelFactory::load(config_filename);
}

void LArTPCDeviceSimulation::applyRecombination(DetectorHit_sptr &hit) {
  const double edep = hit->EnergyDeposit();
  if (!recombinationModel_) {
    hit->setPhotonCount(0.0);
    hit->setEnergyCharge(edep);
    return;
  }
  if (edep <= 0.0) {
    hit->setPhotonCount(0.0);
    hit->setEnergyCharge(0.0);
    return;
  }
  if (!hit->isContinuousProcess()) {
    hit->setPhotonCount(recombinationModel_->lightYield(edep, 1.0));
    hit->setEnergyCharge(edep);
    return;
  }
  
  double dedx = 0.0;
  if (!getdEdxSpline()) {
    const double step_length = (hit->PostStepPointPosition() - hit->PreStepPointPosition()).mag();
    if (step_length <= 0.0) {
      hit->setPhotonCount(recombinationModel_->lightYield(edep, 1.0));
      hit->setEnergyCharge(edep);
      return;
    }
    dedx = edep / step_length;
  }
  else {
    const double ken = hit->KineticEnergy();
    dedx = getdEdxFromKineticEnergy(ken);
    if (dedx <= 0.0) {
      hit->setPhotonCount(recombinationModel_->lightYield(edep, 1.0));
      hit->setEnergyCharge(edep);
      return;
    }
    
  }
  const double recombination_factor = recombinationModel_->getRecombinationFactor(dedx, BiasVoltage() / getThickness());
  const auto new_edep = std::max(edep * recombination_factor, 0.0);
  const auto new_light_count = recombinationModel_->lightYield(edep, recombination_factor) * PhotonEfficiency(hit->LocalPositionX(), hit->LocalPositionY(), hit->LocalPositionZ());
  hit->setPhotonCount(new_light_count);
  hit->setEnergyCharge(new_edep);
}

double LArTPCDeviceSimulation::getdEdxFromKineticEnergy(double kineticEnergy) const {
  if (kineticEnergy <= 0.0) {
    return 0.0;
  }
  if (!dedxSpline_) {
    throw std::runtime_error("LArTPCDeviceSimulation::getdEdxFromKineticEnergy: dEdx spline is not set.");
  }
  return dedxSpline_->Eval(kineticEnergy / keV) * keV / cm;
}
void LArTPCDeviceSimulation::setdEdxFile(const std::string &filename, const std::string &spline_name) {
  if (filename.empty()) {
    return;
  }
  TFile *file = TFile::Open(filename.c_str(), "READ");
  if (!file || file->IsZombie()) {
    throw std::runtime_error("LArTPCDeviceSimulation::setdEdxFile: Cannot open file: " + filename);
  }
  dedxFile_ = file;
  dedxSpline_ = (TSpline *)(file->Get(spline_name.c_str()));
  if (!dedxSpline_) {
    throw std::runtime_error("LArTPCDeviceSimulation::setdEdxFile: Cannot find spline: " + spline_name);
  }
}
} // namespace comptonsoft