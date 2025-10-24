#include "LArTPCDeviceSimulation.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "LArEFieldModel.hh"
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
  }
  else {
    os << "  No recombination model is set.\n";
  }
}
double LArTPCDeviceSimulation::DiffusionSigmaAnode3D(double z, double &longitudinal, double &transverse) {
  if (DiffusionMode() == 3) { // Calcurate from temperature and electric field
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

void LArTPCDeviceSimulation::instantiateRecombinationModel(const std::string &config_filename) {
  recombinationModel_ = VLArRecombinationModelFactory::load(config_filename);
}
double LArTPCDeviceSimulation::applyRecombination(DetectorHit_sptr hit) const {
  const double edep = hit->EnergyDeposit();
  if (!recombinationModel_) {
    return edep;
  }
  if (edep <= 0.0) return edep;
  if (!hit->isContinuousProcess()) return edep;
  const double length = (hit->PostStepPointPosition() - hit->PreStepPointPosition()).mag();
  //std::cout << "Applying recombination model: edep = " << edep
  //<< ", length = " << length << std::endl;
  const double let = edep / length;
  const double let_new = recombinationModel_->electronLet(let, BiasVoltage() / getThickness());
  //std::cout << "Recombination model applied: new let = " << let_new << " edep_new = " << let_new * length << std::endl;
  return std::max(let_new * length, 0.0);
}
} // namespace comptonsoft