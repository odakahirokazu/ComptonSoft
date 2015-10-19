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

#include "PhotoAbsorptionEventReconstructionAlgorithm.hh"
#include "DetectorHit.hh"

namespace comptonsoft {

PhotoAbsorptionEventReconstructionAlgorithm::PhotoAbsorptionEventReconstructionAlgorithm()
{
}

PhotoAbsorptionEventReconstructionAlgorithm::~PhotoAbsorptionEventReconstructionAlgorithm() = default;

void PhotoAbsorptionEventReconstructionAlgorithm::initializeEvent()
{
}

bool PhotoAbsorptionEventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            BasicComptonEvent& eventReconstructed)
{
  const int NumHits = hits.size();
  if (NumHits > MaxHits()) {
    return false;
  }

  DetectorHit_sptr mergedHit(new DetectorHit);
  for (auto& hit: hits) {
    mergedHit->merge(*hit);
  }

  eventReconstructed.setHit1(0, mergedHit);
  eventReconstructed.setNumberOfHits(NumHits);

  return false;
}

} /* namespace comptonsoft */
