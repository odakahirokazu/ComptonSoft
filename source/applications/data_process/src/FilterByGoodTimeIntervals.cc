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

#include "FilterByGoodTimeIntervals.hh"
#include "AstroUnits.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"

using namespace anlnext;

namespace comptonsoft
{

FilterByGoodTimeIntervals::FilterByGoodTimeIntervals()
{
}

FilterByGoodTimeIntervals::~FilterByGoodTimeIntervals() = default;

ANLStatus FilterByGoodTimeIntervals::mod_define()
{
  define_parameter("time_intervals", &mod_class::m_GTIs);
  add_value_element("start", 0.0, CLHEP::second, "s");
  add_value_element("end", 0.0, CLHEP::second, "s");

  return AS_OK;
}

ANLStatus FilterByGoodTimeIntervals::mod_initialize()
{
  get_module("CSHitCollection", &hit_collection_);
  define_evs("FilterByGoodTimeIntervals:OK");

  return AS_OK;
}

ANLStatus FilterByGoodTimeIntervals::mod_analyze()
{
  bool passed = false;
  const int NumTimeGroups = hit_collection_->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    const std::vector<DetectorHit_sptr>& hits
      = hit_collection_->getHits(timeGroup);
    for (const DetectorHit_sptr& hit: hits) {
      const double t = hit->Time();
      for (const std::tuple<double, double>& interval: m_GTIs) {
        const double time0 = std::get<0>(interval);
        const double time1 = std::get<1>(interval);
        if (time0<=t && t<time1) {
          passed = true;
          goto out_of_loop;
        }
      }
    }
  }

  out_of_loop:
  if (passed) {
    set_evs("FilterByGoodTimeIntervals:OK");
  }
  else {
    return AS_SKIP;
  }

  return AS_OK;
}

} /* namespace comptonsoft */
