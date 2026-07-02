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

#ifndef COMPTONSOFT_RealDetectorUnitScintillator_H
#define COMPTONSOFT_RealDetectorUnitScintillator_H 1

#include "VRealDetectorUnit.hh"

namespace comptonsoft {

/**
 * A class of a scintillator unit.
 * @author Hirokazu Odaka
 * @date 2008-09-17
 * @date 2014-10-08
 */
class RealDetectorUnitScintillator : public VRealDetectorUnit
{
public:
  RealDetectorUnitScintillator();
  virtual ~RealDetectorUnitScintillator();

  DetectorType Type() const override
  { return DetectorType::Scintillator; }

  void setSize(double x, double y, double z) override
  {
    VRealDetectorUnit::setSize(x, y, z);
    VRealDetectorUnit::setPixelPitch(x, y);
  }

  void reconstruct(const DetectorHitVector& hitSignals,
                   DetectorHitVector& hitsReconstructed) override;

private:
  void determinePosition(DetectorHitVector& hits);
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RealDetectorUnitScintillator_H */
