/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Shin Watanabe, Hirokazu Odaka                      *
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

// ----- MySteppingAction -----
// by M.Kouda
//
// ----- PickUpDataSteppingAction -----
// 2003/1/10 Shin Watanabe Geant4ANL
// **************************************************************************
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "PickUpDataSteppingAction.hh"
#include "globals.hh"
#include "G4Step.hh"
#include "VPickUpData.hh"

using namespace anlgeant4;

PickUpDataSteppingAction::PickUpDataSteppingAction(VPickUpData* pud)
{
  pickup_data = pud;
  G4cout << "Constructing SteppingAction." << G4endl;
}


PickUpDataSteppingAction::~PickUpDataSteppingAction()
{
  G4cout << "Destructing SteppingAction." << G4endl;
}


void PickUpDataSteppingAction::UserSteppingAction(const G4Step* aStep)
{
  G4Track* aTrack = aStep->GetTrack();
  
#if 0
  // debug
  G4cout << "PreStep " << aStep->GetPreStepPoint()->GetPosition()
	 << " -- PostStep " << aStep->GetPostStepPoint()->GetPosition()
	 << " >>> Material " << aTrack->GetMaterial()->GetName() << G4endl;
#endif
  
  pickup_data->StepAct(aStep, aTrack);
}
