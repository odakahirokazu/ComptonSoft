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

using namespace anl;

namespace comptonsoft
{

FilterByGoodTimeIntervals::FilterByGoodTimeIntervals()
{
}

FilterByGoodTimeIntervals::~FilterByGoodTimeIntervals() = default;

ANLStatus FilterByGoodTimeIntervals::mod_startup()
{
  register_parameter(&m_GTIs, "time_intervals");
  add_value_element(&m_TimeStart, "start", CLHEP::second, "s");
  add_value_element(&m_TimeEnd, "end", CLHEP::second, "s");

  return AS_OK;
}

ANLStatus FilterByGoodTimeIntervals::mod_init()
{
  GetModule("CSHitCollection", &m_HitCollection);
  EvsDef("FilterByGoodTimeIntervals:OK");

  return AS_OK;
}

ANLStatus FilterByGoodTimeIntervals::mod_ana()
{
  bool passed = false;
  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    const std::vector<DetectorHit_sptr>& hits
      = m_HitCollection->getHits(timeGroup);
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
    EvsSet("FilterByGoodTimeIntervals:OK");
  }
  else {
    return AS_SKIP;
  }

  return AS_OK;
}

} /* namespace comptonsoft */
