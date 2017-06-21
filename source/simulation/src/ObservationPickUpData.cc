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

using namespace anl;

namespace comptonsoft
{

ObservationPickUpData::ObservationPickUpData()
  : recordPrimaries_(true)
{
  SetStepActOn(false);
}

ANLStatus ObservationPickUpData::mod_startup()
{
  register_parameter(&recordPrimaries_, "record_primaries");
  register_parameter(&particleSelection_, "particle_selection");

  return AS_OK;
}

void ObservationPickUpData::EventAct_begin(const G4Event* event)
{
  StandardPickUpData::EventAct_begin(event);
  particleVector_.clear();
}

void ObservationPickUpData::TrackAct_end(const G4Track* track)
{
  if (track->GetNextVolume() == 0) {
    const int PDGEncoding = track->GetDefinition()->GetPDGEncoding();
    if (particleSelection_.empty() ||
        std::find(particleSelection_.begin(), particleSelection_.end(), PDGEncoding)!=particleSelection_.end()) {
      const int trackID = track->GetTrackID();
      if (recordPrimaries_ || trackID > 1) {
        ObservedParticle_sptr data(new ObservedParticle);
        data->trackid = trackID;
        data->particle = PDGEncoding;
        data->time = track->GetGlobalTime();
        data->position = track->GetPosition();
        data->energy = track->GetKineticEnergy();
        data->direction = track->GetMomentumDirection();
        data->polarization = track->GetPolarization();
        particleVector_.push_back(data);
      }
    }
  }
}

} /* namespace comptonsoft */
