#ifndef COMPTONSOFT_LArTPCDeviceSimulation_H
#define COMPTONSOFT_LArTPCDeviceSimulation_H 1
#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include "DeviceSimulation.hh"
#include "EFieldModel.hh"
#include "VLArRecombinationModel.hh"
#include "VLArRecombinationModelFactory.hh"
#include <memory>
namespace comptonsoft {
class DetectorHit;
class EFieldModel;
class VLArRecombinationModel;
/**
 * A class of device simulation for LArTPC detectors.
 * @author Shota Arai
 * @date 2025-10-10 | first implementation
 */
class LArTPCDeviceSimulation: public DeviceSimulation {
public:
  LArTPCDeviceSimulation();
  virtual ~LArTPCDeviceSimulation() = default;

  void instantiateRecombinationModel(const std::string &config_filename) override;
  double applyRecombination(DetectorHit_sptr hit) const override;
  void printSimulationParameters(std::ostream &os) const override;

private:
  std::unique_ptr<VLArRecombinationModel> recombinationModel_;
};
} // namespace comptonsoft
#endif //COMPTONSOFT_LArTPCDeviceSimlation_H