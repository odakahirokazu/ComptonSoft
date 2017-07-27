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

#include <cstdint>
#include "G4ThreeVector.hh"

namespace anlnext {
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
  explicit InitialInformation(bool stored, anlnext::BasicModule* mod=nullptr);

  bool InitialInformationStored() const { return stored_; }
  void setInitialInformationStored(bool v=true) { stored_ = v; }
  bool WeightStored() const { return weight_stored_; }
  void setWeightStored(bool v=true) { weight_stored_ = v; }

  double InitialEnergy() const              { return energy_; }
  G4ThreeVector InitialDirection() const    { return direction_; }
  double InitialTime() const                { return time_; }
  G4ThreeVector InitialPosition() const     { return position_; }
  G4ThreeVector InitialPolarization() const { return polarization_; }
  
  int64_t EventID() const { return event_id_; }
  double Weight() const { return weight_; }

  void setEventID(int64_t i) { event_id_ = i; }

  void setInitialEnergy(double v)
  { energy_ = v; }
  void setInitialDirection(G4ThreeVector v)
  { direction_ = v;    }
  void setInitialDirection(double x, double y, double z)
  { direction_.set(x, y, z); }
  void setInitialTime(double v)
  { time_ = v; }
  void setInitialPosition(G4ThreeVector v)
  { position_ = v; }
  void setInitialPosition(double x, double y, double z)
  { position_.set(x, y, z); }
  void setInitialPolarization(G4ThreeVector v)
  { polarization_ = v; }
  void setInitialPolarization(double x, double y, double z)
  { polarization_.set(x, y, z); }
  
  void setWeight(double v) { weight_ = v; }

private:
  bool stored_;
  bool weight_stored_;

  double energy_;
  G4ThreeVector direction_;
  double time_;
  G4ThreeVector position_;
  G4ThreeVector polarization_;
  int64_t event_id_;
  double weight_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_InitialInformation_H */
