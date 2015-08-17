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

#include "RealDetectorUnit2DPad.hh"
#include <list>
#include <algorithm>
#include <iterator>
#include <utility>
#include "DetectorHit.hh"

namespace comptonsoft {

RealDetectorUnit2DPad::RealDetectorUnit2DPad()
  : readoutElectrode_(ElectrodeSide::Undefined)
{
}

RealDetectorUnit2DPad::~RealDetectorUnit2DPad() = default;

void RealDetectorUnit2DPad::reconstruct(const DetectorHitVector& hitSignals,
                                        DetectorHitVector& hitsReconstructed)
{
  std::transform(hitSignals.begin(), hitSignals.end(),
                 std::back_inserter(hitsReconstructed),
                 [](const DetectorHit_sptr& hit) {
                   auto hit2 = hit->clone();
                   hit2->setEnergy(hit->EPI());
                   return std::move(hit2);
                 });
  determinePosition(hitsReconstructed);
  if (isClusteringOn()) cluster(hitsReconstructed);
}

void RealDetectorUnit2DPad::determinePosition(DetectorHitVector& hits) const
{
  for (auto& hit: hits) {
    PixelID pixel = hit->Pixel();
    hit->setPosition(Position(pixel));
    hit->setLocalPosition(LocalPosition(pixel));
  }
}

} /* namespace comptonsoft */
