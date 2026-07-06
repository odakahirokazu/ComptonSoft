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

#include "ObservationPickUpData.hh"

#include <algorithm>
#include "G4Event.hh"
#include "G4Track.hh"
#include "ObservationEventStore.hh"

using namespace anlnext;

namespace comptonsoft
{

ObservationPickUpData::ObservationPickUpData()
{
}

ANLStatus ObservationPickUpData::mod_define()
{
  define_parameter("record_primaries", &mod_class::record_primaries_);
  define_parameter("particle_selection", &mod_class::particle_selection_);

  return AS_OK;
}

ANLStatus ObservationPickUpData::mod_initialize()
{
  VUserActionAssembly::mod_initialize();

  get_module_NC("ObservationEventStore", &event_store_);

  return AS_OK;
}

void ObservationPickUpData::event_action_at_beginning(const G4Event* event)
{
  current_event_id_ = event->GetEventID();
}

void ObservationPickUpData::track_action_at_end(const G4Track* track)
{
  if (track->GetNextVolume() == 0) {
    const int PDGEncoding = track->GetDefinition()->GetPDGEncoding();
    if (particle_selection_.empty() ||
        std::find(particle_selection_.begin(), particle_selection_.end(), PDGEncoding)!=particle_selection_.end()) {
      const int track_id = track->GetTrackID();
      if (record_primaries_ || track_id > 1) {
        ObservedParticle data;
        data.trackid = track_id;
        data.particle = PDGEncoding;
        data.time = track->GetGlobalTime();
        data.position = track->GetPosition();
        data.energy = track->GetKineticEnergy();
        data.direction = track->GetMomentumDirection();
        data.polarization = track->GetPolarization();
        event_store_->insert_observed_particle(current_event_id_, data);
      }
    }
  }
}

} /* namespace comptonsoft */
