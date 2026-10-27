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

#ifndef COMPTONSOFT_RecombinationGroupTable_H
#define COMPTONSOFT_RecombinationGroupTable_H 1

#include <cstdint>
#include <vector>
#include "G4ThreeVector.hh"

namespace comptonsoft {

/**
 * Per-thread table from a track ID to its recombination group in the current event.
 * It is filled by AssignRecombinationGroup at the beginning of each track and read by the sensitive detector.
 * A group is identified by the track ID and the position of the interaction that created it:
 *   - primary particle: (-trackID, vertex)
 *   - electron or positron of pair production: (-trackID, vertex)
 *   - other secondary of a neutral particle: (parent trackID, vertex)
 *   - secondary of a charged particle: same as the parent
 * Track IDs of Geant4 are numbered from 1 in each event, so they are used as indices of a vector.
 *
 * @author Shota Arai
 * @date 2026-10-27
 */
class RecombinationGroupTable
{
public:
  struct Entry
  {
    int32_t originTrackID = 0; // 0 means not assigned
    G4ThreeVector originPosition;
    bool charged = false;
  };

  static RecombinationGroupTable& threadLocalInstance();

  void clear() { entries_.clear(); }

  void insert(int32_t trackID, const Entry& entry)
  {
    const std::size_t index = static_cast<std::size_t>(trackID);
    if (index >= entries_.size()) {
      entries_.resize(index + 1);
    }
    entries_[index] = entry;
  }

  const Entry* find(int32_t trackID) const
  {
    const std::size_t index = static_cast<std::size_t>(trackID);
    if (trackID <= 0 || index >= entries_.size() || entries_[index].originTrackID == 0) {
      return nullptr;
    }
    return &entries_[index];
  }

private:
  RecombinationGroupTable() = default;

  // the capacity is kept over events, so no allocation occurs once it is large enough
  std::vector<Entry> entries_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RecombinationGroupTable_H */
