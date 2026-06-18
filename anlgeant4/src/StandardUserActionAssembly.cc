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

void StandardUserActionAssembly::RunActionAtBeginning(const G4Run* run)
{
  const int runID = run->GetRunID();
  const int num_events = run->GetNumberOfEventToBeProcessed();
  event_store_->initializeRun(runID, num_events);
}

void StandardUserActionAssembly::RunActionAtEnd(const G4Run*)
{
  event_store_->finalizeRun();
}

void StandardUserActionAssembly::EventActionAtBeginning(const G4Event* event)
{
  const int eventID = event->GetEventID();
  event_store_->initializeEvent(eventID);
}

void StandardUserActionAssembly::EventActionAtEnd(const G4Event*)
{
  event_store_->finalizeEvent();
}

void StandardUserActionAssembly::setInitialTime(size_t event_index, double v)
{
  event_store_->setInitialTime(event_index, v);
}

double StandardUserActionAssembly::getInitialTime() const
{
  return event_store_->InitialTime();
}

} /* namespace anlgeant4 */
