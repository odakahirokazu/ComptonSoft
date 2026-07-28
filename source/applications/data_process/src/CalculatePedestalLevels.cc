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

using namespace anlnext;

namespace comptonsoft
{

CalculatePedestalLevels::CalculatePedestalLevels()
  : num_bins_(1024), ADC_min_(-0.5), ADC_max_(1023.5), negative_ADC_(false),
    ADC_zero_level_bin_(250), peak_search_half_width_(245),
    average_range_half_width_(8)
{
}

ANLStatus CalculatePedestalLevels::mod_define()
{
  define_parameter("num_bins", &mod_class::num_bins_);
  define_parameter("adc_nagative", &mod_class::negative_ADC_);
  define_parameter("adc_zero_level_bin", &mod_class::ADC_zero_level_bin_);
  define_parameter("peak_search_half_width", &mod_class::peak_search_half_width_);
  define_parameter("average_range_half_width", &mod_class::average_range_half_width_);
  return AS_OK;
}

ANLStatus CalculatePedestalLevels::mod_initialize()
{
  VCSModule::mod_initialize();
  mkdir("pedestal");

  if (negative_ADC_) {
    ADC_min_ = -0.5*num_bins_ - 0.5;
    ADC_max_ = +0.5*num_bins_ - 0.5;
  }
  else {
    ADC_min_ = -0.5;
    ADC_max_ = num_bins_ - 0.5;
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
      pedestral_histograms_.push_back(hisPed);

      name = (boost::format("noiselevel_r%04d_%04d") % ROMID % j).str();
      TH1* hisNoise = new TH1D(name.c_str(), name.c_str(),
                               NumChannels, -0.5, NumChannels-0.5);
      noise_histograms_.push_back(hisNoise);

      for (int k=0; k<NumChannels; k++) {
        name = (boost::format("spectrum_r%04d_%04d_%04d") % ROMID % j % k).str();
        TH1* hisSpec = new TH1I(name.c_str(), name.c_str(),
                                num_bins_, ADC_min_, ADC_max_);
        spectrum_histograms_.push_back(hisSpec);
      }
    }
  }

  return AS_OK;
}

ANLStatus CalculatePedestalLevels::mod_analyze()
{
  std::vector<TH1*>::iterator itHist = spectrum_histograms_.begin();

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

ANLStatus CalculatePedestalLevels::mod_end_run()
{
  const int ADCZeroLevelBin = ADC_zero_level_bin_;
  const int SearchHalfWidth = peak_search_half_width_;
  const int SearchMin = ADCZeroLevelBin - SearchHalfWidth;
  const int SearchMax = ADCZeroLevelBin + SearchHalfWidth;
  const int RangeHalfWidth = average_range_half_width_;

  std::vector<TH1*>::iterator itHisSpec = spectrum_histograms_.begin();
  std::vector<TH1*>::iterator itHisPed = pedestral_histograms_.begin();
  std::vector<TH1*>::iterator itHisNoise = noise_histograms_.begin();

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
