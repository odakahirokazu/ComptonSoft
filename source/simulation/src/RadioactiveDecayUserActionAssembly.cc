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

#include "RadioactiveDecayUserActionAssembly.hh"
#include "AstroUnits.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4EventManager.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

RadioactiveDecayUserActionAssembly::RadioactiveDecayUserActionAssembly()
  : termination_time_(1.0*unit::ms),
    radioactive_decay_process_name_("RadioactiveDecay"),
    first_decay_time_(0.0)
{
}

ANLStatus RadioactiveDecayUserActionAssembly::mod_define()
{
  define_parameter("termination_time", &mod_class::termination_time_, unit::s, "s");
  define_parameter("radioative_decay_process_name", &mod_class::radioactive_decay_process_name_);

  return AS_OK;
}

void RadioactiveDecayUserActionAssembly::SteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  const double global_time = track->GetGlobalTime();

  if (track->GetTrackID()==1 && track->GetCurrentStepNumber()==1) {
    const G4Event* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
    const int event_id = event->GetEventID();

    const G4String process_name
      = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    if (process_name == radioactive_decay_process_name_) {
      first_decay_time_ = global_time;
      setInitialTime(event_id, first_decay_time_);
    }
    else {
      throw ANLException("RadioactiveDecayUserActionAssembly:Error---First step is not radioactive decay.");
    }
  }

  const double time_from_first_decay = global_time - first_decay_time_;
  if (time_from_first_decay > termination_time_) {
    track->SetTrackStatus(fKillTrackAndSecondaries);
    return;
  }
}

} /* namespace comptonsoft */
