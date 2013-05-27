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

// ----- PickUpDataEventAction -----
// by S. Watanabe
// 2003/1/10 Shin Watanabe for Geant4ANL
// **************************************************************************
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "PickUpDataEventAction.hh"
#include "globals.hh"
#include "VPickUpData.hh"

using namespace anlgeant4;

PickUpDataEventAction::PickUpDataEventAction(VPickUpData* pud)
{
  pickup_data = pud;
  G4cout << "Constructing EventAction." << G4endl;
}
 
PickUpDataEventAction::~PickUpDataEventAction()
{
  G4cout << "Destructing EventAction." << G4endl;
}


void PickUpDataEventAction::BeginOfEventAction(const G4Event* anEvent)
{
  pickup_data->EventAct_begin(anEvent);
}


void PickUpDataEventAction::EndOfEventAction(const G4Event* anEvent)
{
  pickup_data->EventAct_end(anEvent); 
}
