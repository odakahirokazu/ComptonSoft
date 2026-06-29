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

#include "StandardUserActionAssembly.hh"

#include "G4Run.hh"
#include "G4Event.hh"
#include "VEventStore.hh"

using namespace anlnext;

namespace anlgeant4
{

StandardUserActionAssembly::StandardUserActionAssembly()
{
  add_alias("StandardUserActionAssembly");
}

ANLStatus StandardUserActionAssembly::mod_initialize()
{
  get_module_NC("VEventStore", &event_store_);
  return VUserActionAssembly::mod_initialize();
}

void StandardUserActionAssembly::run_action_at_beginning(const G4Run* run)
{
  const int runID = run->GetRunID();
  const int num_events = run->GetNumberOfEventToBeProcessed();
  event_store_->initialize_run(runID, num_events);
}

void StandardUserActionAssembly::run_action_at_end(const G4Run*)
{
  event_store_->finalize_run();
}

void StandardUserActionAssembly::event_action_at_beginning(const G4Event* event)
{
  const int eventID = event->GetEventID();
  current_event_id_ = eventID;
  event_store_->initialize_event(eventID);
}

void StandardUserActionAssembly::event_action_at_end(const G4Event*)
{
  event_store_->finalize_event();
}

void StandardUserActionAssembly::setInitialTime(size_t event_index, double v)
{
  event_store_->set_initial_time(event_index, v);
}

double StandardUserActionAssembly::getInitialTime() const
{
  return event_store_->initial_time();
}

} /* namespace anlgeant4 */
