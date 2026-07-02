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

#include "CSEventStore.hh"
#include "DetectorHit_sptr.hh"
#include "DetectorSystem.hh"

using namespace anlnext;

namespace comptonsoft {

CSEventStore::CSEventStore()
{
  add_alias("CSEventStore");
}

CSEventStore::~CSEventStore() = default;

void CSEventStore::initialize_run(int runID, int num_events)
{
  VEventStore::initialize_run(runID, num_events);
  hits_vector_.resize(num_events);
  for (auto& hits: hits_vector_) {
    hits.clear();
  }
}

void CSEventStore::initialize_event(int eventID)
{
  VEventStore::initialize_event(eventID);
}

void CSEventStore::insertHit(const DetectorHit& hit)
{
  const size_t event_index = static_cast<size_t>(hit.EventID());
  hits_vector_[event_index].push_back(hit);
}

void CSEventStore::insertHit(DetectorHit&& hit)
{
  const size_t event_index = static_cast<size_t>(hit.EventID());
  hits_vector_[event_index].push_back(hit);
}

const std::vector<DetectorHit>& CSEventStore::getHits() const
{
  return hits_vector_[read_index()];
}

} // namespace comptonsoft
