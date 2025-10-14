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
  double DiffusionSigmaAnode3D(double z, double &longitudinal, double &transverse);
  double DiffusionSigmaCathode(double z) override;
  void setDiffusionCoefficientLongitudinal(double val) { longitudinalDiffusionCoefficient_ = val; }
  double DiffusionCoefficientLongitudinal() const { return longitudinalDiffusionCoefficient_; }
  void setDiffusionCoefficientTransverse(double val) { transverseDiffusionCoefficient_ = val; }
  double DiffusionCoefficientTransverse() const { return transverseDiffusionCoefficient_; }
  double DriftVelocity() const { return driftVelocity_; }
  void setDriftVelocity(double val) { driftVelocity_ = val; }

private:
  std::unique_ptr<VLArRecombinationModel> recombinationModel_;
  double longitudinalDiffusionCoefficient_ = 0.0;
  double transverseDiffusionCoefficient_ = 0.0;
  double driftVelocity_ = -1.0;
};
} // namespace comptonsoft
#endif //COMPTONSOFT_LArTPCDeviceSimlation_H