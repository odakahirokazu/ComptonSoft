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

#include "CalculatePedestalLevels.hh"
#include <boost/format.hpp>
#include "MultiChannelData.hh"
#include "ReadoutModule.hh"

using namespace anl;

namespace comptonsoft
{

CalculatePedestalLevels::CalculatePedestalLevels()
  : m_NBins(1024), m_ADCMin(-0.5), m_ADCMax(1023.5), m_NegativeADC(false),
    m_ADCZeroLevelBin(250), m_PeakSearchHalfWidth(245),
    m_AverageRangeHalfWidth(8)
{
}

ANLStatus CalculatePedestalLevels::mod_startup()
{
  register_parameter(&m_NBins, "num_bins");
  register_parameter(&m_NegativeADC, "adc_nagative");
  register_parameter(&m_ADCZeroLevelBin, "adc_zero_level_bin");
  register_parameter(&m_PeakSearchHalfWidth, "peak_search_half_width");
  register_parameter(&m_AverageRangeHalfWidth, "average_range_half_width");
  return AS_OK;
}

ANLStatus CalculatePedestalLevels::mod_his()
{
  VCSModule::mod_his();
  mkdir("pedestal");

  if (m_NegativeADC) {
    m_ADCMin = -0.5*m_NBins - 0.5;
    m_ADCMax = +0.5*m_NBins - 0.5;
  }
  else {
    m_ADCMin = -0.5;
    m_ADCMax = m_NBins - 0.5;
  }
  
  DetectorSystem* detectorManager = getDetectorManager();
  const int NumROM = detectorManager->NumberOfReadoutModules();
  for (int i=0; i<NumROM; i++) {
    ReadoutModule* ROM = detectorManager->getReadoutModuleByIndex(i);
    const int ROMID = ROM->ID();
    const int NumSections = ROM->NumberOfSections();
    for (int j=0; j<NumSections; j++) {
      const DetectorBasedChannelID section = ROM->getSection(j);
      MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      const int NumChannels = mcd->NumberOfChannels();
      std::string name;
      name = (boost::format("pedestal_r%04d_%04d") % ROMID % j).str();
      TH1* hisPed = new TH1D(name.c_str(), name.c_str(),
                             NumChannels, -0.5, NumChannels-0.5);
      m_hisPed.push_back(hisPed);

      name = (boost::format("noiselevel_r%04d_%04d") % ROMID % j).str();
      TH1* hisNoise = new TH1D(name.c_str(), name.c_str(),
                               NumChannels, -0.5, NumChannels-0.5);
      m_hisNoise.push_back(hisNoise);

      for (int k=0; k<NumChannels; k++) {
        name = (boost::format("spectrum_r%04d_%04d_%04d") % ROMID % j % k).str();
        TH1* hisSpec = new TH1I(name.c_str(), name.c_str(),
                                m_NBins, m_ADCMin, m_ADCMax);
        m_hisSpec.push_back(hisSpec);
      }
    }
  }

  return AS_OK;
}

ANLStatus CalculatePedestalLevels::mod_ana()
{
  std::vector<TH1*>::iterator itHist = m_hisSpec.begin();

  DetectorSystem* detectorManager = getDetectorManager();
  const int NumROM = detectorManager->NumberOfReadoutModules();
  for (int i=0; i<NumROM; i++) {
    const ReadoutModule* ROM
      = detectorManager->getReadoutModuleByIndex(i);
    const int NumSections = ROM->NumberOfSections();
    for (int j=0; j<NumSections; j++) {
      const DetectorBasedChannelID section = ROM->getSection(j);
      const MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      const int NumChannels = mcd->NumberOfChannels();
      for (int k=0; k<NumChannels; k++) {
        const int rawADC = mcd->getRawADC(k);
        (*itHist)->Fill(rawADC);
        ++itHist;
      }
    }
  }

  return AS_OK;
}

ANLStatus CalculatePedestalLevels::mod_endrun()
{
  const int ADCZeroLevelBin = m_ADCZeroLevelBin;
  const int SearchHalfWidth = m_PeakSearchHalfWidth;
  const int SearchMin = ADCZeroLevelBin - SearchHalfWidth;
  const int SearchMax = ADCZeroLevelBin + SearchHalfWidth;
  const int RangeHalfWidth = m_AverageRangeHalfWidth;

  std::vector<TH1*>::iterator itHisSpec = m_hisSpec.begin();
  std::vector<TH1*>::iterator itHisPed = m_hisPed.begin();
  std::vector<TH1*>::iterator itHisNoise = m_hisNoise.begin();

  DetectorSystem* detectorManager = getDetectorManager();
  const int NumROM = detectorManager->NumberOfReadoutModules();
  for (int i=0; i<NumROM; i++) {
    const ReadoutModule* ROM
      = detectorManager->getReadoutModuleByIndex(i);
    const int NumSections = ROM->NumberOfSections();
    for (int j=0; j<NumSections; j++) {
      const DetectorBasedChannelID section = ROM->getSection(j);
      const MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      const int NumChannels = mcd->NumberOfChannels();
      for (int k=0; k<NumChannels; k++) {
        (*itHisSpec)->GetXaxis()->SetRange(SearchMin, SearchMax);
        int centerbin = (*itHisSpec)->GetMaximumBin();
        int minbin = centerbin - RangeHalfWidth;
        int maxbin = centerbin + RangeHalfWidth;
        (*itHisSpec)->GetXaxis()->SetRange(minbin, maxbin);
        double pedestalMean = (*itHisSpec)->GetMean();
        (*itHisPed)->SetBinContent(k+1, pedestalMean);
        (*itHisSpec)->GetXaxis()->SetRange();
        double pedestalRMS = (*itHisSpec)->GetRMS();
        (*itHisNoise)->SetBinContent(k+1, pedestalRMS);
        *itHisSpec++;
      }
      itHisPed++;
      itHisNoise++;
    }
  }
 
  return AS_OK;
}

} /* namespace comptonsoft */
