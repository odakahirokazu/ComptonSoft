/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
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

#ifndef ANLGEANT4_InitialInformation_H
#define ANLGEANT4_InitialInformation_H 1

#include "G4ThreeVector.hh"

namespace anl {
class BasicModule;
}

namespace anlgeant4 {

/**
 * store initial particle information
 * @author Hirokazu Odaka
 */
class InitialInformation
{
public:
  explicit InitialInformation(bool s, anl::BasicModule* amod=0);

  bool InitialInformationStored() const { return stored; }
  void setInitialInformationStored(bool v=true) { stored = v; }
  bool WeightStored() const { return weight_stored; }
  void setWeightStored(bool v=true) { weight_stored = v; }

  double InitialEnergy() const              { return energy; }
  G4ThreeVector InitialDirection() const    { return direction; }
  double InitialTime() const                { return time0; }
  G4ThreeVector InitialPosition() const     { return position; }
  G4ThreeVector InitialPolarization() const { return polarization; }
  
  int EventID() const { return event_id; }
  double Weight() const { return weight; }

  void setEventID(int i) { event_id = i; }

protected:
  void setInitialEnergy(double v)
  { energy = v; }
  void setInitialDirection(G4ThreeVector v)
  { direction = v;    }
  void setInitialDirection(double x, double y, double z)
  { direction.set(x, y, z); }
  void setInitialTime(double v)
  { time0 = v; }
  void setInitialPosition(G4ThreeVector v)
  { position = v; }
  void setInitialPosition(double x, double y, double z)
  { position.set(x, y, z); }
  void setInitialPolarization(G4ThreeVector v)
  { polarization = v; }
  void setInitialPolarization(double x, double y, double z)
  { polarization.set(x, y, z); }
  
  void setWeight(double v) { weight = v; }

private:
  bool stored;
  bool weight_stored;

  double energy;
  G4ThreeVector direction;
  double time0;
  G4ThreeVector position;
  G4ThreeVector polarization;
  int event_id;
  double weight;
};

}

#endif /* ANLGEANT4_InitialInformation_H */
