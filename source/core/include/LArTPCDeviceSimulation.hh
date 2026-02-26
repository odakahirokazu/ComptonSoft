#ifndef COMPTONSOFT_LArTPCDeviceSimulation_H
#define COMPTONSOFT_LArTPCDeviceSimulation_H 1
#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include "DeviceSimulation.hh"
#include "EFieldModel.hh"
#include "TFile.h"
#include "TSpline.h"
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
  std::tuple<double, double> compensateEPI(const PixelID &sp, std::tuple<double, double> ePI) const override;
  void setResponseFileForEPICompensation(const std::string &filename, const std::string &graph_name = "g_Etrue_mean_pix");
  const std::vector<TGraph *> &getResponseGraphListForEPICompensation() const { return responseGraphListForEPI_; }
  double getdEdxFromKineticEnergy(double kineticEnergy) const;
  void setdEdxFile(const std::string &filename, const std::string &spline_name="dedx_spline");
  TSpline *getdEdxSpline() const { return dedxSpline_; }
  



  const VLArRecombinationModel* recombinationModel() const override { return recombinationModel_.get(); }

private:
  std::unique_ptr<VLArRecombinationModel> recombinationModel_;
  double longitudinalDiffusionCoefficient_ = 0.0;
  double transverseDiffusionCoefficient_ = 0.0;
  double driftVelocity_ = -1.0;

  TFile *responseFileForEPI_ = nullptr;
  std::vector<TGraph *> responseGraphListForEPI_;
  TFile *dedxFile_ = nullptr;
  TSpline *dedxSpline_ = nullptr;
  std::vector<int> zIndicesForEPI_;
};
} // namespace comptonsoft
#endif //COMPTONSOFT_LArTPCDeviceSimlation_H