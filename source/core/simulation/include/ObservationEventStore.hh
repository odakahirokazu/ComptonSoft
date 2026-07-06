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

#ifndef COMPTONSOFT_ObservationEventStore_H
#define COMPTONSOFT_ObservationEventStore_H 1

#include "CSEventStore.hh"

#include <G4ThreeVector.hh>
#include "ObservedParticle.hh"

namespace comptonsoft
{

/**
 * Raw hit store module for ComptonSoft
 * @author Hirokazu Odaka
 * @date 2026-07-06 | Hirokazu Odaka
 */
class ObservationEventStore : public CSEventStore
{
  DEFINE_ANL_MODULE(ObservationEventStore, 1.0);
public:
  ObservationEventStore();
  virtual ~ObservationEventStore();

  void initialize_run(int run_id, int num_events) override;

  void insert_observed_particle(size_t event_index, const ObservedParticle& observed_particle);
  const std::vector<ObservedParticle>& get_observed_particles() const;

private:
  std::vector<std::vector<ObservedParticle>> observations_vector_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ObservationEventStore_H */
