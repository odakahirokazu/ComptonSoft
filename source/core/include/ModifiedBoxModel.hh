#ifndef COMPTONSOFT_ModifiedBoxModel_H
#define COMPTONSOFT_ModifiedBoxModel_H 1
#include "AstroUnits.hh"
#include "VLArRecombinationModel.hh"
#include <cmath>
namespace comptonsoft {
/**
 * A modified box model for recombination in LArTPC.
 * @author Shota Arai
 * @date 
 */
class ModifiedBoxModel: public VLArRecombinationModel {
public:
  ModifiedBoxModel();
  virtual ~ModifiedBoxModel() = default;

  virtual double electronLet(double let, double electricField) const override;
  void printInfo(std::ostream &os) const override;

private:
  // Constants for the modified box model
  // Reference: C. Adams, et al., "Calibration of the charge and energy loss per unit length of the MicroBooNE liquid argon time projection chamber using muons and protons", 2020, JINST, 15, P03022
  const double beta = 0.184 * (CLHEP::kilovolt / CLHEP::cm3 * CLHEP::g / CLHEP::MeV) / (CLHEP::volt / CLHEP::cm2 / CLHEP::mm * CLHEP::g / CLHEP::MeV); // in (V/mm)(g/cm2)/MeV
  const double alpha = 0.92; // dimensionless
  const double betaOverRho_ = beta / Rho(); // in V/MeV
};
} // namespace comptonsoft
#endif // COMPTONSOFT_ModifiedBoxModel_H