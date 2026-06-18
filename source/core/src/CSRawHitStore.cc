/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Shin Watanabe, Hirokazu Odaka                      *
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

#include "CSRawHitStore.hh"
#include "DetectorHit_sptr.hh"
#include "DetectorSystem.hh"

using namespace anlnext;

namespace comptonsoft {

CSRawHitStore::CSRawHitStore() = default;

CSRawHitStore::~CSRawHitStore() = default;

void CSRawHitStore::initializeRun(int runID, int num_events)
{
  VEventStore::initializeRun(runID, num_events);
  hits_vector_.resize(num_events);
  for (auto& hits: hits_vector_) {
    hits.clear();
  }
}

void CSRawHitStore::initializeEvent(int eventID)
{
  VEventStore::initializeEvent(eventID);
}

void CSRawHitStore::insertHit(const DetectorHit& hit)
{
  const size_t event_index = static_cast<size_t>(hit.EventID());
  hits_vector_[event_index].push_back(hit);
}

void CSRawHitStore::insertHit(DetectorHit&& hit)
{
  const size_t event_index = static_cast<size_t>(hit.EventID());
  hits_vector_[event_index].push_back(hit);
}

const std::vector<DetectorHit>& CSRawHitStore::getHits() const
{
  return hits_vector_[read_index()];
}

} // namespace comptonsoft
