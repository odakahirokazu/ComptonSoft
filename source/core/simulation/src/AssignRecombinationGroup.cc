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

#include "AssignRecombinationGroup.hh"

#include "G4Event.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "RecombinationGroupTable.hh"

using namespace anlnext;

namespace comptonsoft
{

namespace {

bool isCreatedByGammaConversion(const G4Track* track)
{
  const G4VProcess* process = track->GetCreatorProcess();
  return process != nullptr && process->GetProcessName().find("conv") != std::string::npos;
}

} /* anonymous namespace */

AssignRecombinationGroup::AssignRecombinationGroup()
{
}

void AssignRecombinationGroup::event_action_at_beginning(const G4Event*)
{
  RecombinationGroupTable::threadLocalInstance().clear();
}

void AssignRecombinationGroup::track_action_at_beginning(const G4Track* track)
{
  RecombinationGroupTable& table = RecombinationGroupTable::threadLocalInstance();
  const int32_t trackID = track->GetTrackID();
  const int32_t parentID = track->GetParentID();

  RecombinationGroupTable::Entry entry;
  entry.originTrackID = -trackID;
  entry.originPosition = track->GetVertexPosition();
  entry.charged = (track->GetDefinition()->GetPDGCharge() != 0.0);
  if (parentID != 0) {
    // a parent is always tracked before its secondaries
    const RecombinationGroupTable::Entry* parent = table.find(parentID);
    if (parent != nullptr && parent->charged) {
      entry.originTrackID = parent->originTrackID;
      entry.originPosition = parent->originPosition;
    }
    else if (isCreatedByGammaConversion(track)) {
      // the electron and the positron of pair production make separate tracks, so each starts a new group
      entry.originTrackID = -trackID;
    }
    else {
      // secondaries created at the same interaction of a neutral particle share the group
      entry.originTrackID = parentID;
    }
  }
  table.insert(trackID, entry);
}

} /* namespace comptonsoft */
