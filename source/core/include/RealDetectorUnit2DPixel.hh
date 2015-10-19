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

#ifndef COMPTONSOFT_RealDetectorUnit2DPixel_H
#define COMPTONSOFT_RealDetectorUnit2DPixel_H 1

#include "VRealDetectorUnit.hh"

namespace comptonsoft {

/**
 * A class of a 2D-pad semicondutor detector unit.
 * @author Hirokazu Odaka
 * @date 2008-08-25
 * @date 2014-11-20
 */
class RealDetectorUnit2DPixel : public VRealDetectorUnit
{
public:
  RealDetectorUnit2DPixel();
  ~RealDetectorUnit2DPixel();

  DetectorType Type() const { return DetectorType::PixelDetector; }

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

protected:
  virtual bool setReconstructionDetails(int mode);
  virtual void reconstruct(const DetectorHitVector& hitSignals,
                           DetectorHitVector& hitsReconstructed);

private:
  void determinePosition(DetectorHitVector& hits) const;

private:
  ElectrodeSide readoutElectrode_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RealDetectorUnit2DPixel_H */
