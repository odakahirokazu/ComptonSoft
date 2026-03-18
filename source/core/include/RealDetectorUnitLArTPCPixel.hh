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

#ifndef COMPTONSOFT_RealDetectorUnitLArTPCPixel_H
#define COMPTONSOFT_RealDetectorUnitLArTPCPixel_H 1

#include "VRealDetectorUnit.hh"
#include "CLHEP/Units/SystemOfUnits.h"

namespace comptonsoft {

/**
 * A class of a LArTPCPixel detector unit.
 * @author Shota Arai
 * @date 2026-02-17
 * @date 2026-02-21 | added recombination correction
 */
class RealDetectorUnitLArTPCPixel: public VRealDetectorUnit {
public:
  RealDetectorUnitLArTPCPixel();
  virtual ~RealDetectorUnitLArTPCPixel();

  DetectorType Type() const override { return DetectorType::LArTPCPixel; }

  void setReadoutElectrode(ElectrodeSide v) { readoutElectrode_ = v; }
  ElectrodeSide ReadoutElectrode() const { return readoutElectrode_; }
  bool isAnodeReadout() const { return ReadoutElectrode() == ElectrodeSide::Anode; }
  bool isCathodeReadout() const { return ReadoutElectrode() == ElectrodeSide::Cathode; }
  bool isBottomSideReadout() const {
    return (ReadoutElectrode() != ElectrodeSide::Undefined &&
            ReadoutElectrode() == BottomSideElectrode());
  }
  bool isUpSideReadout() const {
    return (ReadoutElectrode() != ElectrodeSide::Undefined &&
            ReadoutElectrode() != BottomSideElectrode());
  }
  bool isRecombinationCorrectionEnabled() const { return isRecombinationCorrectionEnabled_; }
  bool setRecombinationCorrectionEnabled(bool v) { return isRecombinationCorrectionEnabled_ = v; }

  void setWion(double v) { Wion_ = v; }
  double Wion() const { return Wion_; }
  void setWexc(double v) { Wexc_ = v; }
  double Wexc() const { return Wexc_; }
  
  void setPhotonDetectionEfficiency(double v) { photonDetectionEfficiencyInv_ = 1.0 / v; }
  double PhotonDetectionEfficiency() const { return 1.0 / photonDetectionEfficiencyInv_; }
  void correctPhotonDetectionEfficiency(DetectorHitVector &hits) const;
  
  void printDetectorParameters(std::ostream& os) const override;

protected:
  bool setReconstructionDetails(int mode) override;
  void reconstruct(const DetectorHitVector &hitSignals,
                   DetectorHitVector &hitsReconstructed) override;

private:
  void determinePosition(DetectorHitVector &hits) const;
  void applyRecombinationCorrection(DetectorHitVector &hits);

private:
  ElectrodeSide readoutElectrode_;
  bool isRecombinationCorrectionEnabled_ = true;
  double Wion_ = 23.6 * CLHEP::eV; // Ionization energy in liquid argon
  double Wexc_ = 19.5 * CLHEP::eV; // Excitation energy in liquid argon
  double photonDetectionEfficiencyInv_ = 1.0; // Inverse of photon detection efficiency
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RealDetectorUnitLArTPCPixel_H */
