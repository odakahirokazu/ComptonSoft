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

#include "ObservationEventStore.hh"

using namespace anlnext;

namespace comptonsoft {

ObservationEventStore::ObservationEventStore() = default;

ObservationEventStore::~ObservationEventStore() = default;

void ObservationEventStore::initialize_run(int run_id, int num_events)
{
  CSEventStore::initialize_run(run_id, num_events);
  observations_vector_.resize(num_events);
  for (auto& vec: observations_vector_) {
    vec.clear();
  }
}

void ObservationEventStore::insert_observed_particle(size_t event_index, const ObservedParticle& observed_particle)
{
  observations_vector_[event_index].push_back(observed_particle);
}

const std::vector<ObservedParticle>& ObservationEventStore::get_observed_particles() const
{
  return observations_vector_[read_index()];
}

} // namespace comptonsoft
