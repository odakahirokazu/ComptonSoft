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

#include "FilterByGoodTimeIntervalsForSGD.hh"
#include "AstroUnits.hh"
#include "ReadSGDEventFITS.hh"


using namespace anlnext;

namespace comptonsoft
{

FilterByGoodTimeIntervalsForSGD::FilterByGoodTimeIntervalsForSGD() = default;

FilterByGoodTimeIntervalsForSGD::~FilterByGoodTimeIntervalsForSGD() = default;

ANLStatus FilterByGoodTimeIntervalsForSGD::mod_initialize()
{
  get_module("ReadSGDEventFITS", &m_EventReader);
  define_evs("FilterByGoodTimeIntervals:OK");

  return AS_OK;
}

ANLStatus FilterByGoodTimeIntervalsForSGD::mod_analyze()
{
  bool passed = false;

  const double t = m_EventReader->EventTime();
  for (const std::tuple<double, double>& interval: m_GTIs) {
    const double time0 = std::get<0>(interval);
    const double time1 = std::get<1>(interval);
    if (time0<=t && t<time1) {
      passed = true;
      break;
    }
  }

  if (passed) {
    set_evs("FilterByGoodTimeIntervals:OK");
  }
  else {
    return AS_SKIP;
  }

  return AS_OK;
}

} /* namespace comptonsoft */
