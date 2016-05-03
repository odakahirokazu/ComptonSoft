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

#include "HistogramPHA.hh"
#include <iterator>
#include "TH1.h"
#include "MultiChannelData.hh"

using namespace anl;

namespace comptonsoft
{

HistogramPHA::HistogramPHA()
  : m_ReadoutOrder(true), m_GroupingInSection(true),
    m_HistogramType("PHA"),
    m_NumBins(1280), m_RangeMin(-256.5), m_RangeMax(1023.5)
{
}

ANLStatus HistogramPHA::mod_startup()
{
  register_parameter(&m_ReadoutOrder, "readout_order");
  register_parameter(&m_GroupingInSection, "group");
  register_parameter(&m_HistogramType, "category");
  register_parameter(&m_NumBins, "num_bins");
  register_parameter(&m_RangeMin, "range_min");
  register_parameter(&m_RangeMax, "range_max");
  return AS_OK;
}

ANLStatus HistogramPHA::mod_his()
{
  VCSModule::mod_his();
  mkdir();

  if (m_ReadoutOrder) {
    DetectorSystem* detectorManager = getDetectorManager();
    detectorManager->doForEachMultiChannelDataInReadoutOrder(
      [&](MultiChannelData* mcd, const ReadoutChannelID& channel) {
        if (m_GroupingInSection) {
          std::string name = "spectrum_"+channel.toString();
          TH1* h = new TH1I(name.c_str(), name.c_str(),
                            m_NumBins, m_RangeMin, m_RangeMax);
          m_Histograms.push_back(h);
        }
        else {
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            ReadoutChannelID channel2(channel.ReadoutModule(),
                                      channel.Section(),
                                      k);
            std::string name = "spectrum_"+channel2.toString();
            TH1* h = new TH1I(name.c_str(), name.c_str(),
                              m_NumBins, m_RangeMin, m_RangeMax);
            m_Histograms.push_back(h);
          }
        }
      });
  }
  else {
    DetectorSystem* detectorManager = getDetectorManager();
    detectorManager->doForEachMultiChannelDataInDetectorOrder(
      [&](MultiChannelData* mcd, const DetectorChannelID& channel) {
        if (m_GroupingInSection) {
          std::string name = "spectrum_"+channel.toString();
          TH1* h = new TH1I(name.c_str(), name.c_str(),
                            m_NumBins, m_RangeMin, m_RangeMax);
          m_Histograms.push_back(h);
        }
        else {
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            ReadoutChannelID channel2(channel.Detector(),
                                      channel.Section(),
                                      k);
            std::string name = "spectrum_"+channel2.toString();
            TH1* h = new TH1I(name.c_str(), name.c_str(),
                              m_NumBins, m_RangeMin, m_RangeMax);
            m_Histograms.push_back(h);
          }
        }
      });
  }

  if (m_HistogramType=="RawADC") {
    m_GetterFunc = [](MultiChannelData* mcd, int k) -> double {
      return mcd->getRawADC(k);
    };
  }
  else if (m_HistogramType=="PHA") {
    m_GetterFunc = std::mem_fn(&MultiChannelData::getPHA);
  }
  else if (m_HistogramType=="EPI") {
    m_GetterFunc = std::mem_fn(&MultiChannelData::getEPI);
  }
  else {
    std::cout << "Histogram type \"" << m_HistogramType << "\" is invalid." << std::endl;
    return AS_QUIT_ERR;
  }
  
  return AS_OK;
}

ANLStatus HistogramPHA::mod_ana()
{
  std::vector<TH1*>::iterator itHist = std::begin(m_Histograms);

  DetectorSystem* detectorManager = getDetectorManager();
  if (m_ReadoutOrder) {
    detectorManager->doForEachMultiChannelDataInReadoutOrder(
      [&](MultiChannelData* mcd, const ReadoutChannelID&) {
        if (m_GroupingInSection) {
          TH1* h = *itHist;
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            double v = m_GetterFunc(mcd, k);
            h->Fill(v);
          }
          ++itHist;
        }
        else {
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            TH1* h = *itHist;
            double v = m_GetterFunc(mcd, k);
            h->Fill(v);
            ++itHist;
          }
        }
      });
  }
  else {
    detectorManager->doForEachMultiChannelDataInDetectorOrder(
      [&](MultiChannelData* mcd, const DetectorChannelID&) {
        if (m_GroupingInSection) {
          TH1* h = *itHist;
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            double v = m_GetterFunc(mcd, k);
            h->Fill(v);
          }
          ++itHist;
        }
        else {
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            TH1* h = *itHist;
            double v = m_GetterFunc(mcd, k);
            h->Fill(v);
            ++itHist;
          }
        }
      });
  }

  return AS_OK;
}

} /* namespace comptonsoft */
