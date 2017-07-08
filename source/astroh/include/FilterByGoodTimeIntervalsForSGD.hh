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

#ifndef COMPTONSOFT_FilterByGoodTimeIntervalsForSGD_H
#define COMPTONSOFT_FilterByGoodTimeIntervalsForSGD_H 1

#include "FilterByGoodTimeIntervals.hh"

namespace comptonsoft {

class ReadSGDEventFITS;


/**
 * @author Hirokazu Odaka
 * @date 2017-02-06
 */
class FilterByGoodTimeIntervalsForSGD : public FilterByGoodTimeIntervals
{
  DEFINE_ANL_MODULE(FilterByGoodTimeIntervalsForSGD, 1.0);
public:
  FilterByGoodTimeIntervalsForSGD();
  ~FilterByGoodTimeIntervalsForSGD();

  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;

private:
  const ReadSGDEventFITS* m_EventReader = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_FilterByGoodTimeIntervalsForSGD_H */
