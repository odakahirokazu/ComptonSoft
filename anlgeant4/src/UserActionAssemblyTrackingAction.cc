/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Masanobu Ozaki, Shin Watanabe, Hirokazu Odaka      *
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

#include "UserActionAssemblyTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Trajectory.hh"
#include "VUserActionAssembly.hh"

namespace anlgeant4
{

UserActionAssemblyTrackingAction::UserActionAssemblyTrackingAction(const std::list<VUserActionAssembly*>& userActions)
  : userActions_(userActions)
{
}

UserActionAssemblyTrackingAction::~UserActionAssemblyTrackingAction() = default;

void UserActionAssemblyTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(new G4Trajectory(aTrack));
  
  for (VUserActionAssembly* pud: userActions_) {
    pud->TrackActionAtBeginning(aTrack);
  }
}

void UserActionAssemblyTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  for (VUserActionAssembly* pud: userActions_) {
    pud->TrackActionAtEnd(aTrack);
  }
}

} /* namespace anlgeant4 */
