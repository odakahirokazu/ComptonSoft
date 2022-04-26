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

#ifndef COMPTONSOFT_HitTreeIO_H
#define COMPTONSOFT_HitTreeIO_H 1

#include <cstdint>
#include "DetectorHit_sptr.hh"

class TTree;

namespace comptonsoft {

/**
 * 
 * @author Hirokazu Odaka
 * @date 2014-12-02
 * @date 2016-09-07
 * @date 2020-09-02 | add error branches
 * @date 2020-11-24 | add the particle branch
 * @date 2020-12-25 | add the track ID branch
 * @date 2022-04-27 | add the pixel-z
 */
class HitTreeIO
{
public:
  HitTreeIO();
  virtual ~HitTreeIO();

  virtual void setTree(TTree* tree)
  { hittree_ = tree; }

  virtual void defineBranches();
  virtual void setBranchAddresses();

  void fillHits(int64_t eventID, const std::vector<DetectorHit_sptr>& hits);
  void fillHits(const std::vector<DetectorHit_sptr>& hits)
  { fillHits(-1, hits); }

  int64_t getEventID() const { return eventid_; }
  int64_t getNumberOfHits() const { return num_hits_; }
  DetectorHit_sptr retrieveHit() const;
  std::vector<DetectorHit_sptr> retrieveHits(int64_t& entry,
                                             bool get_first_entry=true);
  
private:
  TTree* hittree_;

  /*
   * tree contents
   */
  int64_t eventid_ = 0;
  int16_t ihit_ = 0;
  int32_t num_hits_ = 0;
  // measured data
  int64_t ti_ = 0;
  int16_t instrument_ = 0;
  int16_t detector_ = -1;
  int16_t det_section_ = -1;
  int16_t readout_module_ = -1;
  int16_t section_ = -1;
  int16_t channel_ = -1;
  int16_t pixelx_ = -1;
  int16_t pixely_ = -1;
  int16_t pixelz_ = -1;
  int32_t rawpha_ = 0;
  float pha_ = 0.0;
  float epi_ = 0.0;
  float epi_error_ = 0.0;
  uint64_t flag_data_ = 0ul;
  uint64_t flags_ = 0ul;
  // simulation
  int32_t trackid_ = 0;
  int32_t particle_ = 0;
  double real_time_ = 0.0;
  double time_trig_ = 0.0;
  int16_t time_group_ = 0;
  float real_posx_ = 0.0;
  float real_posy_ = 0.0;
  float real_posz_ = 0.0;
  float edep_ = 0.0;
  float echarge_ = 0.0;
  uint32_t process_ = 0u;
  // reconstructed
  float energy_ = 0.0;
  float energy_error_ = 0.0;
  float posx_ = 0.0;
  float posy_ = 0.0;
  float posz_ = 0.0;
  float posx_error_ = 0.0;
  float posy_error_ = 0.0;
  float posz_error_ = 0.0;
  float local_posx_ = 0.0;
  float local_posy_ = 0.0;
  float local_posz_ = 0.0;
  float local_posx_error_ = 0.0;
  float local_posy_error_ = 0.0;
  float local_posz_error_ = 0.0;
  double time_ = 0.0;
  double time_error_ = 0.0;
  int32_t grade_ = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HitTreeIO_H */
