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

#ifndef ANLGEANT4_PickUpDataRunAction_H
#define ANLGEANT4_PickUpDataRunAction_H 1

#include "G4UserRunAction.hh"

namespace anlgeant4
{

class VPickUpData;

/**
 * User RunAction class for PickUpData.
 * @author M. Kouda, S. Watanabe, H. Odaka
 * @date 2002-12-07 (modified: S. Watanabe)
 * @date 2012-05-30 (modified: H. Odaka)
 */
class PickUpDataRunAction : public G4UserRunAction
{
public:
  explicit PickUpDataRunAction(VPickUpData* pud);
  ~PickUpDataRunAction();

public:
  virtual void BeginOfRunAction(const G4Run* aRun);
  virtual void EndOfRunAction(const G4Run* aRun);

private:
  VPickUpData* pickup_data;
};

}

#endif /* ANLGEANT4_PickUpDataRunAction_H */
