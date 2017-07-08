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

#ifndef COMPTONSOFT_HistogramARMByPositionMeasurement_H
#define COMPTONSOFT_HistogramARMByPositionMeasurement_H 1

#include "HistogramARM.hh"
#include <memory>

class TRandom;

namespace comptonsoft {

class HistogramARMByPositionMeasurement : public HistogramARM
{
  DEFINE_ANL_MODULE(HistogramARMByPositionMeasurement, 3.1);
public:
  HistogramARMByPositionMeasurement();
  ~HistogramARMByPositionMeasurement();

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_analyze() override;

private:
  int m_NumSamples;
  std::unique_ptr<TRandom> m_RandomGenerator;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramARMByPositionMeasurement_H */
