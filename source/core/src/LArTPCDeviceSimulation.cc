#include "LArTPCDeviceSimulation.hh"
namespace comptonsoft {
LArTPCDeviceSimulation::LArTPCDeviceSimulation()
    : DeviceSimulation(),
      recombinationModel_(nullptr) {}
void LArTPCDeviceSimulation::printSimulationParameters(std::ostream &os) const {
  DeviceSimulation::printSimulationParameters(os);
  if (recombinationModel_) {
    recombinationModel_->printInfo(os);
  }
  else {
    os << "  No recombination model is set.\n";
  }
}
void LArTPCDeviceSimulation::instantiateRecombinationModel(const std::string &config_filename) {
  recombinationModel_ = VLArRecombinationModelFactory::load(config_filename);
}
double LArTPCDeviceSimulation::applyRecombination(DetectorHit_sptr hit) const {
  const double edep = hit->EnergyDeposit();
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