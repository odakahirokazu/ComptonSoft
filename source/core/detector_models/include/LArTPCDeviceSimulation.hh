#ifndef COMPTONSOFT_LArTPCDeviceSimulation_H
#define COMPTONSOFT_LArTPCDeviceSimulation_H 1
#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include "DeviceSimulation.hh"
#include "EFieldModel.hh"
#include "G4EmCalculator.hh"
#include "LArRecombinationModelFactory.hh"
#include "TFile.h"
#include "TSpline.h"
#include "VLArRecombinationModel.hh"
#include <memory>
#include <unordered_map>

class G4MaterialCutsCouple;
class G4ParticleDefinition;
class G4VEnergyLossProcess;

namespace comptonsoft {
class DetectorHit;
class EFieldModel;
class VLArRecombinationModel;
/**
 * A class of device simulation for LArTPC detectors.
 * @author Shota Arai
 * @date 2025-10-10 | first implementation
 * @date 2026-04-18 | detached recombination correction with charge only and reimplemented it to RealDetectorUnitLArTPC
 * / RealDetectorUnitLArTPCPixel
 */
class LArTPCDeviceSimulation : public DeviceSimulation
{
public:
  LArTPCDeviceSimulation();
  virtual ~LArTPCDeviceSimulation() = default;

  void setRecombinationModel(std::unique_ptr<VLArRecombinationModel>&& model) override
  {
    recombinationModel_ = std::move(model);
  }
  void applyRecombination(DetectorHit_sptr& hit) const override;
  void printSimulationParameters(std::ostream& os) const override;
  double DiffusionSigmaAnode3D(double z, double& longitudinal, double& transverse) const;
  double DiffusionSigmaCathode(double z) const override;
  void setDiffusionCoefficientLongitudinal(double val) { longitudinalDiffusionCoefficient_ = val; }
  double DiffusionCoefficientLongitudinal() const { return longitudinalDiffusionCoefficient_; }
  void setDiffusionCoefficientTransverse(double val) { transverseDiffusionCoefficient_ = val; }
  double DiffusionCoefficientTransverse() const { return transverseDiffusionCoefficient_; }
  double DriftVelocity() const { return driftVelocity_; }
  void setDriftVelocity(double val) { driftVelocity_ = val; }

  double getdEdxFromKineticEnergy(DetectorHit_sptr& hit) const;

  void setdEdxFile(const std::string& filename, const std::string& spline_name = "dedx_spline");
  TSpline* getdEdxSpline() const { return dedxSpline_; }
  int dEdxMode() const { return dedxMode_; }
  void setdEdxMode(int mode) { dedxMode_ = mode; }
  bool IsDedxAveraged() const { return isDedxAveraged_; }
  void setDedxAveraged(bool v) { isDedxAveraged_ = v; }
  double DedxWindowLength() const { return dEdxWindowLength_; }
  void setDedxWindowLength(double v) { dEdxWindowLength_ = v; }
  int RecombinationGroupMode() const { return recombinationGroupMode_; }
  void setRecombinationGroupMode(int mode) { recombinationGroupMode_ = mode; }
  double ElectricFieldForRecombination() const { return BiasVoltage() / getThickness(); }

  void makeRawDetectorHits() override;
  const VLArRecombinationModel* recombinationModel() const override { return recombinationModel_.get(); }
  VLArRecombinationModel* recombinationModel() { return recombinationModel_.get(); }

  virtual void assignLocalDepth(DetectorHit_sptr hit) const override;
  virtual void assignLocalPositionError(DetectorHit_sptr hit) const override;

protected:
  void calculateEdepStepLengthAverage(const DetectorHitVector& hits);
  // depth sensing mode 2 requires diffusion mode 3; throws otherwise.
  void checkLongitudinalDiffusionMode() const;

private:
  std::unique_ptr<VLArRecombinationModel> recombinationModel_;
  double longitudinalDiffusionCoefficient_ = 0.0;
  double transverseDiffusionCoefficient_ = 0.0;
  double driftVelocity_ = -1.0;
  // mutable because ComputeDEDX is not a const method.
  mutable G4EmCalculator* calculator_;
  mutable std::unordered_map<const G4ParticleDefinition*, G4VEnergyLossProcess*> ionisationProcessCache_;

  TFile* dedxFile_ = nullptr;
  TSpline* dedxSpline_ = nullptr;
  bool isDedxAveraged_ = false;
  int dedxMode_ = 0; // 0: from step information, 1: from kinetic energy using dEdx file, 2: from kinetic energy using
                     // EMcalculator, 3: from kinetic energy averaged by window, 4: from edep/length averaged by window.
  double dEdxWindowLength_ = 0.0;
  int recombinationGroupMode_ = 0; // 0: per step, 1: per group of a primary electron and its charged descendants
  std::vector<std::pair<double, double>> averagedEdepStepLength_;
  std::vector<int> zIndicesForEPI_;

  double getdEdxFromEmCalculator(const DetectorHit_sptr& hit) const;

  static double modelLowEnergyLimit(const G4VEnergyLossProcess* process, const G4MaterialCutsCouple* couple,
                                    double kineticEnergy);

  void printdEdxDiagnostics(const DetectorHit_sptr& hit, double dedx, std::ostream& os) const;
};
} // namespace comptonsoft
#endif // COMPTONSOFT_LArTPCDeviceSimulation_H