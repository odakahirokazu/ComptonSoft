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
    int32_t event_id = -1;
    double weight = 1.0;
    double time = 0.0;
    G4ThreeVector position = {0.0, 0.0, 0.0};
    double energy = 0.0;
    G4ThreeVector direction = {0.0, 0.0, 0.0};
    G4ThreeVector polarization = {0.0, 0.0, 0.0};
  };

public:
  explicit InitialInformation(bool stored, anlnext::BasicModule* mod=nullptr);

  bool initial_information_stored() const { return stored_; }
  void set_initial_information_stored(bool v=true) { stored_ = v; }

  int32_t run_id() const { return run_id_; }

  int32_t event_id() const { return primary_vector_[index_].event_id; }
  double weight() const { return primary_vector_[index_].weight; }
  double initial_time() const { return primary_vector_[index_].time; }
  G4ThreeVector initial_position() const { return primary_vector_[index_].position; }
  double initial_energy() const { return primary_vector_[index_].energy; }
  G4ThreeVector initial_direction() const { return primary_vector_[index_].direction; }
  G4ThreeVector initial_polarization() const { return primary_vector_[index_].polarization; }

  void set_run_id(int32_t i) { run_id_ = i; }

  void set_event_id(size_t i, int32_t v) { primary_vector_[i].event_id = v; }
  void set_weight(size_t i, double v) { primary_vector_[i].weight = v; }
  void set_initial_time(size_t i, double v) { primary_vector_[i].time = v; }
  void set_initial_position(size_t i, G4ThreeVector v) { primary_vector_[i].position = v; }
  void set_initial_energy(size_t i, double v) { primary_vector_[i].energy = v; }
  void set_initial_direction(size_t i, G4ThreeVector v){ primary_vector_[i].direction = v; }
  void set_initial_polarization(size_t i, G4ThreeVector v) { primary_vector_[i].polarization = v; }

  // setters for a single event mode
  void set_event_id(int32_t v) { primary_vector_[0].event_id = v; }
  void set_weight(double v) { primary_vector_[0].weight = v; }
  void set_initial_time(double v) { primary_vector_[0].time = v; }
  void set_initial_position(G4ThreeVector v) { primary_vector_[0].position = v; }
  void set_initial_energy(double v) { primary_vector_[0].energy = v; }
  void set_initial_direction(G4ThreeVector v){ primary_vector_[0].direction = v;    }
  void set_initial_polarization(G4ThreeVector v) { primary_vector_[0].polarization = v; }

protected:
  void initialize_run(int runID, int num_events);
  void initialize_event(int eventID);
  void set_read_index(size_t i) { index_ = i; }

private:
  bool stored_;
  int32_t run_id_;
  std::vector<PrimaryData> primary_vector_;
  size_t index_ = 0;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_InitialInformation_H */
