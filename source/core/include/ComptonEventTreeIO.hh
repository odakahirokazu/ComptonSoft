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

#ifndef COMPTONSOFT_ComptonEventTreeIO_H
#define COMPTONSOFT_ComptonEventTreeIO_H 1

#include <cstdint>
#include "DetectorHit_sptr.hh"

class TTree;

namespace comptonsoft {

class BasicComptonEvent;

/**
 * 
 * @author Hirokazu Odaka
 * @date 2014-12-02
 */
class ComptonEventTreeIO
{
public:
  ComptonEventTreeIO();
  virtual ~ComptonEventTreeIO();

  virtual void setTree(TTree* tree)
  { cetree_ = tree; }

  virtual void defineBranches();
  virtual void setBranchAddresses();

  void fillEvent(int64_t eventID,
                 const BasicComptonEvent& event);
  void fillEvent(const BasicComptonEvent& event)
  { fillEvent(-1, event); }
  
  BasicComptonEvent retrieveEvent() const;
  void retrieveEvent(BasicComptonEvent& event) const;

  int64_t getEventID() const { return eventid_; }
  
private:
  TTree* cetree_;

  /*
   * tree contents
   */
  uint64_t eventid_ = 0u;

  int16_t hit1_id_ = 0;
  uint32_t hit1_process_ = 0;
  int16_t hit1_detector_ = 0;
  int16_t hit1_readout_module_ = 0;
  int16_t hit1_section_ = 0;
  int16_t hit1_channel_ = 0;
  int16_t hit1_pixelx_ = 0;
  int16_t hit1_pixely_ = 0;
  float hit1_time_ = 0.0;
  float hit1_posx_ = 0.0;
  float hit1_posy_ = 0.0;
  float hit1_posz_ = 0.0;
  float hit1_energy_ = 0.0;

  int16_t hit2_id_ = 0;
  uint32_t hit2_process_ = 0;
  int16_t hit2_detector_ = 0;
  int16_t hit2_readout_module_ = 0;
  int16_t hit2_section_ = 0;
  int16_t hit2_channel_ = 0;
  int16_t hit2_pixelx_ = 0;
  int16_t hit2_pixely_ = 0;
  float hit2_time_ = 0.0;
  float hit2_posx_ = 0.0;
  float hit2_posy_ = 0.0;
  float hit2_posz_ = 0.0;
  float hit2_energy_ = 0.0;

  uint64_t flags_ = 0u;

  float costheta_ = 0.0;
  float dtheta_ = 0.0;
  float phi_ = 0.0;

  uint64_t hitpattern_ = 0u;
  int32_t grade_ = 0;
  float likelihood_ = 1.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ComptonEventTreeIO_H */
