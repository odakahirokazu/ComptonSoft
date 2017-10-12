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

#ifndef COMPTONSOFT_FilterByGoodTimeIntervalsForHXI_H
#define COMPTONSOFT_FilterByGoodTimeIntervalsForHXI_H 1

#include "FilterByGoodTimeIntervals.hh"

namespace comptonsoft {

class ReadHXIEventFITS;


/**
 * @author Hirokazu Odaka
 * @date 2017-02-06
 */
class FilterByGoodTimeIntervalsForHXI : public FilterByGoodTimeIntervals
{
  DEFINE_ANL_MODULE(FilterByGoodTimeIntervalsForHXI, 1.0);
public:
  FilterByGoodTimeIntervalsForHXI();
  ~FilterByGoodTimeIntervalsForHXI();

  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  const ReadHXIEventFITS* m_EventReader = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_FilterByGoodTimeIntervalsForHXI_H */
