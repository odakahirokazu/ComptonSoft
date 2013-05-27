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

#ifndef ANLGEANT4_PickUpDataSteppingAction_H
#define ANLGEANT4_PickUpDataSteppingAction_H 1

#include "G4UserSteppingAction.hh"

namespace anlgeant4
{

class VPickUpData;

/**
 * User SteppingAction class for PickUpData
 * @author M. Kouda, S. Watanabe, H. Odaka
 * @date 2003-01-10 (modified: S. Watanabe)
 * @date 2012-05-30 (modified: H. Odaka)
 */
class PickUpDataSteppingAction : public G4UserSteppingAction
{
public:
  explicit PickUpDataSteppingAction(VPickUpData* pud);
  ~PickUpDataSteppingAction();

  void UserSteppingAction(const G4Step*);

private:
  VPickUpData* pickup_data;
};

}

#endif /* ANLGEANT4_PickUpDataSteppingAction_H */
