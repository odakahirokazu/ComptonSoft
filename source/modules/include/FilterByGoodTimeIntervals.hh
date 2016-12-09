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

#ifndef COMPTONSOFT_FilterByGoodTimeIntervals_H
#define COMPTONSOFT_FilterByGoodTimeIntervals_H 1

#include "BasicModule.hh"
#include <tuple>

namespace comptonsoft {

class CSHitCollection;

/**
 * @author Hirokazu Odaka
 * @date 2016-12-08
 */
class FilterByGoodTimeIntervals : public anl::BasicModule
{
  DEFINE_ANL_MODULE(FilterByGoodTimeIntervals, 1.0);
public:
  FilterByGoodTimeIntervals();
  ~FilterByGoodTimeIntervals();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();

private:
  const CSHitCollection* m_HitCollection = nullptr;
  std::vector<std::tuple<double, double>> m_GTIs;
  double m_TimeStart = 0.0;
  double m_TimeEnd = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_FilterByGoodTimeIntervals_H */
