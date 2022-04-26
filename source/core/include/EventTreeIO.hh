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

#ifndef COMPTONSOFT_EventTreeIO_H
#define COMPTONSOFT_EventTreeIO_H 1

#include <cstdint>
#include <cstddef>
#include <array>
#include "DetectorHit_sptr.hh"

class TTree;

namespace comptonsoft {

/**
 * 
 * @author Hirokazu Odaka
 * @date 2015-10-11
 * @date 2016-09-07
 * @date 2020-11-24 | add the particle branch
 * @date 2020-12-25 | add the track ID branch
 */
class EventTreeIO
{
public:
  static const std::size_t MaxHits = 65536;
  
public:
  EventTreeIO();
  virtual ~EventTreeIO();

  virtual void setTree(TTree* tree)
  { tree_ = tree; }

  virtual void defineBranches();
  virtual void setBranchAddresses();

  void fillHits(int64_t eventID, const std::vector<DetectorHit_sptr>& hits);
  void fillHits(const std::vector<DetectorHit_sptr>& hits)
  { fillHits(-1, hits); }
  void fillUndetectedEvent(int64_t eventID);

  int64_t getEventID() const { return eventid_; }
  int64_t getNumberOfHits() const { return num_hits_; }
  DetectorHit_sptr retrieveHit(std::size_t i) const;
  std::vector<DetectorHit_sptr> retrieveHits(int64_t& entry, bool get_entry=true);
  
private:
  TTree* tree_;

  /*
   * tree contents
   */
  int64_t eventid_ = 0;
  int32_t num_hits_ = 0;
  // measured data
  int64_t ti_ = 0;
  int16_t instrument_ = 0;
  std::array<int16_t, MaxHits> detector_;
  std::array<int16_t, MaxHits> det_section_;
  std::array<int16_t, MaxHits> readout_module_;
  std::array<int16_t, MaxHits> section_;
  std::array<int16_t, MaxHits> channel_;
  std::array<int16_t, MaxHits> pixelx_;
  std::array<int16_t, MaxHits> pixely_;
  std::array<int16_t, MaxHits> pixelz_;
  std::array<int32_t, MaxHits> rawpha_;
  std::array<float, MaxHits> pha_;
  std::array<float, MaxHits> epi_;
  uint64_t flag_data_ = 0ul;
  uint64_t flags_ = 0ul;
  // simulation
  std::array<int32_t, MaxHits> trackid_;
  std::array<int32_t, MaxHits> particle_;
  std::array<double, MaxHits> real_time_;
  std::array<double, MaxHits> time_trig_;
  std::array<int16_t, MaxHits> time_group_;
  std::array<float, MaxHits> real_posx_;
  std::array<float, MaxHits> real_posy_;
  std::array<float, MaxHits> real_posz_;
  std::array<float, MaxHits> edep_;
  std::array<float, MaxHits> echarge_;
  std::array<uint32_t, MaxHits> process_;
  // reconstructed
  std::array<float, MaxHits> energy_;
  std::array<float, MaxHits> posx_;
  std::array<float, MaxHits> posy_;
  std::array<float, MaxHits> posz_;
  std::array<float, MaxHits> local_posx_;
  std::array<float, MaxHits> local_posy_;
  std::array<float, MaxHits> local_posz_;
  std::array<double, MaxHits> time_;
  int32_t grade_ = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_EventTreeIO_H */
