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

#include "NanoGRAMSTPCDataProcessor.hh"
#include "NanoGRAMSLightAnalysis.hh"
#include "NanoGRAMSTPCProperty.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>

namespace comptonsoft
{
namespace grams
{

namespace
{

int lowerBoundTimeIndex(double time_window,
                        double trigger_delay,
                        double dt,
                        int waveform_len)
{
  const double raw = (time_window + trigger_delay) / dt;
  return std::clamp(static_cast<int>(std::ceil(raw - 1e-12)), 0, waveform_len);
}

double waveCompressToTimebin(uint16_t wave_compress)
{
  return static_cast<double>(wave_compress) * unit::ns;
}

bool usesLightAnalysis(const Config& cfg)
{
  return cfg.light_event_selection_mode != LightEventSelectionMode::Disabled;
}

bool isTPCDataUsable(int error_flags)
{
  if ((error_flags==0)||(error_flags==4)) {
    return true;
  } else {
    return false;
  }
}

bool isLightDataUsable(int error_flags)
{
  if ((error_flags==0)||(error_flags==4)) {
    return true;
  } else {
    return false;
  }
}

bool requiresLightGamma(const Config& cfg)
{
  return cfg.light_event_selection_mode == LightEventSelectionMode::GammaRequired;
}

double median64(const PixelADU& values)
{
  std::array<double, NUM_CH_EACH_VATA> sorted = values;
  std::sort(sorted.begin(), sorted.end());
  return 0.5 * (sorted[31] + sorted[32]);
}

double lowerMean(const PixelADU& values, int n_values)
{
  std::array<double, NUM_CH_EACH_VATA> sorted = values;
  std::sort(sorted.begin(), sorted.end());
  n_values = std::clamp(n_values, 1, NUM_CH_EACH_VATA);
  const double sum = std::accumulate(sorted.begin(),
                                     sorted.begin() + n_values,
                                     0.0);
  return sum / static_cast<double>(n_values);
}

std::pair<int, double> findCoreSignalChannel(const PixelADU& values,
                                             const PixelMask& mask)
{
  int core_ch = 0;
  double core_value = -std::numeric_limits<double>::infinity();

  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    double value = -std::numeric_limits<double>::infinity();
    if (mask[ch] && std::isfinite(values[ch])) {
      value = values[ch];
    }
    if (value > core_value) {
      core_value = value;
      core_ch = ch;
    }
  }

  return {core_ch, core_value};
}

bool isPeripheryPixel(const AnodeChannelTopology& topology, int fec, int ch)
{
  const auto& periphery = topology.periphery[fec];
  return std::find(periphery.begin(), periphery.end(), ch) != periphery.end();
}

bool containsPixel(const std::vector<std::pair<int, int>>& pixels,
                   int fec,
                   int ch)
{
  return std::find(pixels.begin(), pixels.end(), std::pair<int, int>{fec, ch}) !=
         pixels.end();
}

void addPixelIfNew(std::vector<std::pair<int, int>>& pixels, int fec, int ch)
{
  if (!containsPixel(pixels, fec, ch)) {
    pixels.push_back({fec, ch});
  }
}

bool collinear3Global(const AnodeChannelTopology& topology,
                      const std::vector<std::pair<int, int>>& pixels)
{
  const auto [x1, y1] =
      topology.anode_grid_of_channel[pixels[0].first][pixels[0].second];
  const auto [x2, y2] =
      topology.anode_grid_of_channel[pixels[1].first][pixels[1].second];
  const auto [x3, y3] =
      topology.anode_grid_of_channel[pixels[2].first][pixels[2].second];
  return (x2 - x1) * (y3 - y1) == (y2 - y1) * (x3 - x1);
}

double driftTimeFromClock(uint32_t drift_time_count)
{
  return static_cast<double>(drift_time_count) * kClkToUs * unit::us + kDppResponseTime;
}

bool hasTimeUp(const Config& cfg, const TPCTreeBuffer& tpc_tree_buffer)
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    if (driftTimeFromClock(tpc_tree_buffer.drift_time[fec]) < cfg.drift_time_max) {
      return false;
    }
  }
  return true;
}

LightTimingState makeLightTimingState(const TPCTreeLayout& tpc_tree_layout)
{
  LightTimingState light_timing;
  light_timing.pre_pileup_start_index.fill(0);
  light_timing.pre_pileup_stop_index.fill(0);
  light_timing.post_pileup_start_index.fill(tpc_tree_layout.waveform_len);
  light_timing.post_pileup_stop_index.fill(tpc_tree_layout.waveform_len);
  return light_timing;
}

void recordLightTimingFromCurrentEntry(LightTimingState& light_timing,
                                       const Config& cfg,
                                       const TPCTreeBuffer& tpc_tree_buffer)
{
  const TPCTreeLayout& tpc_tree_layout = tpc_tree_buffer.layout();
  std::cout << "[INFO] entries=" << tpc_tree_layout.n_entries
            << " waveform_len=" << tpc_tree_layout.waveform_len
            << " waveform_num_channels=" << tpc_tree_layout.waveform_num_channels
            << "\n";

  for (int light_ch = 0; light_ch < tpc_tree_layout.num_dpp_registered_slots; ++light_ch) {
    if (!tpc_tree_buffer.registered_channels[light_ch]) {
      continue;
    }
    const uint16_t wave_compress = tpc_tree_buffer.wave_compress[light_ch];
    const double dt = waveCompressToTimebin(wave_compress);
    const int delay_counts = cfg.light_delay_counts[light_ch];
    const double trigger_delay = static_cast<double>(delay_counts) * 8.0 * dt;

    light_timing.wave_compress[light_ch] = wave_compress;
    light_timing.timebin[light_ch] = dt;
    light_timing.pre_pileup_start_index[light_ch] =
        lowerBoundTimeIndex(-cfg.drift_time_max,
                            trigger_delay,
                            dt,
                            tpc_tree_layout.waveform_len);
    light_timing.pre_pileup_stop_index[light_ch] =
        lowerBoundTimeIndex(-cfg.pre_roi_window,
                            trigger_delay,
                            dt,
                            tpc_tree_layout.waveform_len);
    light_timing.post_pileup_start_index[light_ch] =
        lowerBoundTimeIndex(cfg.post_roi_window,
                            trigger_delay,
                            dt,
                            tpc_tree_layout.waveform_len);
    light_timing.post_pileup_stop_index[light_ch] =
        lowerBoundTimeIndex(cfg.drift_time_max,
                            trigger_delay,
                            dt,
                            tpc_tree_layout.waveform_len);

    std::cout << "[INFO] light_ch=" << light_ch << "\n";
    std::cout << " delay_counts="  << delay_counts << "\n";
    std::cout << " wave_compress="  << static_cast<int>(light_timing.wave_compress[light_ch]) << "\n";
    std::cout << " timebin_ns="     << light_timing.timebin[light_ch] / unit::ns << "\n";
    std::cout << " pre_pileup_start_index="
              << light_timing.pre_pileup_start_index[light_ch] << "\n";
    std::cout << " pre_pileup_stop_index="
              << light_timing.pre_pileup_stop_index[light_ch] << "\n";
    std::cout << " post_pileup_start_index="
              << light_timing.post_pileup_start_index[light_ch] << "\n";
    std::cout << " post_pileup_stop_index="
              << light_timing.post_pileup_stop_index[light_ch] << "\n";
  }

  light_timing.ready = true;
}

} // namespace

FECTITracker::FECTITracker()
    : overflow_(NUM_VATA, 0),
      prev_ti_(NUM_VATA, 0),
      have_prev_ti_(NUM_VATA, 0)
{}

uint64_t FECTITracker::absoluteTi(int fec, uint32_t ti_value)
{
  if (have_prev_ti_[fec] && ti_value < prev_ti_[fec]) {
    overflow_[fec] += (uint64_t{1} << 32);
  }
  const uint64_t ti_abs = static_cast<uint64_t>(ti_value) + overflow_[fec];
  prev_ti_[fec]         = ti_value;
  have_prev_ti_[fec]    = 1;
  return ti_abs;
}

FECChargeSelector::FECChargeSelector(const Config& cfg,
                                     const TPCProperty& tpc_property)
    : cfg_(cfg),
      tpc_property_(tpc_property),
      anode_topology_(buildAnodeChannelTopology()),
      include_diag_(cfg_.pix_max >= 3) //discussion needed
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    masks_[fec] = buildFECMask(cfg_, fec);
  }
}

std::vector<RawFECHit>
FECChargeSelector::selectHits(const TPCTreeBuffer& tpc_tree_buffer,
                              FECTITracker& fec_ti_tracker,
                              bool charge_selection_enabled,
                              bool light_cosmic,
                              bool light_pileup,
                              bool& rejected_by_excluded_core) const
{
  std::vector<RawFECHit> event_hits;
  event_hits.reserve(static_cast<std::size_t>(NUM_VATA));
  rejected_by_excluded_core = false;

  std::array<PixelADU, NUM_VATA> adu_cmn_sub_values{};
  std::array<PixelADU, NUM_VATA> hit_selection_energy_values{};
  std::array<double, NUM_VATA> drift_times{};
  std::array<uint64_t, NUM_VATA> ti_values{};
  std::array<double, NUM_VATA> core_values{};
  std::array<int, NUM_VATA> fec_order{};

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    PixelADU adu_values{};
    for (int pix = 0; pix < NUM_CH_EACH_VATA; ++pix) {
      adu_values[pix] =
          static_cast<double>(tpc_tree_buffer.adc[fec * NUM_CH_EACH_VATA + pix]);
    }

    const double output_cmn = median64(adu_values);
    const double selection_cmn = lowerMean(adu_values, 10);
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      adu_cmn_sub_values[fec][ch] = adu_values[ch] - output_cmn;
      const double selection_adu_cmn_sub = adu_values[ch] - selection_cmn;
      hit_selection_energy_values[fec][ch] =
          hitSelectionEnergy(fec, ch, selection_adu_cmn_sub);
    }

    drift_times[fec] = driftTimeFromClock(tpc_tree_buffer.drift_time[fec]);
    ti_values[fec]   = fec_ti_tracker.absoluteTi(fec, tpc_tree_buffer.ti[fec]);
    core_values[fec] =
        findCoreSignalChannel(hit_selection_energy_values[fec], masks_[fec]).second;
    fec_order[fec]   = fec;
  }

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    if (hasExcludedCorePixel(fec, hit_selection_energy_values[fec])) {
      rejected_by_excluded_core = true;
      return event_hits;
    }
  }

  std::sort(fec_order.begin(), fec_order.end(), [&](int lhs, int rhs) {
    return core_values[lhs] > core_values[rhs];
  });

  std::array<PixelMask, NUM_VATA> claimed_pixels{};
  for (auto& mask : claimed_pixels) {
    mask.fill(0);
  }

  for (int fec : fec_order) {
    RawFECHit hit;
    hit.fec        = fec;
    hit.ti         = ti_values[fec];
    hit.drift_time = drift_times[fec];

    FECSelectionInput selection_input{adu_cmn_sub_values,
                                      hit_selection_energy_values,
                                      drift_times,
                                      claimed_pixels,
                                      fec,
                                      charge_selection_enabled,
                                      light_cosmic,
                                      light_pileup};
    const bool accepted = fillSelectedChannels(selection_input, hit);
    if (accepted) {
      event_hits.push_back(std::move(hit));
    }
  }

  return event_hits;
}

bool FECChargeSelector::isTimeUp(const FECSelectionInput& input) const
{
  const double drift_time = input.drift_times[input.fec];
  return !std::isfinite(drift_time) || drift_time >= cfg_.drift_time_max;
}

bool FECChargeSelector::isRejectedByTiming(const FECSelectionInput& input) const
{
  return input.light_pileup || isTimeUp(input);
}

bool FECChargeSelector::hasExcludedCorePixel(
    int fec,
    const PixelADU& hit_selection_energy) const
{
  const auto it = cfg_.core_exclude_pix.find(fec);
  if (it == cfg_.core_exclude_pix.end()) {
    return false;
  }

  PixelMask all_pixels{};
  all_pixels.fill(1);
  const auto [max_ch, max_energy] =
      findCoreSignalChannel(hit_selection_energy, all_pixels);
  if (!std::isfinite(max_energy) || max_energy <= cfg_.core_noise_energy_th) {
    return false;
  }

  for (int ch : it->second) {
    if (ch == max_ch) {
      return true;
    }
  }
  return false;
}

PixelMask FECChargeSelector::buildAllowedPixelMask(int fec, int core_ch) const
{
  PixelMask allowed{};
  allowed.fill(0);
  allowed[core_ch] = 1;
  for (int ch : anode_topology_.section_cross_neighbors[fec][core_ch]) {
    allowed[ch] = 1;
  }
  if (include_diag_) {
    for (int ch : anode_topology_.section_diag_neighbors[fec][core_ch]) {
      allowed[ch] = 1;
    }
  }
  return allowed;
}

bool FECChargeSelector::hasExtraHighPixel(const FECSelectionInput& input,
                                          const PixelMask& allowed_pixels) const
{
  const int fec = input.fec;
  const PixelMask& mask_in = masks_[fec];
  const PixelADU& hit_selection_energy = input.hit_selection_energy_values[fec];

  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    if (mask_in[ch] && !allowed_pixels[ch] &&
        hit_selection_energy[ch] > cfg_.core_noise_energy_th) {
      return true;
    }
  }
  return false;
}

std::vector<std::pair<int, int>>
FECChargeSelector::collectClusterPixels(const FECSelectionInput& input,
                                        int core_ch) const
{
  const int fec = input.fec;
  std::vector<std::pair<int, int>> selected_pixels;
  selected_pixels.push_back({fec, core_ch});

  auto addCandidatePixel = [&](int candidate_fec, int ch) {
    // Excluded pixels cannot seed a cluster, but can be absorbed by a real core.
    if (!input.claimed_pixels[candidate_fec][ch] &&
        input.hit_selection_energy_values[candidate_fec][ch] >
            cfg_.spread_thr_energy) {
      addPixelIfNew(selected_pixels, candidate_fec, ch);
    }
  };

  for (int ch : anode_topology_.section_cross_neighbors[fec][core_ch]) {
    addCandidatePixel(fec, ch);
  }
  if (include_diag_) {
    for (int ch : anode_topology_.section_diag_neighbors[fec][core_ch]) {
      addCandidatePixel(fec, ch);
    }
  }

  const bool can_merge_neighbor_section =
      cfg_.cross_fec_merge_drift_time_tolerance >= 0.0 &&
      isPeripheryPixel(anode_topology_, fec, core_ch);

  auto addNeighborSectionPixel = [&](const std::pair<int, int>& pixel) {
    if (!can_merge_neighbor_section) {
      return;
    }

    const int neighbor_fec = pixel.first;
    const int neighbor_ch  = pixel.second;
    const double drift_delta = std::abs(input.drift_times[neighbor_fec] -
                                        input.drift_times[fec]);
    if (std::isfinite(drift_delta) &&
        drift_delta <= cfg_.cross_fec_merge_drift_time_tolerance) {
      addCandidatePixel(neighbor_fec, neighbor_ch);
    }
  };

  for (const auto& pixel : anode_topology_.cross_section_neighbors[fec][core_ch]) {
    addNeighborSectionPixel(pixel);
  }
  if (include_diag_) {
    for (const auto& pixel : anode_topology_.diag_section_neighbors[fec][core_ch]) {
      addNeighborSectionPixel(pixel);
    }
  }

  return selected_pixels;
}

void FECChargeSelector::fillHitChannels(
    const FECSelectionInput& input,
    const std::vector<std::pair<int, int>>& selected_pixels,
    RawFECHit& hit) const
{
  for (const auto& pixel : selected_pixels) {
    const int selected_fec = pixel.first;
    const int ch = pixel.second;
    input.claimed_pixels[selected_fec][ch] = 1;
    hit.channel_fecs.push_back(static_cast<int16_t>(selected_fec));
    hit.channels.push_back(static_cast<int16_t>(ch));
    const float adu_cmn_sub =
        static_cast<float>(input.adu_cmn_sub_values[selected_fec][ch]);
    hit.adus.push_back(adu_cmn_sub);
    hit.energies.push_back(
        hitSelectionEnergy(selected_fec, ch, static_cast<double>(adu_cmn_sub)));
  }
}

bool FECChargeSelector::fillSelectedChannels(const FECSelectionInput& input,
                                             RawFECHit& hit) const
{
  hit.channel_fecs.clear();
  hit.channels.clear();
  hit.energies.clear();
  hit.adus.clear();

  const int fec = input.fec;
  const PixelMask& mask_in = masks_[fec];
  const PixelADU& hit_selection_energy = input.hit_selection_energy_values[fec];
  const auto [core_ch, core_energy] =
      findCoreSignalChannel(hit_selection_energy, mask_in);
  if (core_energy <= cfg_.core_noise_energy_th ||
      input.claimed_pixels[fec][core_ch]) {
    return false;
  }

  const PixelMask allowed_pixels = buildAllowedPixelMask(fec, core_ch);
  const std::vector<std::pair<int, int>> selected_pixels =
      collectClusterPixels(input, core_ch);
  const int adopted_count = static_cast<int>(selected_pixels.size());
  const bool count_ok = adopted_count >= cfg_.pix_min && adopted_count <= cfg_.pix_max;
  const bool cosmic = input.light_cosmic ||
                      (adopted_count == 3 &&
                       collinear3Global(anode_topology_, selected_pixels));
  const bool event_mask =
      input.charge_selection_enabled &&
      !isRejectedByTiming(input) &&
      count_ok &&
      !cosmic &&
      !hasExtraHighPixel(input, allowed_pixels);

  if (event_mask) {
    fillHitChannels(input, selected_pixels, hit);
  }

  return event_mask && !hit.channels.empty();
}

double FECChargeSelector::hitSelectionEnergy(int fec,
                                             int ch,
                                             double adu_cmn_sub) const
{
  if (!std::isfinite(adu_cmn_sub) || adu_cmn_sub <= 0.0) {
    return 0.0 * unit::keV;
  }

  const double correction_factor = tpc_property_.temperatureCorrectionFactor(fec);
  if (!std::isfinite(correction_factor) || correction_factor <= 0.0) {
    return std::numeric_limits<double>::quiet_NaN();
  }

  const double corrected_adu = adu_cmn_sub * correction_factor;
  if (corrected_adu <= 0.0) {
    return 0.0 * unit::keV;
  }

  return tpc_property_.convertADC2keV(fec, ch, corrected_adu);
}

TPCTreeReader::TPCTreeReader(TTree* tpc_tree,
                             const Config& cfg,
                             const TPCProperty& tpc_property)
    : cfg_(cfg),
      tpc_tree_buffer_(tpc_tree),
      fec_selector_(cfg_, tpc_property),
      fec_ti_tracker_()
{
  if (tpc_tree_buffer_.nEntries() > 0) {
    tpc_tree_buffer_.getEntry(0);
    tpc_tree_buffer_.updateWaveformLayoutFromRegisteredChannels();
    light_timing_ = makeLightTimingState(tpc_tree_buffer_.layout());
    recordLightTimingFromCurrentEntry(light_timing_, cfg_, tpc_tree_buffer_);
  }
}

TPCTreeReader::~TPCTreeReader() = default;

bool TPCTreeReader::readNextEntry(int64_t& raw_event_id)
{
  if (current_entry_ >= tpc_tree_buffer_.nEntries()) {
    return false;
  }

  raw_event_id = current_entry_;
  tpc_tree_buffer_.getEntry(current_entry_);
  current_unix_time_ = tpc_tree_buffer_.representativeUnixTime();
  ++current_entry_;
  return true;
}

void TPCTreeReader::extractCurrentEventHits(std::vector<RawFECHit>& event_hits)
{
  const int err = static_cast<int>(tpc_tree_buffer_.error_flags);
  const bool tpc_ok   = isTPCDataUsable(err);
  const bool light_ok = isLightDataUsable(err);
  LightStatus light_status;
  light_status = analyzeLightEvent(cfg_, tpc_tree_buffer_, light_timing_, light_ok);

  bool light_pileup = false;
  bool light_cosmic = false;
  bool charge_selection_enabled = tpc_ok;
  if (usesLightAnalysis(cfg_)) {
    light_pileup = light_status.hasPileup();
    light_cosmic = light_status.cosmic;
    if (requiresLightGamma(cfg_)) {
      charge_selection_enabled = tpc_ok && light_status.gamma;
    }
  }
  const bool time_up      = hasTimeUp(cfg_, tpc_tree_buffer_);

  bool rejected_by_excluded_core = false;
  event_hits = fec_selector_.selectHits(tpc_tree_buffer_,
                                        fec_ti_tracker_,
                                        charge_selection_enabled,
                                        light_cosmic,
                                        light_pileup,
                                        rejected_by_excluded_core);

  if (!tpc_ok) {
    current_event_type_ = TPCEventType::Error;
  } else if (rejected_by_excluded_core) {
    current_event_type_ = TPCEventType::Other;
  } else if (!event_hits.empty()) {
    current_event_type_ = TPCEventType::Gamma;
  } else if (light_cosmic) {
    current_event_type_ = TPCEventType::Cosmic;
  } else if (light_pileup) {
    current_event_type_ = TPCEventType::PileUp;
  } else if (time_up) {
    current_event_type_ = TPCEventType::TimeUp;
  } else {
    current_event_type_ = TPCEventType::Other;
  }

  if (current_event_type_ == TPCEventType::Gamma) {
    const double roi_integral_charge =
        lightRoiIntegralCharge(cfg_, tpc_tree_buffer_, light_timing_, light_ok);
    for (auto& hit : event_hits) {
      hit.light_roi_charge = roi_integral_charge;
    }
  }
}

} /* namespace grams */
} /* namespace comptonsoft */
