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

#ifndef COMPTONSOFT_XrayEventTreeIO_H
#define COMPTONSOFT_XrayEventTreeIO_H 1

#include <cstdint>
#include <list>
#include "XrayEvent.hh"

class TTree;

namespace comptonsoft {


/**
 * 
 * @author Hirokazu Odaka
 * @date 2019-06-05
 * @date 2020-04-02 | add detector_id
 */
class XrayEventTreeIO
{
public:
  XrayEventTreeIO();
  virtual ~XrayEventTreeIO();

  void setEventSize(int v);
  int EventSize() const { return eventSize_; }

  virtual void setTree(TTree* tree)
  { tree_ = tree; }

  virtual void defineBranches();
  virtual void setBranchAddresses();

  int fillEvents(const XrayEventContainer& events);
  int fillEvents(XrayEventCIter events_begin, XrayEventCIter events_end);
  XrayEvent_sptr retrieveEvent() const;
  std::list<XrayEvent_sptr> retrieveEvents(int64_t& entry);
  
private:
  static constexpr int MaxEventSize_ = 9;

  TTree* tree_ = nullptr;
  int eventSize_ = 1;

  /*
   * tree contents
   */
  int32_t frameID_ = -1;
  double time_ = 0.0;
  int32_t detectorID_ = 0;
  int32_t ix_ = 0;
  int32_t iy_ = 0;
  double data_[MaxEventSize_][MaxEventSize_];
  double sumPH_ = 0.0;
  double centerPH_ = 0.0;
  int32_t weight_ = 0;
  int32_t rank_ = 0;
  double angle_ = 0.0;
  int32_t grade_ = 0;
  double energy_ = 0.0;
  double posx_ = 0.0;
  double posy_ = 0.0;
  double posz_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_XrayEventTreeIO_H */
