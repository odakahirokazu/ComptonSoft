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

#ifndef COMPTONSOFT_CalculatePedestalLevels_H
#define COMPTONSOFT_CalculatePedestalLevels_H 1

#include "VCSModule.hh"
#include "TH1.h"

namespace comptonsoft {

class CalculatePedestalLevels : public VCSModule
{
  DEFINE_ANL_MODULE(CalculatePedestalLevels, 2.2);
public:
  CalculatePedestalLevels();
  ~CalculatePedestalLevels() = default;
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  std::vector<TH1*> m_hisSpec;
  std::vector<TH1*> m_hisPed;
  std::vector<TH1*> m_hisNoise;

  int m_NBins = 1024;
  double m_ADCMin;
  double m_ADCMax;
  bool m_NegativeADC = false;

  int m_ADCZeroLevelBin = 250;
  int m_PeakSearchHalfWidth = 245;
  int m_AverageRangeHalfWidth = 8;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CalculatePedestalLevels_H */
