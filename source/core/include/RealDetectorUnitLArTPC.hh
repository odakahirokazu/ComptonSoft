/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#ifndef COMPTONSOFT_RealDetectorUnitLArTPC_H
#define COMPTONSOFT_RealDetectorUnitLArTPC_H 1

#include "VRealDetectorUnit.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "VGainFunction.hh"
#include "TH1I.h"
#include "TFile.h"

class TH1I;
class TFile;
namespace comptonsoft {
class VGainFunction;

/**
 * A class of a LArTPC detector unit.
 * @author Shota Arai
 * @date 2025-06-27
 * @date 2025-10-10 | defined DetectorType as LArTPC
 * @date 2026-02-21 | added recombination correction
 * @date 2026-04-18 | implemented recombination correction with charge only
 */
class RealDetectorUnitLArTPC : public VRealDetectorUnit
{
public:
  RealDetectorUnitLArTPC();
  virtual ~RealDetectorUnitLArTPC();

  DetectorType Type() const override
  { return DetectorType::LArTPC; }

  void setReadoutElectrode(ElectrodeSide v)
  { readoutElectrode_ = v; }
  ElectrodeSide ReadoutElectrode() const
  { return readoutElectrode_; }
  bool isAnodeReadout() const
  { return ReadoutElectrode()==ElectrodeSide::Anode; }
  bool isCathodeReadout() const
  { return ReadoutElectrode()==ElectrodeSide::Cathode; }
  bool isBottomSideReadout() const
  {
    return (ReadoutElectrode()!=ElectrodeSide::Undefined &&
            ReadoutElectrode()==BottomSideElectrode());
  }
  bool isUpSideReadout() const
  {
    return (ReadoutElectrode()!=ElectrodeSide::Undefined &&
            ReadoutElectrode()!=BottomSideElectrode());
  }
  int recombinationCorrectionMode() const { return recombinationCorrectionMode_; }
  void setRecombinationCorrectionMode(int v) { recombinationCorrectionMode_ = v; }
  
  void setWion(double v) { Wion_ = v; }
  double Wion() const { return Wion_; }
  void setWexc(double v) { Wexc_ = v; }
  double Wexc() const { return Wexc_; }
  void setPhotonDetectionEfficiency(double v) { photonDetectionEfficiencyInv_ = 1.0 / v; }
  double PhotonDetectionEfficiency() const { return 1.0 / photonDetectionEfficiencyInv_; }
  void correctPhotonDetectionEfficiency(DetectorHitVector &hits) const;
  
  void printDetectorParameters(std::ostream& os) const override;
  void applyRecombinationCorrection(DetectorHitVector &hits) const override;
  void setRecombinationCorrectionFile(const std::string &filename, const std::string &meta_tree_name);

protected:
  bool setReconstructionDetails(int mode) override;
  void reconstruct(const DetectorHitVector& hitSignals,
                   DetectorHitVector& hitsReconstructed) override;

private:
  void determinePosition(DetectorHitVector& hits) const;
  std::tuple<double, double> applyRecombinationCorrectionWithLight(const DetectorHit_sptr &hit) const;
  std::tuple<double, double> applyRecombinationCorrectionWithCorrectionFile(const DetectorHit_sptr &hit) const;

private:
  ElectrodeSide readoutElectrode_;
  int recombinationCorrectionMode_ = 1; // 0: off, 1: using light and charge, 2: using charge only
  std::vector<VGainFunction *>recombinationCorrectionFunction_;
  TH1I *recombinationCorrectionZMapping_ = nullptr;
  TFile *recombinationCorrectionFile_ = nullptr;
  
  double Wion_ = 23.6 * CLHEP::eV; // Ionization energy in liquid argon
  double Wexc_ = 19.5 * CLHEP::eV; // Excitation energy in liquid argon
  double photonDetectionEfficiencyInv_ = 1.0; // Inverse of photon detection efficiency
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RealDetectorUnitLArTPC_H */
