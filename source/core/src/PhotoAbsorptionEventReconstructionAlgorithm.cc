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
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  const int NumHits = hits.size();
  if (NumHits > MaxHits()) {
    return false;
  }

  DetectorHit_sptr mergedHit(new DetectorHit);
  for (auto& hit: hits) {
    mergedHit->merge(*hit);
  }

  auto event = std::make_shared<BasicComptonEvent>(baseEvent);
  event->setHit1(0, mergedHit);
  event->setNumberOfHits(NumHits);
  event->setTotalEnergyDeposit(total_energy_deposits(hits));

  eventsReconstructed.push_back(event);
  return true;
}

} /* namespace comptonsoft */
