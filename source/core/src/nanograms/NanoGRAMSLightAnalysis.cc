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

#include "NanoGRAMSLightAnalysis.hh"

#include "AstroUnits.hh"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

namespace comptonsoft
{
namespace unit = anlgeant4::unit;

namespace grams
{

namespace
{

constexpr double kDigitizerInputImpedanceOhm = 50.0 * unit::ohm;

struct LightPeaks
{
  double peak          = -std::numeric_limits<double>::infinity() * unit::volt;
  double pre_roi_peak  = -std::numeric_limits<double>::infinity() * unit::volt;
  double post_roi_peak = -std::numeric_limits<double>::infinity() * unit::volt;
};

std::vector<int> collectValidLightChannels(const std::vector<int>& requested_channels,
                                           const TPCTreeBuffer& tpc_tree_buffer,
                                           bool light_ok)
{
  std::vector<int> valid_channels;
  const TPCTreeLayout& tpc_tree_layout = tpc_tree_buffer.layout();

  if (!light_ok) {
    return valid_channels;
  }

  for (const int light_ch : requested_channels) {
    if (light_ch < 0 || light_ch >= tpc_tree_layout.num_dpp_registered_slots) {
      continue;
    }
    if (!tpc_tree_buffer.registered_channels[light_ch]) {
      continue;
    }
    valid_channels.push_back(light_ch);
  }
  return valid_channels;
}

bool containsChannel(const std::vector<int>& channels, int channel)
{
  return std::find(channels.begin(), channels.end(), channel) != channels.end();
}

bool hasSharedChannel(const std::vector<int>& lhs, const std::vector<int>& rhs)
{
  for (const int ch : lhs) {
    if (containsChannel(rhs, ch)) {
      return true;
    }
  }
  return false;
}

uint16_t requireSameWaveCompressForAverage(const TPCTreeBuffer& tpc_tree_buffer,
                                           const std::vector<int>& valid_channels,
                                           const std::string& group_name)
{
  if (valid_channels.empty()) {
    return 0;
  }

  const uint16_t reference_wave_compress =
      tpc_tree_buffer.wave_compress[valid_channels.front()];
  for (const int light_ch : valid_channels) {
    if (tpc_tree_buffer.wave_compress[light_ch] != reference_wave_compress) {
      throw std::runtime_error(
          "light.waveform_analysis=average requires the same wave_compress "
          "inside light." + group_name + ".");
    }
  }
  return reference_wave_compress;
}

void validateAverageAnalysisGroups(const TPCTreeBuffer& tpc_tree_buffer,
                                   const std::vector<int>& general_channels,
                                   const std::vector<int>& pileup_channels)
{
  const bool has_general_channels = !general_channels.empty();
  const bool has_pileup_channels = !pileup_channels.empty();

  uint16_t general_wave_compress = 0;
  uint16_t pileup_wave_compress = 0;
  if (has_general_channels) {
    general_wave_compress =
        requireSameWaveCompressForAverage(tpc_tree_buffer,
                                          general_channels,
                                          "general_analysis_channels");
  }
  if (has_pileup_channels) {
    pileup_wave_compress =
        requireSameWaveCompressForAverage(tpc_tree_buffer,
                                          pileup_channels,
                                          "pileup_analysis_channels");
  }

  if (has_general_channels && has_pileup_channels &&
      hasSharedChannel(general_channels, pileup_channels) &&
      general_wave_compress != pileup_wave_compress) {
    throw std::runtime_error(
        "light.general_analysis_channels and light.pileup_analysis_channels "
        "share at least one channel, so their common wave_compress values "
        "must match in average mode.");
  }
}

double lightVoltageAtSample(const Config& cfg,
                            const TPCTreeBuffer& tpc_tree_buffer,
                            int light_ch,
                            int raw_idx)
{
  const int waveform_len = tpc_tree_buffer.layout().waveform_len;
  const int waveform_slot = tpc_tree_buffer.waveformSlotForDPPChannel(light_ch);
  if (waveform_slot < 0) {
    return std::numeric_limits<double>::quiet_NaN() * unit::volt;
  }
  const int waveform_offset = waveform_slot * waveform_len;
  return static_cast<double>(tpc_tree_buffer.waveform[waveform_offset + raw_idx]) *
         cfg.adc2mv * (unit::volt / 1000.0);
}

void updateLightPeaksByIndex(LightPeaks& peaks,
                             int raw_idx,
                             int pre_pileup_start_index,
                             int pre_pileup_stop_index,
                             int post_pileup_start_index,
                             int post_pileup_stop_index,
                             double voltage)
{
  if (pre_pileup_start_index <= raw_idx &&
      raw_idx < pre_pileup_stop_index) {
    peaks.pre_roi_peak = std::max(peaks.pre_roi_peak, voltage);
  } else if (post_pileup_start_index <= raw_idx &&
             raw_idx < post_pileup_stop_index) {
    peaks.post_roi_peak = std::max(peaks.post_roi_peak, voltage);
  } else if (pre_pileup_stop_index <= raw_idx &&
             raw_idx < post_pileup_start_index) {
    peaks.peak = std::max(peaks.peak, voltage);
  }
}

void updateGeneralLightStatusFromPeaks(LightStatus& status,
                                       const Config& cfg,
                                       const LightPeaks& peaks)
{
  status.cosmic = status.cosmic || (peaks.peak > cfg.light_cosmic_thr);
  status.gamma = status.gamma || (peaks.peak > cfg.light_gamma_thr);
}

void updatePileupLightStatusFromPeaks(LightStatus& status,
                                      const Config& cfg,
                                      const LightPeaks& peaks)
{
  status.pileup_pre_roi =
      status.pileup_pre_roi || (peaks.pre_roi_peak > cfg.out_roi_peak_thr);
  status.pileup_post_roi =
      status.pileup_post_roi || (peaks.post_roi_peak > cfg.out_roi_peak_thr);
}

double baselineVoltage(const Config& cfg,
                       const TPCTreeBuffer& tpc_tree_buffer,
                       const LightTimingState& light_timing,
                       int light_ch)
{
  const int start_index = light_timing.pre_pileup_start_index[light_ch];
  const int stop_index  = light_timing.pre_pileup_stop_index[light_ch];

  double voltage_sum = 0.0 * unit::volt;
  int num_samples = 0;
  for (int raw_idx = start_index; raw_idx < stop_index; ++raw_idx) {
    const double voltage =
        lightVoltageAtSample(cfg, tpc_tree_buffer, light_ch, raw_idx);
    if (std::isfinite(voltage)) {
      voltage_sum += voltage;
      ++num_samples;
    }
  }

  if (num_samples == 0) {
    return std::numeric_limits<double>::quiet_NaN() * unit::volt;
  }

  return voltage_sum / static_cast<double>(num_samples);
}

double integratedDigitizerVoltSecond(const Config& cfg,
                                     const TPCTreeBuffer& tpc_tree_buffer,
                                     const LightTimingState& light_timing,
                                     int light_ch)
{
  const double baseline =
      baselineVoltage(cfg, tpc_tree_buffer, light_timing, light_ch);
  if (!std::isfinite(baseline)) {
    return std::numeric_limits<double>::quiet_NaN();
  }

  const double timebin  = light_timing.timebin[light_ch];
  const int start_index = light_timing.pre_pileup_stop_index[light_ch];
  const int stop_index  = light_timing.post_pileup_start_index[light_ch];

  double integral = 0.0;
  for (int raw_idx = start_index; raw_idx < stop_index; ++raw_idx) {
    const double voltage =
        lightVoltageAtSample(cfg, tpc_tree_buffer, light_ch, raw_idx);
    if (std::isfinite(voltage)) {
      integral += (voltage - baseline) / unit::volt *
                  timebin / unit::second;
    }
  }

  return integral;
}

double lightRoiIntegralChargeForChannels(const Config& cfg,
                                         const TPCTreeBuffer& tpc_tree_buffer,
                                         const LightTimingState& light_timing,
                                         const std::vector<int>& valid_channels)
{
  double digitizer_integral_volt_second = 0.0;
  for (const int light_ch : valid_channels) {
    const double channel_integral =
        integratedDigitizerVoltSecond(cfg,
                                      tpc_tree_buffer,
                                      light_timing,
                                      light_ch);
    if (std::isfinite(channel_integral)) {
      digitizer_integral_volt_second += channel_integral;
    }
  }

  const double output_voltage_scale =
      (cfg.light_output_impedance_ohm + kDigitizerInputImpedanceOhm) /
      kDigitizerInputImpedanceOhm;
  const double tia_output_integral_volt_second =
      digitizer_integral_volt_second * output_voltage_scale;
  const double charge_coulomb =
      tia_output_integral_volt_second /
      (cfg.light_transimpedance_feedback_resistance_ohm / unit::ohm);

  return charge_coulomb * unit::coulomb;
}

LightPeaks analyzeSingleLightChannel(const Config& cfg,
                                     const TPCTreeBuffer& tpc_tree_buffer,
                                     const LightTimingState& light_timing,
                                     int light_ch)
{
  LightPeaks peaks;
  const int waveform_len = tpc_tree_buffer.layout().waveform_len;
  const int pre_pileup_start_index =
      light_timing.pre_pileup_start_index[light_ch];
  const int pre_pileup_stop_index =
      light_timing.pre_pileup_stop_index[light_ch];
  const int post_pileup_start_index =
      light_timing.post_pileup_start_index[light_ch];
  const int post_pileup_stop_index =
      light_timing.post_pileup_stop_index[light_ch];

  for (int raw_idx = 0; raw_idx < waveform_len; ++raw_idx) {
    updateLightPeaksByIndex(peaks,
                            raw_idx,
                            pre_pileup_start_index,
                            pre_pileup_stop_index,
                            post_pileup_start_index,
                            post_pileup_stop_index,
                            lightVoltageAtSample(cfg, tpc_tree_buffer, light_ch, raw_idx));
  }
  return peaks;
}

LightPeaks analyzeAverageLightWaveform(const Config& cfg,
                                       const TPCTreeBuffer& tpc_tree_buffer,
                                       const LightTimingState& light_timing,
                                       const std::vector<int>& valid_channels)
{
  LightPeaks peaks;
  if (valid_channels.empty()) {
    return peaks;
  }

  const int waveform_len = tpc_tree_buffer.layout().waveform_len;
  const int reference_ch = valid_channels.front();
  const int pre_pileup_start_index =
      light_timing.pre_pileup_start_index[reference_ch];
  const int pre_pileup_stop_index =
      light_timing.pre_pileup_stop_index[reference_ch];
  const int post_pileup_start_index =
      light_timing.post_pileup_start_index[reference_ch];
  const int post_pileup_stop_index =
      light_timing.post_pileup_stop_index[reference_ch];

  for (int raw_idx = 0; raw_idx < waveform_len; ++raw_idx) {
    double voltage_sum = 0.0;
    for (const int light_ch : valid_channels) {
      voltage_sum += lightVoltageAtSample(cfg, tpc_tree_buffer, light_ch, raw_idx);
    }
    updateLightPeaksByIndex(peaks,
                            raw_idx,
                            pre_pileup_start_index,
                            pre_pileup_stop_index,
                            post_pileup_start_index,
                            post_pileup_stop_index,
                            voltage_sum / static_cast<double>(valid_channels.size()));
  }
  return peaks;
}

void mergeLightPeaks(LightPeaks& merged_peaks, const LightPeaks& peaks)
{
  merged_peaks.peak = std::max(merged_peaks.peak, peaks.peak);
  merged_peaks.pre_roi_peak =
      std::max(merged_peaks.pre_roi_peak, peaks.pre_roi_peak);
  merged_peaks.post_roi_peak =
      std::max(merged_peaks.post_roi_peak, peaks.post_roi_peak);
}

LightPeaks analyzeEachLightChannel(const Config& cfg,
                                   const TPCTreeBuffer& tpc_tree_buffer,
                                   const LightTimingState& light_timing,
                                   const std::vector<int>& valid_channels)
{
  LightPeaks merged_peaks;
  for (const int light_ch : valid_channels) {
    mergeLightPeaks(
        merged_peaks,
        analyzeSingleLightChannel(cfg, tpc_tree_buffer, light_timing, light_ch));
  }
  return merged_peaks;
}

LightPeaks analyzeLightChannelGroup(const Config& cfg,
                                    const TPCTreeBuffer& tpc_tree_buffer,
                                    const LightTimingState& light_timing,
                                    const std::vector<int>& valid_channels)
{
  if (cfg.light_waveform_analysis == "average") {
    return analyzeAverageLightWaveform(cfg,
                                       tpc_tree_buffer,
                                       light_timing,
                                       valid_channels);
  }

  if (cfg.light_waveform_analysis == "each_channel") {
    return analyzeEachLightChannel(cfg,
                                   tpc_tree_buffer,
                                   light_timing,
                                   valid_channels);
  }

  throw std::runtime_error("Unknown light waveform analysis mode: " +
                           cfg.light_waveform_analysis);
}

} // namespace

std::string normalizeLightWaveformAnalysis(const std::string& mode)
{
  if (mode == "average") {
    return "average";
  }
  if (mode == "each_channel") {
    return "each_channel";
  }
  throw std::runtime_error(
      "light.waveform_analysis must be average or each_channel.");
}

LightStatus analyzeLightEvent(const Config& cfg,
                              const TPCTreeBuffer& tpc_tree_buffer,
                              const LightTimingState& light_timing,
                              bool light_ok)
{
  LightStatus status;
  const std::vector<int> valid_general_channels =
      collectValidLightChannels(cfg.general_analysis_channels,
                                tpc_tree_buffer,
                                light_ok);
  const std::vector<int> valid_pileup_channels =
      collectValidLightChannels(cfg.pileup_analysis_channels,
                                tpc_tree_buffer,
                                light_ok);

  status.general_valid = !valid_general_channels.empty();
  status.pileup_valid = !valid_pileup_channels.empty();
  status.valid_any = status.general_valid || status.pileup_valid;
  if (!status.valid_any) {
    return status;
  }

  if (cfg.light_waveform_analysis == "average") {
    validateAverageAnalysisGroups(tpc_tree_buffer,
                                  valid_general_channels,
                                  valid_pileup_channels);
  }

  if (status.general_valid) {
    updateGeneralLightStatusFromPeaks(
        status,
        cfg,
        analyzeLightChannelGroup(cfg,
                                 tpc_tree_buffer,
                                 light_timing,
                                 valid_general_channels));
  }

  if (status.pileup_valid) {
    updatePileupLightStatusFromPeaks(
        status,
        cfg,
        analyzeLightChannelGroup(cfg,
                                 tpc_tree_buffer,
                                 light_timing,
                                 valid_pileup_channels));
  }

  return status;
}

double lightRoiIntegralCharge(const Config& cfg,
                              const TPCTreeBuffer& tpc_tree_buffer,
                              const LightTimingState& light_timing,
                              bool light_ok)
{
  const std::vector<int> valid_general_channels =
      collectValidLightChannels(cfg.general_analysis_channels,
                                tpc_tree_buffer,
                                light_ok);
  if (valid_general_channels.empty()) {
    return 0.0 * unit::coulomb;
  }

  return lightRoiIntegralChargeForChannels(cfg,
                                           tpc_tree_buffer,
                                           light_timing,
                                           valid_general_channels);
}

} /* namespace grams */
} /* namespace comptonsoft */
