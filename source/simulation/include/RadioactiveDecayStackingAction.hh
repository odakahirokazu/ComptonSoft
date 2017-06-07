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

#ifndef COMPTONSOFT_RadioactiveDecayStackingAction_H
#define COMPTONSOFT_RadioactiveDecayStackingAction_H 1

#include "G4UserStackingAction.hh"

class G4Track;

namespace comptonsoft {


/**
 * Geant4 user statiking action for radioactive decay simulations.
 *
 * @author Hirokazu Odaka
 * @date 2017-06-07
 */
class RadioactiveDecayStackingAction : public G4UserStackingAction
{
public:
  RadioactiveDecayStackingAction();
  virtual ~RadioactiveDecayStackingAction();
  
public:
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
  virtual void NewStage() {}
  virtual void PrepareNewEvent() {}

  void setFirstDecayTime(double v) { firstDecayTime_ = v; }
  void setTerminationTime(double v) { terminationTime_ = v; }

private:
  double firstDecayTime_ = 0.0;
  double terminationTime_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RadioactiveDecayStackingAction_H */
