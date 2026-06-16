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

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace comptonsoft
{
namespace grams
{

namespace
{

struct LightPeaks
{
  double peak          = -std::numeric_limits<double>::infinity() * unit::volt;
  double pre_roi_peak  = -std::numeric_limits<double>::infinity() * unit::volt;
  double post_roi_peak = -std::numeric_limits<double>::infinity() * unit::volt;
};

std::vector<int> collectValidLightChannels(const Config& cfg,
                                           const TPCTreeBuffer& tpc_tree_buffer,
                                           bool light_ok)
{
  std::vector<int> valid_channels;
  const TPCTreeLayout& tpc_tree_layout = tpc_tree_buffer.layout();

  if (!light_ok) {
    return valid_channels;
  }

  for (const int light_ch : cfg.light_channels) {
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

void requireSameWaveCompressForAverage(const TPCTreeBuffer& tpc_tree_buffer,
                                       const std::vector<int>& valid_channels)
{
  if (valid_channels.empty()) {
    return;
  }

  const uint16_t reference_wave_compress =
      tpc_tree_buffer.wave_compress[valid_channels.front()];
  for (const int light_ch : valid_channels) {
    if (tpc_tree_buffer.wave_compress[light_ch] != reference_wave_compress) {
      throw std::runtime_error(
          "light.waveform_analysis=average requires the same wave_compress "
          "for all enabled light_channels.");
    }
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
                             int pre_roi_index,
                             int post_roi_index,
                             double voltage)
{
  if (raw_idx < pre_roi_index) {
    peaks.pre_roi_peak = std::max(peaks.pre_roi_peak, voltage);
  } else if (raw_idx > post_roi_index) {
    peaks.post_roi_peak = std::max(peaks.post_roi_peak, voltage);
  } else {
    peaks.peak = std::max(peaks.peak, voltage);
  }
}

void updateLightStatusFromPeaks(LightStatus& status,
                                const Config& cfg,
                                const LightPeaks& peaks)
{
  status.cosmic = status.cosmic || (peaks.peak > cfg.light_cosmic_thr);
  status.pileup_pre_roi =
      status.pileup_pre_roi || (peaks.pre_roi_peak > cfg.out_roi_peak_thr);
  status.pileup_post_roi =
      status.pileup_post_roi || (peaks.post_roi_peak > cfg.out_roi_peak_thr);
  status.gamma = status.gamma || (peaks.peak > cfg.light_gamma_thr);
}

LightPeaks analyzeSingleLightChannel(const Config& cfg,
                                     const TPCTreeBuffer& tpc_tree_buffer,
                                     const LightTimingState& light_timing,
                                     int light_ch)
{
  LightPeaks peaks;
  const int waveform_len = tpc_tree_buffer.layout().waveform_len;
  const int pre_roi_index = light_timing.pre_roi_index[light_ch];
  const int post_roi_index = light_timing.post_roi_index[light_ch];

  for (int raw_idx = 0; raw_idx < waveform_len; ++raw_idx) {
    updateLightPeaksByIndex(peaks,
                            raw_idx,
                            pre_roi_index,
                            post_roi_index,
                            lightVoltageAtSample(cfg, tpc_tree_buffer, light_ch, raw_idx));
  }
  return peaks;
}

LightPeaks analyzeAverageLightWaveform(const Config& cfg,
                                       const TPCTreeBuffer& tpc_tree_buffer,
                                       const LightTimingState& light_timing,
                                       const std::vector<int>& valid_channels)
{
  requireSameWaveCompressForAverage(tpc_tree_buffer, valid_channels);

  LightPeaks peaks;
  if (valid_channels.empty()) {
    return peaks;
  }

  const int waveform_len = tpc_tree_buffer.layout().waveform_len;
  const int reference_ch = valid_channels.front();
  const int pre_roi_index = light_timing.pre_roi_index[reference_ch];
  const int post_roi_index = light_timing.post_roi_index[reference_ch];

  for (int raw_idx = 0; raw_idx < waveform_len; ++raw_idx) {
    double voltage_sum = 0.0;
    for (const int light_ch : valid_channels) {
      voltage_sum += lightVoltageAtSample(cfg, tpc_tree_buffer, light_ch, raw_idx);
    }
    updateLightPeaksByIndex(peaks,
                            raw_idx,
                            pre_roi_index,
                            post_roi_index,
                            voltage_sum / static_cast<double>(valid_channels.size()));
  }
  return peaks;
}

} // namespace

std::string normalizeLightWaveformAnalysis(const std::string& mode)
{
  if (mode == "average" || mode == "mean") {
    return "average";
  }
  if (mode == "each_channel" || mode == "per_channel") {
    return "each_channel";
  }
  throw std::runtime_error(
      "light.waveform_analysis must be average, mean, each_channel, or per_channel.");
}

LightStatus analyzeLightEvent(const Config& cfg,
                              const TPCTreeBuffer& tpc_tree_buffer,
                              const LightTimingState& light_timing,
                              bool light_ok)
{
  LightStatus status;
  const std::vector<int> valid_channels =
      collectValidLightChannels(cfg, tpc_tree_buffer, light_ok);
  if (valid_channels.empty()) {
    return status;
  }
  status.valid_any = true;

  if (cfg.light_waveform_analysis == "average") {
    updateLightStatusFromPeaks(
        status,
        cfg,
        analyzeAverageLightWaveform(cfg, tpc_tree_buffer, light_timing, valid_channels));
    return status;
  }

  if (cfg.light_waveform_analysis == "each_channel") {
    for (const int light_ch : valid_channels) {
      updateLightStatusFromPeaks(
          status,
          cfg,
          analyzeSingleLightChannel(cfg, tpc_tree_buffer, light_timing, light_ch));
    }
    return status;
  }

  throw std::runtime_error("Unknown light waveform analysis mode: " +
                           cfg.light_waveform_analysis);
}

} /* namespace grams */
} /* namespace comptonsoft */
