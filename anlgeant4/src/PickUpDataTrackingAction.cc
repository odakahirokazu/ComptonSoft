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

//
// v1.0 2.?.2001 M.Ozaki
// v1.1 2.? 2001 T.Takahashi
//
// v2.0 4.29. 2002 S. Watanabe 
//      with class MyDaq for output interaction processes
//      
// ----- MyTrackingAction -----
// by M.Ozaki & T.Takahashi
//
// 2002, 11, 11 Shin Watanabe 
//
// ----- stdTrackingAction -----
// 2003/1/10 Shin Watanabe Geant4ANL
//
// **************************************************************************
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...

#include "PickUpDataTrackingAction.hh"

#include "globals.hh"
#include "G4TrackingManager.hh"
#include "G4Trajectory.hh"
#include "VPickUpData.hh"


using namespace anlgeant4;

PickUpDataTrackingAction::PickUpDataTrackingAction(VPickUpData* pud)
{
  pickup_data = pud;
  G4cout << "Constructing TrackingAction " << G4endl;
}


PickUpDataTrackingAction::~PickUpDataTrackingAction()
{
  G4cout << "Destructing TrackingAction " << G4endl;
}


void PickUpDataTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(new G4Trajectory(aTrack));
  
  pickup_data->TrackAct_begin(aTrack);
}


void PickUpDataTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  pickup_data->TrackAct_end(aTrack);
}
