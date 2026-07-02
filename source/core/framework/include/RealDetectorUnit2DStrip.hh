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

#ifndef COMPTONSOFT_RealDetectorUnit2DStrip_H
#define COMPTONSOFT_RealDetectorUnit2DStrip_H 1

#include "VRealDetectorUnit.hh"

namespace comptonsoft {

/**
 * A class of a 2D-strip semicondutor detector unit.
 * @author Hirokazu Odaka
 * @date 2008-08-25
 * @date 2014-10-08
 * @date 2015-06-13 | check consistency of energies of both sides
 */
class RealDetectorUnit2DStrip : public VRealDetectorUnit
{
public:
  enum class HitSelectionMethod_t { OneByOne, MaxPulseHeight };

public:
  RealDetectorUnit2DStrip();
  virtual ~RealDetectorUnit2DStrip();

  DetectorType Type() const override
  { return DetectorType::DoubleSidedStripDetector; }

  void initializeEvent() override;

  void setPrioritySide(ElectrodeSide v) { prioritySide_ = v; }
  ElectrodeSide PrioritySide() const { return prioritySide_; }
  bool PriorityToAnodeSide() const
  { return PrioritySide()==ElectrodeSide::Anode; }
  bool PriorityToCathodeSide() const
  { return PrioritySide()==ElectrodeSide::Cathode; }

  void setXStripSideElectrode(ElectrodeSide v) { xStripSideElectrode_ = v; }
  ElectrodeSide XStripSideElectrode() const { return xStripSideElectrode_; }
  bool isXStripSideAnode() const
  { return XStripSideElectrode()==ElectrodeSide::Anode; }
  bool isXStripSideCathode() const
  { return XStripSideElectrode()==ElectrodeSide::Cathode; }
  bool isYStripSideAnode() const
  { return isXStripSideCathode(); }
  bool isYStripSideCathode() const
  { return isXStripSideAnode(); }

  bool isBottomSideXStrip() const
  {
    return (XStripSideElectrode()!=ElectrodeSide::Undefined &&
            XStripSideElectrode()==BottomSideElectrode());
  }
  bool isUpSideXStrip() const
  {
    return (XStripSideElectrode()!=ElectrodeSide::Undefined &&
            XStripSideElectrode()!=BottomSideElectrode());
  }

  bool isBottomSideYStrip() const
  { return isUpSideXStrip(); }
  bool isUpSideYStrip() const
  { return isBottomSideXStrip(); }
  
  int NumberOfCathodeSideHits() const { return cathodeSideHits_.size(); }
  DetectorHit_sptr getCathodeSideHit(int i) { return cathodeSideHits_[i]; }
  int NumberOfAnodeSideHits() const { return anodeSideHits_.size(); }
  DetectorHit_sptr getAnodeSideHit(int i) { return anodeSideHits_[i]; }

  void setHitSelectionMethod(HitSelectionMethod_t v)
  { hitSelectionMethod_ = v; }
  HitSelectionMethod_t HitSelectionMethod() const
  { return hitSelectionMethod_; }

  void requireEnergyConsistency(bool v=true)
  { energyConsistencyRequired_ = v; }
  
  bool isEnergyConsistencyRequired() const
  { return energyConsistencyRequired_; }

  void setEnergyConsistencyCheckFunctions(double lower_c0,
                                          double lower_c1,
                                          double upper_c0,
                                          double upper_c1)
  {
    lowerEnergyConsistencyCheckFunctionC0_ = lower_c0;
    lowerEnergyConsistencyCheckFunctionC1_ = lower_c1;
    upperEnergyConsistencyCheckFunctionC0_ = upper_c0;
    upperEnergyConsistencyCheckFunctionC1_ = upper_c1;
  }
  
  double LowerEnergyConsistencyCheckFunctionC0() const
  { return lowerEnergyConsistencyCheckFunctionC0_; }
  double LowerEnergyConsistencyCheckFunctionC1() const
  { return lowerEnergyConsistencyCheckFunctionC1_; }
  double UpperEnergyConsistencyCheckFunctionC0() const
  { return upperEnergyConsistencyCheckFunctionC0_; }
  double UpperEnergyConsistencyCheckFunctionC1() const
  { return upperEnergyConsistencyCheckFunctionC1_; }

  bool checkEnergyConsistency(double energy_x, double energy_y);
  
protected:
  bool setReconstructionDetails(int mode) override;
  void reconstruct(const DetectorHitVector& hitSignals,
                   DetectorHitVector& hitReconstructed) override;
  virtual void reconstructDoubleSides(const DetectorHitVector& hitsCathode,
                                      const DetectorHitVector& hitsAnode,
                                      DetectorHitVector& hitReconstructed);
  void reconstructWithMaxPulseHeightMethod(const DetectorHitVector& hitsCathode,
                                           const DetectorHitVector& hitsAnode,
                                           DetectorHitVector& hitReconstructed);
  void reconstructWithOneByOneMethod(const DetectorHitVector& hitsCathode,
                                     const DetectorHitVector& hitsAnode,
                                     DetectorHitVector& hitReconstructed);
  
  virtual void cluster1Dimension(DetectorHitVector& hits) const;
  DetectorHit_sptr selectMaxEPI(const DetectorHitVector& hits) const;
  DetectorHit_sptr mergeDoubleSides(const DetectorHit_sptr hitCathode,
                                    const DetectorHit_sptr hitAnode) const;

private:
  ElectrodeSide prioritySide_;
  ElectrodeSide xStripSideElectrode_;
  HitSelectionMethod_t hitSelectionMethod_;

  DetectorHitVector cathodeSideHits_;
  DetectorHitVector anodeSideHits_;

  // consistency check between energies (EPI) of cathode and anode
  bool energyConsistencyRequired_;
  double lowerEnergyConsistencyCheckFunctionC0_;
  double lowerEnergyConsistencyCheckFunctionC1_;
  double upperEnergyConsistencyCheckFunctionC0_;
  double upperEnergyConsistencyCheckFunctionC1_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RealDetectorUnit2DStrip_H */
