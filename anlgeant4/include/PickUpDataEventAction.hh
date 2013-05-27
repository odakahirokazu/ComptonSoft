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

#ifndef ANLGEANT4_PickUpdataEventAction_H
#define ANLGEANT4_PickUpDataEventAction_H 1

#include "G4UserEventAction.hh"

namespace anlgeant4
{

class VPickUpData;

/**
 * User EventAction class for PickUpData.
 * @author S. Watanabe, H. Odaka
 * @date 2003-01-10 (modified: S. Watanabe)
 * @date 2012-05-30 (modified: H. Odaka)
 */
class PickUpDataEventAction : public G4UserEventAction
{
public:
  explicit PickUpDataEventAction(VPickUpData* pud);
  ~PickUpDataEventAction();

public:
  void BeginOfEventAction(const G4Event* anEvent);
  void EndOfEventAction(const G4Event* anEvent);

private:
  VPickUpData* pickup_data;
};

}

#endif /* ANLGEANT4_PickUpDataEventAction_H */
