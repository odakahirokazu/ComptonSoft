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

using namespace anlnext;

namespace comptonsoft
{

HistogramPHA::HistogramPHA()
  : readout_order_(true), grouping_in_section_(true),
    histogram_type_("PHA"),
    num_bins_(1280), range_min_(-256.5), range_max_(1023.5)
{
}

ANLStatus HistogramPHA::mod_define()
{
  define_parameter("readout_order", &mod_class::readout_order_);
  define_parameter("group", &mod_class::grouping_in_section_);
  define_parameter("category", &mod_class::histogram_type_);
  define_parameter("num_bins", &mod_class::num_bins_);
  define_parameter("range_min", &mod_class::range_min_);
  define_parameter("range_max", &mod_class::range_max_);
  return AS_OK;
}

ANLStatus HistogramPHA::mod_initialize()
{
  VCSModule::mod_initialize();
  mkdir();

  if (readout_order_) {
    DetectorSystem* detectorManager = getDetectorManager();
    detectorManager->doForEachMultiChannelDataInReadoutOrder(
      [&](MultiChannelData* mcd, const ReadoutBasedChannelID& channel) {
        if (grouping_in_section_) {
          std::string name = "spectrum_"+channel.toString();
          TH1* h = new TH1I(name.c_str(), name.c_str(),
                            num_bins_, range_min_, range_max_);
          histograms_.push_back(h);
        }
        else {
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            ReadoutBasedChannelID channel2(channel.ReadoutModule(),
                                      channel.Section(),
                                      k);
            std::string name = "spectrum_"+channel2.toString();
            TH1* h = new TH1I(name.c_str(), name.c_str(),
                              num_bins_, range_min_, range_max_);
            histograms_.push_back(h);
          }
        }
      });
  }
  else {
    DetectorSystem* detectorManager = getDetectorManager();
    detectorManager->doForEachMultiChannelDataInDetectorOrder(
      [&](MultiChannelData* mcd, const DetectorBasedChannelID& channel) {
        if (grouping_in_section_) {
          std::string name = "spectrum_"+channel.toString();
          TH1* h = new TH1I(name.c_str(), name.c_str(),
                            num_bins_, range_min_, range_max_);
          histograms_.push_back(h);
        }
        else {
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            ReadoutBasedChannelID channel2(channel.Detector(),
                                           channel.Section(),
                                           k);
            std::string name = "spectrum_"+channel2.toString();
            TH1* h = new TH1I(name.c_str(), name.c_str(),
                              num_bins_, range_min_, range_max_);
            histograms_.push_back(h);
          }
        }
      });
  }

  if (histogram_type_=="RawADC") {
    getter_func_ = [](MultiChannelData* mcd, int k) -> double {
      return mcd->getRawADC(k);
    };
  }
  else if (histogram_type_=="PHA") {
    getter_func_ = std::mem_fn(&MultiChannelData::getPHA);
  }
  else if (histogram_type_=="EPI") {
    getter_func_ = std::mem_fn(&MultiChannelData::getEPI);
  }
  else {
    std::cout << "Histogram type \"" << histogram_type_ << "\" is invalid." << std::endl;
    return AS_QUIT_ERROR;
  }

  return AS_OK;
}

ANLStatus HistogramPHA::mod_analyze()
{
  std::vector<TH1*>::iterator itHist = std::begin(histograms_);

  DetectorSystem* detectorManager = getDetectorManager();
  if (readout_order_) {
    detectorManager->doForEachMultiChannelDataInReadoutOrder(
      [&](MultiChannelData* mcd, const ReadoutBasedChannelID&) {
        if (grouping_in_section_) {
          TH1* h = *itHist;
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            double v = getter_func_(mcd, k);
            h->Fill(v);
          }
          ++itHist;
        }
        else {
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            TH1* h = *itHist;
            double v = getter_func_(mcd, k);
            h->Fill(v);
            ++itHist;
          }
        }
      });
  }
  else {
    detectorManager->doForEachMultiChannelDataInDetectorOrder(
      [&](MultiChannelData* mcd, const DetectorBasedChannelID&) {
        if (grouping_in_section_) {
          TH1* h = *itHist;
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            double v = getter_func_(mcd, k);
            h->Fill(v);
          }
          ++itHist;
        }
        else {
          const int NumChannels = mcd->NumberOfChannels();
          for (int k=0; k<NumChannels; k++) {
            TH1* h = *itHist;
            double v = getter_func_(mcd, k);
            h->Fill(v);
            ++itHist;
          }
        }
      });
  }

  return AS_OK;
}

} /* namespace comptonsoft */
