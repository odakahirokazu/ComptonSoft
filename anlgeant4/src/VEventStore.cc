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

#include "VEventStore.hh"

using namespace anlnext;

namespace anlgeant4
{

VEventStore::VEventStore()
  : InitialInformation(true, this)
{
  add_alias("VEventStore");
  add_alias("Geant4EventStore");
}

VEventStore::~VEventStore() = default;

void VEventStore::initializeRun(int runID, int num_events)
{
  eventid_vector_.resize(num_events, 0);
  read_index_ = 0;
  postprocess_ready_ = false;

  InitialInformation::initializeRun(runID, num_events);
}

void VEventStore::finalizeRun()
{
  postprocess_ready_ = true;
}

void VEventStore::initializeEvent(int eventID)
{
  eventid_vector_[eventID] = eventID;
  InitialInformation::initializeEvent(eventID);
}

ANLStatus VEventStore::mod_analyze()
{
  if (postprocess_ready_) {
    read_index_++;
  }

  InitialInformation::set_read_index(read_index_);

  if (read_index_ < eventid_vector_.size()) {
    set_evs("Geant4Body:DataStored");
  }

  return AS_OK;
}

} /* namespace anlgeant4 */
