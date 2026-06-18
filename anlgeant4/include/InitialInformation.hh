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
  struct PrimaryData
  {
    int32_t event_id_ = -1;
    double weight_ = 1.0;
    double energy_ = 0.0;
    G4ThreeVector direction_ = {0.0, 0.0, 0.0};
    double time_ = 0.0;
    G4ThreeVector position_ = {0.0, 0.0, 0.0};
    G4ThreeVector polarization_ = {0.0, 0.0, 0.0};
  };

public:
  explicit InitialInformation(bool stored, anlnext::BasicModule* mod=nullptr);

  bool InitialInformationStored() const { return stored_; }
  void setInitialInformationStored(bool v=true) { stored_ = v; }

  int32_t RunID() const { return run_id_; }

  int32_t EventID() const { return primary_vector_[index_].event_id_; }
  double Weight() const { return primary_vector_[index_].weight_; }
  double InitialEnergy() const { return primary_vector_[index_].energy_; }
  G4ThreeVector InitialDirection() const { return primary_vector_[index_].direction_; }
  double InitialTime() const { return primary_vector_[index_].time_; }
  G4ThreeVector InitialPosition() const { return primary_vector_[index_].position_; }
  G4ThreeVector InitialPolarization() const { return primary_vector_[index_].polarization_; }

  void setRunID(int32_t i) { run_id_ = i; }

  void setEventID(size_t i, int32_t v) { primary_vector_[i].event_id_ = v; }
  void setWeight(size_t i, double v) { primary_vector_[i].weight_ = v; }
  void setInitialEnergy(size_t i, double v) { primary_vector_[i].energy_ = v; }
  void setInitialDirection(size_t i, G4ThreeVector v){ primary_vector_[i].direction_ = v;    }
  void setInitialDirection(size_t i, double x, double y, double z) { primary_vector_[i].direction_.set(x, y, z); }
  void setInitialTime(size_t i, double v) { primary_vector_[i].time_ = v; }
  void setInitialPosition(size_t i, G4ThreeVector v) { primary_vector_[i].position_ = v; }
  void setInitialPosition(size_t i, double x, double y, double z) { primary_vector_[i].position_.set(x, y, z); }
  void setInitialPolarization(size_t i, G4ThreeVector v) { primary_vector_[i].polarization_ = v; }
  void setInitialPolarization(size_t i, double x, double y, double z) { primary_vector_[i].polarization_.set(x, y, z); }

  // setters for a single event mode
  void setEventID(int32_t v) { primary_vector_[0].event_id_ = v; }
  void setWeight(double v) { primary_vector_[0].weight_ = v; }
  void setInitialEnergy(double v) { primary_vector_[0].energy_ = v; }
  void setInitialDirection(G4ThreeVector v){ primary_vector_[0].direction_ = v;    }
  void setInitialDirection(double x, double y, double z) { primary_vector_[0].direction_.set(x, y, z); }
  void setInitialTime(double v) { primary_vector_[0].time_ = v; }
  void setInitialPosition(G4ThreeVector v) { primary_vector_[0].position_ = v; }
  void setInitialPosition(double x, double y, double z) { primary_vector_[0].position_.set(x, y, z); }
  void setInitialPolarization(G4ThreeVector v) { primary_vector_[0].polarization_ = v; }
  void setInitialPolarization(double x, double y, double z) { primary_vector_[0].polarization_.set(x, y, z); }

protected:
  void initializeRun(int runID, int num_events);
  void initializeEvent(int eventID);
  void set_read_index(size_t i) { index_ = i; }

private:
  bool stored_;
  int32_t run_id_;
  std::vector<PrimaryData> primary_vector_;
  size_t index_ = 0;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_InitialInformation_H */
