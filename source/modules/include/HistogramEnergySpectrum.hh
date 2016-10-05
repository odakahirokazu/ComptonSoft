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

#ifndef COMPTONSOFT_HistogramEnergySpectrum_H
#define COMPTONSOFT_HistogramEnergySpectrum_H 1

#include "VCSModule.hh"

class TH1;
namespace anlgeant4 { class InitialInformation; }

namespace comptonsoft {

class CSHitCollection;

class HistogramEnergySpectrum : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramEnergySpectrum, 1.1);
public:
  HistogramEnergySpectrum();  
  ~HistogramEnergySpectrum() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
private:
  CSHitCollection* m_HitCollection;
  const anlgeant4::InitialInformation* m_InitialInfo;
  std::string m_EnergyBinType;
  int m_NumBinEnergy;
  double m_RangeEnergy1;
  double m_RangeEnergy2;
  std::map<std::string, TH1*> m_Histograms;
  std::vector<std::string> m_Selections;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramEnergySpectrum_H */
