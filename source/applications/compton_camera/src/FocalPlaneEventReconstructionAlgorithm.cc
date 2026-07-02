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

#include "FocalPlaneEventReconstructionAlgorithm.hh"
#include <list>
#include <algorithm>
#include <numeric>
#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"

namespace comptonsoft {

FocalPlaneEventReconstructionAlgorithm::FocalPlaneEventReconstructionAlgorithm()
{
}

FocalPlaneEventReconstructionAlgorithm::~FocalPlaneEventReconstructionAlgorithm() = default;

void FocalPlaneEventReconstructionAlgorithm::initializeEvent()
{
}

bool FocalPlaneEventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  const int NumHits = hits.size();
  if (NumHits > MaxHits()) {
    return false;
  }

  bool result = false;
  auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);
  eventReconstructed->setTotalEnergyDeposit(total_energy_deposits(hits));

  if (NumHits == 1) {
    eventReconstructed->setHit1(0, hits[0]);
    eventReconstructed->setNumberOfHits(1);
    result = true;
  }
  else if (NumHits == 2) {
    const bool hit0LowZ = hits[0]->isFlags(flag::LowZHit);
    const bool hit1LowZ = hits[1]->isFlags(flag::LowZHit);
    const bool hit0HighZ = hits[0]->isFlags(flag::HighZHit);
    const bool hit1HighZ = hits[1]->isFlags(flag::HighZHit);

    if (hit0LowZ && hit1LowZ) {
      result = reconstruct2HitsLL(hits, *eventReconstructed);
    }
    else if (hit0HighZ && hit1HighZ) {
      result = reconstruct2HitsHH(hits, *eventReconstructed);
    }
    else if (hit0LowZ && hit1HighZ) {
      result = reconstruct2HitsLH(hits, *eventReconstructed);
    }
    else if (hit0HighZ && hit1LowZ) {
      std::vector<DetectorHit_sptr> hitsOrdered(2);
      hitsOrdered[0] = hits[1];
      hitsOrdered[1] = hits[0];
      result = reconstruct2HitsLH(hitsOrdered, *eventReconstructed);
    }
  }

  if (result) {
    eventsReconstructed.push_back(eventReconstructed);
  }
  
  return result;
}

bool FocalPlaneEventReconstructionAlgorithm::
reconstruct2HitsLH(const std::vector<DetectorHit_sptr>& hits,
                   BasicComptonEvent& eventReconstructed)
{
  DetectorHit_sptr hitLowZ = hits[0];
  DetectorHit_sptr hitHighZ = hits[1];
  if (hitLowZ->isFlags(flag::FluorescenceHit)) {
    eventReconstructed.setHit1(0, hitHighZ);
    eventReconstructed.setHit2(1, hitLowZ);
  }
  else {
    return false;
  }
  
  eventReconstructed.setNumberOfHits(2);
  return true;
}

bool FocalPlaneEventReconstructionAlgorithm::
reconstruct2HitsHH(const std::vector<DetectorHit_sptr>& /* hits */,
                   BasicComptonEvent& /* eventReconstructed */)
{
  return false;
}

bool FocalPlaneEventReconstructionAlgorithm::
reconstruct2HitsLL(const std::vector<DetectorHit_sptr>& /* hits */,
                   BasicComptonEvent& /* eventReconstructed */)
{
  return false;
}

} /* namespace comptonsoft */
