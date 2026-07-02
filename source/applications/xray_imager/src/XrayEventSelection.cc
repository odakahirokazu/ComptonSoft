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

#include "XrayEventSelection.hh"
#include "XrayEvent.hh"

using namespace anlnext;

namespace comptonsoft
{

XrayEventSelection::XrayEventSelection()
{
}

XrayEventSelection::~XrayEventSelection() = default;

ANLStatus XrayEventSelection::mod_define()
{
  return AS_OK;
}

ANLStatus XrayEventSelection::mod_pre_initialize()
{
  if (status_!=AS_OK) {
    std::cerr << "Error in setting selection condition," << std::endl;
    return status_;
  }
  
  return AS_OK;
}

ANLStatus XrayEventSelection::mod_initialize()
{
  get_module("XrayEventCollection", &collection_);

  return AS_OK;
}

ANLStatus XrayEventSelection::mod_analyze()
{
  initializeEvent();

  const XrayEventContainer& events = collection_->getEvents();
  for (const XrayEvent_sptr& e: events) {
    if (select(e)) {
      insertEvent(e);
    }
  }

  return AS_OK;
}

bool XrayEventSelection::select(const XrayEvent_sptr& e) const
{
  bool selected = true;
  for (auto conditions: conditionsVector_) {
    bool ok = false;
    if (conditions.Conditions().size()==0) {
      ok = true;
    }
    for (auto f: conditions.Conditions()) {
      if (f(*e)) {
        ok = true;
        break;
      }
    }
    if (!ok) {
      selected = false;
      break;
    }
  }
  return selected;
}

void XrayEventSelection::add_condition(const std::string& key, const std::vector<std::pair<int, int>>& ranges)
{
  if (key=="frameID" || key=="ix" || key=="iy" || key=="weight" || key=="rank" || key=="grade") {
    ;
  }
  else {
    std::cerr << "Invalid key for int type: " << key << std::endl;
    status_ = AS_QUIT_ERROR;
    return;
  }
  add_condition<int>(key, ranges);
}

void XrayEventSelection::add_condition(const std::string& key, const std::vector<std::pair<double, double>>& ranges)
{
  if (key=="sumPH" || key=="centerPH" || key=="angle") {
    ;
  }
  else {
    std::cerr << "Invalid key for double type: " << key << std::endl;
    status_ = AS_QUIT_ERROR;
    return;
  }
  add_condition<double>(key, ranges);
}

template <typename T>
void XrayEventSelection::add_condition(const std::string& key, const std::vector<std::pair<T, T>>& ranges)
{
  namespace arg = std::placeholders;

  for (auto r: ranges) {
    std::cout << "FILTER  key: " << key << ", min: " << r.first << ", max: " << r.second << std::endl;
  }

  conditionsVector_.resize(conditionsVector_.size()+1);
  conditionsVector_.back().add_key(key);
  for (auto r: ranges) {
    std::function<bool (const XrayEvent&)> condition;
    const T min_value = r.first;
    const T max_value = r.second;
    if (key=="frameID") {
      condition = std::bind(filterXrayEvent<int>, &XrayEvent::FrameID, arg::_1, min_value, max_value);
    }
    if (key=="ix") {
      condition = std::bind(filterXrayEvent<int>, &XrayEvent::PixelX, arg::_1, min_value, max_value);
    }
    else if (key=="iy") {
      condition = std::bind(filterXrayEvent<int>, &XrayEvent::PixelY, arg::_1, min_value, max_value);
    }
    else if (key=="sumPH") {
      condition = std::bind(filterXrayEvent<double>, &XrayEvent::SumPH, arg::_1, min_value, max_value);
    }
    else if (key=="centerPH") {
      condition = std::bind(filterXrayEvent<double>, &XrayEvent::CenterPH, arg::_1, min_value, max_value);
    }
    else if (key=="weight") {
      condition = std::bind(filterXrayEvent<int>, &XrayEvent::Weight, arg::_1, min_value, max_value);
    }
    else if (key=="rank") {
      condition = std::bind(filterXrayEvent<int>, &XrayEvent::Rank, arg::_1, min_value, max_value);
    }
    else if (key=="angle") {
      condition = std::bind(filterXrayEvent<double>, &XrayEvent::Angle, arg::_1, min_value, max_value);
    }
    else if (key=="grade") {
      condition = std::bind(filterXrayEvent<int>, &XrayEvent::Grade, arg::_1, min_value, max_value);
    }
    conditionsVector_.back().add_condition(condition);
  }
}

} /* namespace comptonsoft */
