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

#ifndef COMPTONSOFT_ObservationTreeIO_H
#define COMPTONSOFT_ObservationTreeIO_H 1

#include <cstdint>
#include <utility>
#include "ObservedParticle.hh"

class TTree;

namespace comptonsoft {

/**
 * 
 * @author Hirokazu Odaka
 * @date 2017-06-20
 */
class ObservationTreeIO
{
public:
  ObservationTreeIO();
  virtual ~ObservationTreeIO();

  virtual void setTree(TTree* tree)
  { tree_ = tree; }

  virtual void defineBranches();
  virtual void setBranchAddresses();

  void fillParticles(int64_t eventID, const std::vector<ObservedParticle_sptr>& particles);

  int64_t getEventID() const { return eventid_; }
  int32_t getNumberOfParticles() const { return num_; }

  ObservedParticle_sptr retrieveParticle() const;

  std::pair<int64_t, std::vector<ObservedParticle_sptr>>
  retrieveParticles(int64_t& entry,
                    bool get_first_entry=true);
  
private:
  TTree* tree_;

  /*
   * tree contents
   */
  int64_t eventid_ = 0;
  int32_t num_ = 0;
  int32_t trackid_ = 0;
  int32_t particle_ = 0;
  double time_ = 0.0;
  float posx_ = 0.0;
  float posy_ = 0.0;
  float posz_ = 0.0;
  float energy_ = 0.0;
  float dirx_ = 0.0;
  float diry_ = 0.0;
  float dirz_ = 0.0;
  float polarx_ = 0.0;
  float polary_ = 0.0;
  float polarz_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ObservationTreeIO_H */
