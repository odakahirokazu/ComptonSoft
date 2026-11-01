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

#include "NanoGRAMSAnalyzeLight.hh"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "LightData.hh"
#include "NanoGRAMSConfig.hh"
#include "NanoGRAMSLoadConfig.hh"
#include "RealDetectorUnitNanoGRAMS.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

namespace {

constexpr double DigitizerInputImpedance = 50.0 * unit::ohm;

} // namespace

NanoGRAMSAnalyzeLight::NanoGRAMSAnalyzeLight() = default;

NanoGRAMSAnalyzeLight::~NanoGRAMSAnalyzeLight() = default;

ANLStatus NanoGRAMSAnalyzeLight::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  get_module("NanoGRAMSLoadConfig", &configLoader_);
  config_ = &configLoader_->config();
  return AS_OK;
}

ANLStatus NanoGRAMSAnalyzeLight::mod_analyze()
{
  const grams::Config& cfg = *config_;
  for (auto& detector : getDetectorManager()->getDetectors()) {
    if (!detector->checkType(DetectorType::NanoGRAMS)) {
      continue;
    }
    // always RealDetectorUnitNanoGRAMS
    // Waveform (voltage) is given by the reader and may be corrected by NanoGRAMSCorrectLightWaveform
    auto* nanograms = static_cast<RealDetectorUnitNanoGRAMS*>(detector.get());

    const std::vector<int> generalChannels = collectValidChannels(*nanograms, cfg.general_analysis_channels);
    if (!generalChannels.empty()) {
      const LightPeaks peaks = analyzeChannelGroup(*nanograms, generalChannels);
      if (peaks.ROI > cfg.light_gamma_thr) {
        nanograms->addEventFlags(nanograms_event_flag::LightGamma);
      }
      if (peaks.ROI > cfg.light_cosmic_thr) {
        nanograms->addEventFlags(nanograms_event_flag::LightCosmic);
      }
    }
    // integrated charges&peak search; their sum over the general analysis channels is the light PHA of the event
    double lightPHA = 0.0;
    for (const int dppChannel : generalChannels) {
      const double charge =
          integratedROICharge(*nanograms->getLightData(dppChannel)) * cfg.light_gain_correction[dppChannel];
      nanograms->setLightIntegratedCharge(dppChannel, charge);
      const LightData::range_t roi_range = {-cfg.pre_roi_window, cfg.post_roi_window};
      const double peak =
          nanograms->getLightData(dppChannel)->peak(roi_range); // This is deplicated the process above. But I stay it.
      nanograms->setLightPeakValue(dppChannel, peak);
      if (std::isfinite(charge)) {
        lightPHA += charge;
      }
    }
    nanograms->setLightPHA(lightPHA);
    const double gain = cfg.light_gain_mode == grams::LightGainMode::Direct ? 1.0 : 1.0 / cfg.sipm_gain;
    nanograms->setPhotonCount(lightPHA * gain); // calibrate into the actual photon count with the SiPM gain

    const std::vector<int> pileupChannels = collectValidChannels(*nanograms, cfg.pileup_analysis_channels);
    if (!pileupChannels.empty()) {
      const LightPeaks peaks = analyzeChannelGroup(*nanograms, pileupChannels);
      if (peaks.preROI > cfg.out_roi_peak_thr || peaks.postROI > cfg.out_roi_peak_thr) {
        nanograms->addEventFlags(nanograms_event_flag::LightPileup);
      }
    }
    // integrated charges of the pileup analysis channels (not included in the photon count)
    for (const int dppChannel : pileupChannels) {
      if (std::find(generalChannels.begin(), generalChannels.end(), dppChannel) == generalChannels.end()) {
        nanograms->setLightIntegratedCharge(dppChannel, integratedROICharge(*nanograms->getLightData(dppChannel)));
      }
    }
  }

  return AS_OK;
}

double NanoGRAMSAnalyzeLight::integratedROICharge(const LightData& lightData) const
{
  const grams::Config& cfg = *config_;
  const LightData::range_t preROI(-cfg.drift_time_max, -cfg.pre_roi_window);
  const LightData::range_t ROI(-cfg.pre_roi_window, cfg.post_roi_window);

  const double baseline = lightData.mean(preROI);
  if (!std::isfinite(baseline)) {
    return std::numeric_limits<double>::quiet_NaN();
  }

  // integral of (voltage - baseline) over the ROI
  const int numPoints = lightData.NumberOfPoints();
  const int start = std::clamp(lightData.findTimeIndex(ROI.first), 0, numPoints);
  const int stop = std::clamp(lightData.findTimeIndex(ROI.second), 0, numPoints);
  const int numSamples = std::max(0, stop - start);
  const double digitizerIntegral = lightData.integral(ROI) - baseline * numSamples * lightData.TimeWidth();

  // digitizer voltage -> output voltage of the transimpedance amplifier -> input current
  const double outputVoltageScale =
      cfg.light_gain_mode == grams::LightGainMode::Direct
          ? 1.0 / cfg.light_direct_gain // If direct gain mode, it converts to light count directly
          : (cfg.light_output_impedance_ohm + DigitizerInputImpedance) / DigitizerInputImpedance /
                cfg.light_transimpedance_feedback_resistance_ohm; // otherwise, it outputs the input current of TIA.
  return digitizerIntegral * outputVoltageScale;
}

std::vector<int> NanoGRAMSAnalyzeLight::collectValidChannels(const RealDetectorUnitNanoGRAMS& detector,
                                                             const std::vector<int>& channels) const
{
  std::vector<int> validChannels;
  for (const int dppChannel : channels) {
    if (dppChannel < detector.NumberOfLightData() && detector.getLightData(dppChannel)->isValid()) {
      validChannels.push_back(dppChannel);
    }
  }
  return validChannels;
}

NanoGRAMSAnalyzeLight::LightPeaks NanoGRAMSAnalyzeLight::analyzeChannelGroup(const RealDetectorUnitNanoGRAMS& detector,
                                                                             const std::vector<int>& channels) const
{
  const grams::Config& cfg = *config_;
  const LightData::range_t preROI(-cfg.drift_time_max, -cfg.pre_roi_window);
  const LightData::range_t ROI(-cfg.pre_roi_window, cfg.post_roi_window);
  const LightData::range_t postROI(cfg.post_roi_window, cfg.drift_time_max);

  // a missing peak (empty window) is treated as -infinity, i.e. never above the thresholds
  auto peakIn = [](const LightData& lightData, const LightData::range_t& range) {
    const double peak = lightData.peak(range);
    return std::isnan(peak) ? -std::numeric_limits<double>::infinity() : peak;
  };

  if (cfg.light_analysis_method == grams::LightAnalysisMethod::Average) {
    // the waveforms are averaged sample by sample; the time axis of the first channel is used
    const LightData& reference = *detector.getLightData(channels.front());
    for (const int dppChannel : channels) {
      if (detector.getLightData(dppChannel)->TimeWidth() != reference.TimeWidth()) {
        throw std::runtime_error(
            "light.waveform_analysis=average requires the same time width (wave_compress) in an analysis group.");
      }
    }
    LightData average(reference.NumberOfPoints(), reference.TimeWidth(), reference.TimeStart());
    std::vector<double>& averageWaveform = average.Waveform();
    for (const int dppChannel : channels) {
      const std::vector<double>& waveform = detector.getLightData(dppChannel)->Waveform();
      const std::size_t n = std::min(averageWaveform.size(), waveform.size());
      for (std::size_t i = 0; i < n; ++i) {
        averageWaveform[i] += waveform[i] / static_cast<double>(channels.size());
      }
    }
    return LightPeaks{peakIn(average, preROI), peakIn(average, ROI), peakIn(average, postROI)};
  }

  // each channel: the maximum over the channels
  LightPeaks peaks{-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(),
                   -std::numeric_limits<double>::infinity()};
  for (const int dppChannel : channels) {
    const LightData& lightData = *detector.getLightData(dppChannel);
    peaks.preROI = std::max(peaks.preROI, peakIn(lightData, preROI));
    peaks.ROI = std::max(peaks.ROI, peakIn(lightData, ROI));
    peaks.postROI = std::max(peaks.postROI, peakIn(lightData, postROI));
  }
  return peaks;
}

} /* namespace comptonsoft */
