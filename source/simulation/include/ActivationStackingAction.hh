/*************************************************************************
 *                                                                       *
 * Copyright (c) 2012 Hirokazu Odaka, Makoto Asai                        *
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

#ifndef COMPTONSOFT_ActivationStackingAction_H
#define COMPTONSOFT_ActivationStackingAction_H 1

#include "G4UserStackingAction.hh"

class G4Track;

namespace comptonsoft {


/**
 * Geant4 user statiking action for radioactivation simulations.
 *
 * @author Hirokazu Odaka, Makoto Asai
 * @date 2012-03-07
 */
class ActivationStackingAction : public G4UserStackingAction
{
public:
  ActivationStackingAction();
  virtual ~ActivationStackingAction();
  
public:
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
  virtual void NewStage() {}
  virtual void PrepareNewEvent() {}
};

}

#endif /* COMPTONSOFT_ActivationStackingAction_H */
