#ifndef COMPTONSOFT_LArTPCDeviceSimulation_H
#define COMPTONSOFT_LArTPCDeviceSimulation_H 1
#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include "DeviceSimulation.hh"
#include "EFieldModel.hh"
#include "TFile.h"
#include "TSpline.h"
#include "VLArRecombinationModel.hh"
#include "LArRecombinationModelFactory.hh"
#include <memory>
#include <unordered_map>
#include "G4EmCalculator.hh"
#include "G4EmCalculator.hh"

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
 * @date 2026-04-18 | detached recombination correction with charge only and reimplemented it to RealDetectorUnitLArTPC / RealDetectorUnitLArTPCPixel
 */
class LArTPCDeviceSimulation: public DeviceSimulation {
public:
  LArTPCDeviceSimulation();
  virtual ~LArTPCDeviceSimulation() = default;

  void instantiateRecombinationModel(const std::string &config_filename) override;
  void applyRecombination(DetectorHit_sptr &hit) override;
  void printSimulationParameters(std::ostream &os) const override;
  double DiffusionSigmaAnode3D(double z, double &longitudinal, double &transverse);
  double DiffusionSigmaCathode(double z) override;
  void setDiffusionCoefficientLongitudinal(double val) { longitudinalDiffusionCoefficient_ = val; }
  double DiffusionCoefficientLongitudinal() const { return longitudinalDiffusionCoefficient_; }
  void setDiffusionCoefficientTransverse(double val) { transverseDiffusionCoefficient_ = val; }
  double DiffusionCoefficientTransverse() const { return transverseDiffusionCoefficient_; }
  double DriftVelocity() const { return driftVelocity_; }
  void setDriftVelocity(double val) { driftVelocity_ = val; }
  double getdEdxFromKineticEnergy(const DetectorHit_sptr& hit) const;
  void setdEdxFile(const std::string &filename, const std::string &spline_name="dedx_spline");
  TSpline *getdEdxSpline() const { return dedxSpline_; }
  int dEdxMode() const { return dedxMode_; }
  void setdEdxMode(int mode) { dedxMode_ = mode; }

  void makeRawDetectorHits() override;
  const VLArRecombinationModel* recombinationModel() const override { return recombinationModel_.get(); }

private:
  std::unique_ptr<VLArRecombinationModel> recombinationModel_;
  double longitudinalDiffusionCoefficient_ = 0.0;
  double transverseDiffusionCoefficient_ = 0.0;
  double driftVelocity_ = -1.0;
  // mutable because ComputeDEDX is not a const method. Note that G4EmCalculator
  // is not thread-safe, so this class must not be shared between worker threads.
  mutable G4EmCalculator calculator_;
  mutable std::unordered_map<const G4ParticleDefinition*, G4VEnergyLossProcess*> ionisationProcessCache_;


  TFile *dedxFile_ = nullptr;
  TSpline *dedxSpline_ = nullptr;
  int dedxMode_ = 0; // 0: from step information, 1: from kinetic energy, 2; from G4EmCalculator
  std::vector<int> zIndicesForEPI_;

  double getdEdxFromEmCalculator(const DetectorHit_sptr& hit) const;

  static double modelLowEnergyLimit(const G4VEnergyLossProcess* process, const G4MaterialCutsCouple* couple,
                                    double kineticEnergy);

  void printdEdxDiagnostics(const DetectorHit_sptr& hit, double dedx, std::ostream& os) const;
};
} // namespace comptonsoft
#endif //COMPTONSOFT_LArTPCDeviceSimulation_H