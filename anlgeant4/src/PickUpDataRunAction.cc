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

// ----- MyRunAction -----
// by M.Kouda
//
// 2002, 11, 7, modefied S. Watanabe 
// ----- stdRunAction -----
// 2003/01/10 Shin Watanabe  Geant4ANL
// 2012/05/30 Hirokazu Odaka
// **************************************************************************
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "PickUpDataRunAction.hh"
#include "globals.hh"
#include "VPickUpData.hh"

using namespace anlgeant4;


PickUpDataRunAction::PickUpDataRunAction(VPickUpData* pud)
{
  G4cout << "Constructing RunAction." << G4endl;
  pickup_data = pud;
}


PickUpDataRunAction::~PickUpDataRunAction()
{
  G4cout <<"Destructing RunAction."<< G4endl;
}


void PickUpDataRunAction::BeginOfRunAction(const G4Run* aRun)
{
  pickup_data->RunAct_begin(aRun);
}


void PickUpDataRunAction::EndOfRunAction(const G4Run* aRun)
{
  pickup_data->RunAct_end(aRun);
}
