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

#ifndef COMPTONSOFT_HistogramPHA_H
#define COMPTONSOFT_HistogramPHA_H 1

#include "VCSModule.hh"
#include <string>
#include <vector>
#include <functional>

class TH1;

namespace comptonsoft {

/**
 * Making histograms.
 * @author Hirokazu Odaka
 * @date 2014-11-30
 */
class HistogramPHA : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramPHA, 3.0);
public:
  HistogramPHA();
  ~HistogramPHA() = default;
 
  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_his() override;
  anl::ANLStatus mod_ana() override;

private:
  bool m_ReadoutOrder;
  bool m_GroupingInSection;
  std::string m_HistogramType;
  std::vector<TH1*> m_Histograms;
  int m_NumBins;
  double m_RangeMin;
  double m_RangeMax;
  std::function<double (MultiChannelData*, int)> m_GetterFunc;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramPHA_H */
