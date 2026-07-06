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

#include "RadioactivationEventStore.hh"
#include "DetectorSystem.hh"

using namespace anlnext;

namespace comptonsoft {

RadioactivationEventStore::RadioactivationEventStore() = default;

RadioactivationEventStore::~RadioactivationEventStore() = default;

void RadioactivationEventStore::initialize_run(int run_id, int num_events)
{
  CSEventStore::initialize_run(run_id, num_events);
  radioactivations_vector_.resize(num_events);
  for (auto& vec: radioactivations_vector_) {
    vec.clear();
  }
}

void RadioactivationEventStore::insert_radioactivation(size_t event_index, const RadioactivationInfo& info)
{
  radioactivations_vector_[event_index].push_back(info);
}

const std::vector<RadioactivationInfo>& RadioactivationEventStore::get_radioactivations() const
{
  return radioactivations_vector_[read_index()];
}

} // namespace comptonsoft
