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

#include "NanoGRAMSTPCTreeUtil.hh"

#include <TFile.h>
#include <TLeaf.h>
#include <TTree.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <yaml-cpp/yaml.h>

namespace comptonsoft
{
namespace ngUtil
{

namespace
{

struct LightStatus
{
  bool valid_any = false;
  bool gamma  = false;
  bool cosmic = false;
  //bool pileup    = false;
  bool pileup_pre_roi  = false;
  bool pileup_post_roi = false;

  bool hasPileup() const
  {
    return pileup_pre_roi || pileup_post_roi;
  }
};

//from lower right
constexpr std::array<std::array<int, NUM_CH_EACH_VATA>, NUM_VATA> kPlotNumAll = {{
    {0, 8, 23, 24, 39, 40, 55, 63,
     1, 9, 22, 25, 38, 41, 54, 62,
     2, 10, 21, 26, 37, 42, 53, 61,
     3, 11, 20, 27, 36, 43, 52, 60,
     4, 12, 19, 28, 35, 44, 51, 59,
     5, 13, 18, 29, 34, 45, 50, 58,
     6, 14, 17, 30, 33, 46, 49, 57,
     7, 15, 16, 31, 32, 47, 48, 56},
    {63, 62, 61, 60, 59, 58, 57, 56,
     55, 54, 53, 52, 51, 50, 49, 48,
     40, 41, 42, 43, 44, 45, 46, 47,
     39, 38, 37, 36, 35, 34, 33, 32,
     24, 25, 26, 27, 28, 29, 30, 31,
     23, 22, 21, 20, 19, 18, 17, 16,
     8, 9, 10, 11, 12, 13, 14, 15,
     0, 1, 2, 3, 4, 5, 6, 7},
    {56, 48, 47, 32, 31, 16, 15, 7,
     57, 49, 46, 33, 30, 17, 14, 6,
     58, 50, 45, 34, 29, 18, 13, 5,
     59, 51, 44, 35, 28, 19, 12, 4,
     60, 52, 43, 36, 27, 20, 11, 3,
     61, 53, 42, 37, 26, 21, 10, 2,
     62, 54, 41, 38, 25, 22, 9, 1,
     63, 55, 40, 39, 24, 23, 8, 0},
    {7, 6, 5, 4, 3, 2, 1, 0,
     15, 14, 13, 12, 11, 10, 9, 8,
     16, 17, 18, 19, 20, 21, 22, 23,
     31, 30, 29, 28, 27, 26, 25, 24,
     32, 33, 34, 35, 36, 37, 38, 39,
     47, 46, 45, 44, 43, 42, 41, 40,
     48, 49, 50, 51, 52, 53, 54, 55,
     56, 57, 58, 59, 60, 61, 62, 63}
}};

void readLightConfig(Config& cfg, const YAML::Node& node)
{
  const auto nodeLight = node["light"];
  cfg.delay_counts     = nodeLight["delay_counts"].as<int>();
  //cfg.light_peak_thr   = nodeLight["light_peak_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.light_gamma_thr  = nodeLight["light_gamma_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.light_cosmic_thr = nodeLight["light_cosmic_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.pre_roi_window   = nodeLight["pre_roi_window_us"].as<double>() * unit::us;
  cfg.post_roi_window  = nodeLight["post_roi_window_us"].as<double>() * unit::us;
  cfg.pre_roi_peak_thr = nodeLight["pre_roi_peak_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.post_roi_peak_thr = nodeLight["post_roi_peak_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.light_channels   = nodeLight["light_channels"].as<std::vector<int>>();
  //cfg.late_window      = nodeLight["late_window_us"].as<double>() * unit::us;

  std::cout << "readLightConfig()" << std::endl;
  std::cout << "delay_counts:       " << cfg.delay_counts << std::endl;
  //std::cout << "light_peak_thr_mV:  " << cfg.light_peak_thr / (unit::volt/1000.0) << std::endl;
  std::cout << "light_gamma_thr_mV:  "  << cfg.light_gamma_thr / (unit::volt/1000.0) << std::endl;
  std::cout << "light_cosmic_thr_mV:  " << cfg.light_cosmic_thr / (unit::volt/1000.0) << std::endl;
  std::cout << "pre_roi_window_us:  "   << cfg.pre_roi_window / unit::us << std::endl;
  std::cout << "post_roi_window_us: "   << cfg.post_roi_window / unit::us << std::endl;
  std::cout << "pre_roi_peak_thr_mV:  " << cfg.pre_roi_peak_thr  / (unit::volt/1000.0) << std::endl;
  std::cout << "post_roi_peak_thr_mV: " << cfg.post_roi_peak_thr  / (unit::volt/1000.0) << std::endl;
  //std::cout << "late_window_us:      " << cfg.late_window / unit::us << std::endl;

  std::cout << "light_channels: [ ";
  bool first_light_channel = true;
  for (const int light_ch : cfg.light_channels) {
    if (first_light_channel) {
      std::cout << light_ch;
      first_light_channel = false;
    } else {
      std::cout << ", " << light_ch;
    }
  }
  std::cout << " ]" << std::endl;
}

void readChargeConfig(Config& cfg, const YAML::Node& node)
{
  const auto nodeCharge = node["charge"];
  cfg.pix_min           = nodeCharge["pix_min"].as<int>();
  cfg.pix_max           = nodeCharge["pix_max"].as<int>();
  cfg.circ_min_hits     = nodeCharge["circ_min_hits"].as<int>();
  cfg.adc_min           = nodeCharge["adc_min"].as<double>();
  cfg.adc_max           = nodeCharge["adc_max"].as<double>();
  cfg.circ_thr          = nodeCharge["circ_thr"].as<double>();
  cfg.spread_thr        = nodeCharge["spread_thr"].as<double>();
  cfg.drift_time_max    = nodeCharge["drift_time_max_us"].as<double>() * unit::us;
  cfg.noise_th          = nodeCharge["noise_th"].as<double>();
  cfg.circ_min_ratio    = nodeCharge["circ_min_ratio"].as<double>();

  for (const auto& item : nodeCharge["exclude_pix"]) {
      int key = item.first.as<int>();
      std::vector<int> values = item.second.as<std::vector<int>>();

      cfg.exclude_pix[key] = values;
  }

  std::cout << "pix_min: "           << cfg.pix_min           << std::endl;
  std::cout << "pix_max: "           << cfg.pix_max           << std::endl;
  std::cout << "circ_min_hits: "     << cfg.circ_min_hits     << std::endl;
  std::cout << "adc_min: "           << cfg.adc_min           << std::endl;
  std::cout << "adc_max: "           << cfg.adc_max           << std::endl;
  std::cout << "circ_thr: "          << cfg.circ_thr          << std::endl;
  std::cout << "spread_thr: "        << cfg.spread_thr        << std::endl;
  std::cout << "drift_time_max_us: " << cfg.drift_time_max / unit::us << std::endl;
  std::cout << "noise_th: "          << cfg.noise_th          << std::endl;
  std::cout << "circ_min_ratio: "    << cfg.circ_min_ratio    << std::endl;

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const std::vector<int> pix_vec = cfg.exclude_pix[fec];
    std::cout << fec << ": [ ";
    int index = 0;
    for (const auto& pix : pix_vec) {
      if (index == 0) {
        std::cout << pix;
      } else {
        std::cout << ", " << pix;
      }
      ++index;
    }
    std::cout << " ]" << std::endl;
  }
}

int lowerBoundTimeIndex(double time_window,
                        double trigger_delay,
                        double dt,
                        int waveform_len)
{
  const double raw = (time_window + trigger_delay) / dt;
  int idx = static_cast<int>(std::ceil(raw - 1e-12));
  if (idx < 0) {
    idx = 0;
  }
  if (idx > waveform_len) {
    idx = waveform_len;
  }
  return idx;
}

double waveCompressToTimebin(uint16_t wave_compress)
{
    //if((0<=wave_compress)&&(wave_compress<=8)){
    //    return std::ldexp(1.0, static_cast<int>(wave_compress));
    //} else{
    //    std::cout << "Warning: wave_compress < 0 or wavecompress > 8" << std::endl;
    //    return 1.0;
    //}
    //return 32.0 * unit::ns; //temporary
    return static_cast<double>(wave_compress) * unit::ns;
}

std::filesystem::path prepareOutputPath(const std::string& output_file_path)
{
  if (output_file_path.empty()) {
    throw std::runtime_error("Output file path is empty.");
  }

  std::filesystem::path output_path(output_file_path);
  const auto output_parent = output_path.parent_path();
  if (!output_parent.empty()) {
    std::filesystem::create_directories(output_parent);
  }
  return output_path;
}

double median64(const PixelAdc& values)
{
  std::array<double, NUM_CH_EACH_VATA> sorted = values;
  std::sort(sorted.begin(), sorted.end());
  return 0.5 * (sorted[31] + sorted[32]);
}

double lowerMean(const PixelAdc& values, int n_values)
{
  std::array<double, NUM_CH_EACH_VATA> sorted = values;
  std::sort(sorted.begin(), sorted.end());
  n_values = std::clamp(n_values, 1, NUM_CH_EACH_VATA);
  const double sum = std::accumulate(sorted.begin(),
                                     sorted.begin() + n_values,
                                     0.0);
  return sum / static_cast<double>(n_values);
}

FECChannelGeometry buildFECChannelGeometry()
{
  FECChannelGeometry geom;

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const auto& disp_to_ch = kPlotNumAll[fec];

    for (int xx = 0; xx < 8; ++xx) {
      for (int yy = 0; yy < 8; ++yy) {
        const int ch = disp_to_ch[xx * 8 + yy];
        geom.xy_of_ch[fec][ch] = {xx, yy};
      }
    }

    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      const auto [x, y] = geom.xy_of_ch[fec][ch];

      for (const auto& delta : {std::pair<int, int>{-1, 0},
                                std::pair<int, int>{1, 0},
                                std::pair<int, int>{0, -1},
                                std::pair<int, int>{0, 1}}) {
        const int xx = x + delta.first;
        const int yy = y + delta.second;
        if (0 <= xx && xx < 8 && 0 <= yy && yy < 8) {
          geom.cross_neighbors[fec][ch].push_back(disp_to_ch[xx * 8 + yy]);
        }
      }

      for (const auto& delta : {std::pair<int, int>{-1, -1},
                                std::pair<int, int>{-1, 1},
                                std::pair<int, int>{1, -1},
                                std::pair<int, int>{1, 1}}) {
        const int xx = x + delta.first;
        const int yy = y + delta.second;
        if (0 <= xx && xx < 8 && 0 <= yy && yy < 8) {
          geom.diag_neighbors[fec][ch].push_back(disp_to_ch[xx * 8 + yy]);
        }
      }
    }


    std::vector<int> periphery;
    for (int xx = 0; xx < 8; ++xx) {
      for (int yy = 0; yy < 8; ++yy) {
        if (xx == 0 || xx == 7 || yy == 0 || yy == 7) {
          periphery.push_back(disp_to_ch[xx * 8 + yy]);
        }
      }
    }

    std::sort(periphery.begin(), periphery.end());
    periphery.erase(std::unique(periphery.begin(), periphery.end()), periphery.end());
    geom.periphery[fec] = std::move(periphery);
  }

  return geom;
}

bool collinear3(const FECChannelGeometry& geom, int fec, int a, int b, int c)
{
  const auto [x1, y1] = geom.xy_of_ch[fec][a];
  const auto [x2, y2] = geom.xy_of_ch[fec][b];
  const auto [x3, y3] = geom.xy_of_ch[fec][c];
  return (x2 - x1) * (y3 - y1) == (y2 - y1) * (x3 - x1);
}

PixelMask buildMaskIn(const Config& cfg, int fec)
{
  PixelMask mask{};
  mask.fill(1);

  const auto it = cfg.exclude_pix.find(fec);
  if (it != cfg.exclude_pix.end()) {
    for (int ch : it->second) {
      if (0 <= ch && ch < NUM_CH_EACH_VATA) {
        mask[ch] = 0;
      }
    }
  }
  return mask;
}

int waveformTotalLength(TTree* tpc_tree)
{
  const int total_len = tpc_tree->GetLeaf("waveform")->GetLenStatic();
  if (total_len <= 0 || total_len % NUM_CH_DPP_ON != 0) {
    throw std::runtime_error("Unexpected waveform branch length.");
  }
  return total_len;
}

TPCTreeLayout inspectTPCTreeLayout(TTree* tpc_tree)
{
  TPCTreeLayout tpc_tree_layout;
  tpc_tree_layout.n_entries          = static_cast<int64_t>(tpc_tree->GetEntries());
  tpc_tree_layout.waveform_total_len = waveformTotalLength(tpc_tree);
  tpc_tree_layout.waveform_len       = tpc_tree_layout.waveform_total_len / NUM_CH_DPP_ON;

  std::cout << "inspectTPCTreeLayout()" << std::endl;
  std::cout << "n_entries:          "   << tpc_tree_layout.n_entries         << std::endl;
  std::cout << "num_dpp_enable_ch:  "   << tpc_tree_layout.num_dpp_enable_ch << std::endl;
  std::cout << "waveform_len:       "   << tpc_tree_layout.waveform_len      << std::endl;
  return tpc_tree_layout;
}

LightTimingState makeLightTimingState(const TPCTreeLayout& tpc_tree_layout)
{
  LightTimingState light_timing;
  light_timing.pre_roi_index.fill(0);
  light_timing.post_roi_index.fill(tpc_tree_layout.waveform_len);
  return light_timing;
}

void recordLightTimingFromCurrentEntry(LightTimingState& light_timing,
                                       const Config& cfg,
                                       const TPCTreeBuffer& tpc_tree_buffer)
{
  const TPCTreeLayout& tpc_tree_layout = tpc_tree_buffer.layout();
  std::cout << "[INFO] entries=" << tpc_tree_layout.n_entries
            << " waveform_len=" << tpc_tree_layout.waveform_len
            << " delay_counts=" << cfg.delay_counts << "\n";

  for (int light_ch = 0; light_ch < tpc_tree_layout.num_dpp_enable_ch; ++light_ch) {
    const uint16_t wave_compress = tpc_tree_buffer.wave_compress[light_ch];
    const double dt = waveCompressToTimebin(wave_compress);
    const double trigger_delay = static_cast<double>(cfg.delay_counts) * 8.0 * dt;

    light_timing.wave_compress[light_ch] = wave_compress;
    light_timing.timebin[light_ch] = dt;
    light_timing.pre_roi_index[light_ch] =
        lowerBoundTimeIndex(-cfg.pre_roi_window,
                            trigger_delay,
                            dt,
                            tpc_tree_layout.waveform_len);
    light_timing.post_roi_index[light_ch] =
        lowerBoundTimeIndex(cfg.post_roi_window,
                            trigger_delay,
                            dt,
                            tpc_tree_layout.waveform_len);

    std::cout << "[INFO] light_ch=" << light_ch << "\n";
    std::cout << " wave_compress="  << static_cast<int>(light_timing.wave_compress[light_ch]) << "\n";
    std::cout << " timebin_ns="     << light_timing.timebin[light_ch] / unit::ns << "\n";
    std::cout << " pre_roi_index="  << light_timing.pre_roi_index[light_ch]  << "\n";
    std::cout << " post_roi_index=" << light_timing.post_roi_index[light_ch] << "\n";
  }

  light_timing.ready = true;
}

LightStatus analyzeLightEvent(const Config& cfg,
                              const TPCTreeBuffer& tpc_tree_buffer,
                              const LightTimingState& light_timing,
                              bool light_ok)
{
  LightStatus status;
  const TPCTreeLayout& tpc_tree_layout = tpc_tree_buffer.layout();

  for (int light_ch : cfg.light_channels) {
    if (light_ch < 0 || light_ch >= tpc_tree_layout.num_dpp_enable_ch) {
      continue;
    }
    if (!light_ok || !tpc_tree_buffer.dpp_enable_channels[light_ch]) {
      continue;
    }
    status.valid_any = true;

    double peak          = -std::numeric_limits<double>::infinity() * unit::volt;
    double pre_roi_peak  = -std::numeric_limits<double>::infinity() * unit::volt;
    double post_roi_peak = -std::numeric_limits<double>::infinity() * unit::volt;
    const int waveform_offset = light_ch * tpc_tree_layout.waveform_len;
    const int pre_roi_index   = light_timing.pre_roi_index[light_ch];
    const int post_roi_index  = light_timing.post_roi_index[light_ch];

    for (int raw_idx = 0; raw_idx < tpc_tree_layout.waveform_len; ++raw_idx) {
      const double voltage =
          static_cast<double>(tpc_tree_buffer.waveform[waveform_offset + raw_idx]) * cfg.adc2mv * (unit::volt/1000.0);
      if (raw_idx < pre_roi_index) {
        if (voltage > pre_roi_peak) {
          pre_roi_peak = voltage;
        }
      } else if (raw_idx > post_roi_index) {
        if (voltage > post_roi_peak) {
          post_roi_peak = voltage;
        }
      } else {
        if (voltage > peak) {
          peak = voltage;
        }
      }
    }
    status.cosmic          = status.cosmic || (peak > cfg.light_cosmic_thr);
    status.pileup_pre_roi  = status.pileup_pre_roi || (pre_roi_peak > cfg.pre_roi_peak_thr);
    status.pileup_post_roi = status.pileup_post_roi || (post_roi_peak > cfg.post_roi_peak_thr);
    status.gamma           = status.gamma || (peak > cfg.light_gamma_thr);
    //status.pileup =
    //    status.pileup || (pre_roi_peak > cfg.pre_roi_peak_thr) ||(post_roi_peak > cfg.post_roi_peak_thr);
  }
  return status;
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

FECChargeSelector::FECChargeSelector(const Config& cfg)
    : cfg_(cfg),
      geom_(buildFECChannelGeometry()),
      include_diag_(cfg_.pix_max >= 3) //discussion needed
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    masks_[fec] = buildMaskIn(cfg_, fec);
    const int perimeter = static_cast<int>(geom_.periphery[fec].size());
    min_periph_hits_[fec] =
        std::max(cfg_.circ_min_hits,
                 static_cast<int>(std::ceil(cfg_.circ_min_ratio * perimeter)));
  }
}

std::vector<RawFECHit>
FECChargeSelector::selectHits(const TPCTreeBuffer& tpc_tree_buffer,
                              FECTITracker& fec_ti_tracker,
                              bool subset_mask,
                              bool light_cosmic,
                              bool light_pileup) const
{
  std::vector<RawFECHit> event_hits;
  event_hits.reserve(static_cast<std::size_t>(NUM_VATA));

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    PixelAdc adc_values{};
    for (int pix = 0; pix < NUM_CH_EACH_VATA; ++pix) {
      adc_values[pix] =
          static_cast<double>(tpc_tree_buffer.adc[fec * NUM_CH_EACH_VATA + pix]);
    }

    RawFECHit hit;
    hit.fec        = fec;
    hit.ti         = fec_ti_tracker.absoluteTi(fec, tpc_tree_buffer.ti[fec]);
    hit.drift_time = static_cast<double>(tpc_tree_buffer.drift_time[fec]) * clk_to_us * unit::us + dpp_response_time;

    const bool accepted =
        fillSelectedChannels(adc_values,
                             fec,
                             hit.drift_time,
                             subset_mask,
                             light_cosmic,
                             light_pileup,
                             hit.channels,
                             hit.adcs);
    if (accepted) {
      event_hits.push_back(std::move(hit));
    }
  }

  return event_hits;
}

bool FECChargeSelector::fillSelectedChannels(const PixelAdc& adc_values,
                                             int fec,
                                             double drift_time,
                                             bool subset_mask,
                                             bool light_cosmic,
                                             bool light_pileup,
                                             std::vector<int16_t>& channels,
                                             std::vector<float>& adcs) const
{
  channels.clear();
  adcs.clear();

  const PixelMask& mask_in  = masks_[fec];
  const int min_periph_hits = min_periph_hits_[fec];

  PixelAdc adc_sub{};
  const double cmn = median64(adc_values);
  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    adc_sub[ch] = adc_values[ch] - cmn;
  }

  bool pile_up = !std::isfinite(drift_time) || drift_time >= cfg_.drift_time_max;

  int count_periph = 0;
  for (int ch : geom_.periphery[fec]) {
    if (adc_sub[ch] > cfg_.circ_thr) {
      ++count_periph;
    }
  }

  bool circle_noise = count_periph >= min_periph_hits;
  const double edge_sum = (std::isfinite(adc_sub[0]) ? adc_sub[0] : 0.0) +
                          (std::isfinite(adc_sub[63]) ? adc_sub[63] : 0.0);
  circle_noise = circle_noise || (edge_sum > cfg_.noise_th);

  bool cosmic = light_cosmic;
  pile_up = pile_up || light_pileup;

  int core_idx = 0;
  double core_val = -std::numeric_limits<double>::infinity();
  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    const double value = mask_in[ch] ? adc_sub[ch] : -std::numeric_limits<double>::infinity();
    if (value > core_val) {
      core_val = value;
      core_idx = ch;
    }
  }

  const bool core_has_val = core_val > cfg_.adc_min;
  PixelMask selected{};
  selected.fill(0);

  int adopted_count = 0;
  bool extra_high = false;
  if (core_has_val) {
    std::vector<int> selected_list;
    selected_list.push_back(core_idx);

    PixelMask allowed{};
    allowed.fill(0);
    allowed[core_idx] = 1;
    for (int ch : geom_.cross_neighbors[fec][core_idx]) {
      allowed[ch] = 1;
    }
    if (include_diag_) {
      for (int ch : geom_.diag_neighbors[fec][core_idx]) {
        allowed[ch] = 1;
      }
    }

    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      if (mask_in[ch] && !allowed[ch] && adc_sub[ch] > cfg_.adc_min) {
        extra_high = true;
        break;
      }
    }

    for (int ch : geom_.cross_neighbors[fec][core_idx]) {
      if (mask_in[ch] && adc_sub[ch] > cfg_.spread_thr) {
        selected_list.push_back(ch);
      }
    }
    if (include_diag_) {
      for (int ch : geom_.diag_neighbors[fec][core_idx]) {
        if (mask_in[ch] && adc_sub[ch] > cfg_.spread_thr) {
          selected_list.push_back(ch);
        }
      }
    }

    for (int ch : selected_list) {
      selected[ch] = 1;
    }
    adopted_count = static_cast<int>(selected_list.size());

    if (adopted_count == 3 &&
        collinear3(geom_, fec, selected_list[0], selected_list[1], selected_list[2])) {
      cosmic = true;
    }
  }

  const bool count_ok   = adopted_count >= cfg_.pix_min && adopted_count <= cfg_.pix_max;
  const bool base_valid = !pile_up && !circle_noise && core_has_val;
  const bool event_mask = base_valid && count_ok && !cosmic && !extra_high;

  if (event_mask) {
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      if (!selected[ch]) {
        continue;
      }
      channels.push_back(static_cast<int16_t>(ch));
      adcs.push_back(static_cast<float>(adc_sub[ch]));
    }
  }

  return event_mask && subset_mask && !channels.empty();
}

void readConfig(Config& cfg, const std::string& config_path)
{
  const auto configNode = YAML::LoadFile(config_path);

  readLightConfig(cfg, configNode);
  readChargeConfig(cfg, configNode);

  static_assert(NUM_VATA == static_cast<int>(kPlotNumAll.size()),
                "kPlotNumAll must match NUM_VATA.");

}

TPCTreeBuffer::TPCTreeBuffer(TTree* tpc_tree)
    : tpc_tree_(tpc_tree)
{
  if (!tpc_tree_) {
    throw std::runtime_error("TPCTreeBuffer received a null TTree pointer.");
  }

  layout_ = inspectTPCTreeLayout(tpc_tree_);

  wave_compress.assign(NUM_CH_DPP_ON, 0);
  dpp_enable_channels = std::make_unique<bool[]>(NUM_CH_DPP_ON);
  adc.assign(NUM_VATA * NUM_CH_EACH_VATA, 0);
  drift_time.assign(NUM_VATA, 0);
  ti.assign(NUM_VATA, 0);
  waveform.assign(NUM_CH_DPP_ON * layout_.waveform_len, 0);

  bindBranches(tpc_tree_);
}

void TPCTreeBuffer::getEntry(int64_t entry)
{
  tpc_tree_->GetEntry(entry);
}

void TPCTreeBuffer::bindBranches(TTree* tpc_tree)
{
  tpc_tree->SetBranchAddress("adc",           adc.data());
  tpc_tree->SetBranchAddress("drift_time",    drift_time.data());
  tpc_tree->SetBranchAddress("ti",            ti.data());
  tpc_tree->SetBranchAddress("waveform",      waveform.data());
  tpc_tree->SetBranchAddress("wave_compress", wave_compress.data());
  tpc_tree->SetBranchAddress("registered",    dpp_enable_channels.get());
  tpc_tree->SetBranchAddress("error_flags",   &error_flags);
}

TPCTreeReader::TPCTreeReader(TTree* tpc_tree, const Config& cfg)
    : cfg_(cfg),
      tpc_tree_buffer_(tpc_tree),
      fec_selector_(cfg_),
      light_timing_(makeLightTimingState(tpc_tree_buffer_.layout())),
      fec_ti_tracker_()
{
  if (tpc_tree_buffer_.nEntries() > 0) {
    tpc_tree_buffer_.getEntry(0);
    recordLightTimingFromCurrentEntry(light_timing_, cfg_, tpc_tree_buffer_);
  }
}

TPCTreeReader::~TPCTreeReader() = default;

bool TPCTreeReader::processNext(int64_t& raw_event_id,
                                std::vector<RawFECHit>& event_hits)
{
  if (current_entry_ >= tpc_tree_buffer_.nEntries()) {
    return false;
  }

  raw_event_id = current_entry_;
  tpc_tree_buffer_.getEntry(current_entry_);

  //discuss intepretation of the error_flags
  const int err = static_cast<int>(tpc_tree_buffer_.error_flags);
  const bool tpc_ok   = (err == 0);
  const bool light_ok = (err == 0 || err == 4);
  const LightStatus light_status = analyzeLightEvent(cfg_, tpc_tree_buffer_,
                                                     light_timing_, light_ok);
  const bool light_pileup = light_status.hasPileup();
  const bool subset_mask  = tpc_ok && light_status.gamma;

  event_hits = fec_selector_.selectHits(tpc_tree_buffer_,
                                        fec_ti_tracker_,
                                        subset_mask,
                                        light_status.cosmic,
                                        light_pileup);
  if (!tpc_ok) {
    current_event_type_ = TPCEventType::Error;
  } else if (light_pileup) {
    current_event_type_ = TPCEventType::PileUp;
  } else if (light_status.cosmic) {
    current_event_type_ = TPCEventType::Cosmic;
  } else if (light_status.gamma && !event_hits.empty()) {
    current_event_type_ = TPCEventType::Gamma;
  } else {
    current_event_type_ = TPCEventType::Other;
  }
  ++current_entry_;
  return true;
}

RawHitTreeOutputWriter::RawHitTreeOutputWriter(const std::string& output_file_path)
    : output_path_(prepareOutputPath(output_file_path)),
      file_(std::make_unique<TFile>(output_path_.string().c_str(), "RECREATE")),
      rawhit_tree_(std::make_unique<TTree>(kRawHitTreeName, kRawHitTreeName))
{
  if (file_->IsZombie()) {
    throw std::runtime_error("Failed to create output ROOT file: " + output_path_.string());
  }

  // Keep ownership in rawhit_tree_; otherwise ROOT may delete it again with the TFile.
  rawhit_tree_->SetDirectory(nullptr);
  bindBranches();
}

RawHitTreeOutputWriter::~RawHitTreeOutputWriter() = default;

void RawHitTreeOutputWriter::fillEvent(int64_t event_id,
                                       int64_t raw_event_id,
                                       const std::vector<RawFECHit>& hits)
{
  eventid_    = event_id;
  raweventid_ = raw_event_id;
  num_hits_   = static_cast<int32_t>(hits.size());

  for (std::size_t ih=0;ih<hits.size();++ih) {
    const auto& hit = hits[ih];
    ihit_      = static_cast<int16_t>(ih);
    ti_        = static_cast<int64_t>(hit.ti);
    fecid_     = static_cast<int16_t>(hit.fec);
    drifttime_ = static_cast<float>(hit.drift_time);

    for (std::size_t j = 0; j < hit.channels.size(); ++j) {
      ch_  = hit.channels[j];
      adc_ = j < hit.adcs.size() ? hit.adcs[j] : std::numeric_limits<float>::quiet_NaN();
      rawhit_tree_->Fill();
    }
  }
}

std::string RawHitTreeOutputWriter::close()
{
  file_->cd();
  rawhit_tree_->Write();
  file_->Write();
  const auto entries = rawhit_tree_->GetEntries();
  file_->Close();

  std::cout << "[ROOT] Saved file: " << output_path_.string()
            << " (entries=" << entries << ")\n";
  return output_path_.string();
}

void RawHitTreeOutputWriter::bindBranches()
{
  rawhit_tree_->Branch("eventid",     &eventid_,    "eventid/L");
  rawhit_tree_->Branch("raweventid",  &raweventid_, "raweventid/L");
  rawhit_tree_->Branch("ihit",        &ihit_,       "ihit/S");
  rawhit_tree_->Branch("ti",          &ti_,         "ti/L");
  rawhit_tree_->Branch("num_hits",    &num_hits_,   "num_hits/I");
  rawhit_tree_->Branch("adc",         &adc_,        "adc/F");
  rawhit_tree_->Branch("fecid",       &fecid_,      "fecid/S");
  rawhit_tree_->Branch("ch",          &ch_,         "ch/S");
  rawhit_tree_->Branch("drifttime",   &drifttime_,  "drifttime/F");
}

QuickLookTreeOutputWriter::QuickLookTreeOutputWriter(
    const std::string& output_file_path,
    const TPCTreeLayout& tpc_tree_layout)
    : output_path_(prepareOutputPath(output_file_path)),
      file_(std::make_unique<TFile>(output_path_.string().c_str(), "RECREATE")),
      quicklook_tree_(std::make_unique<TTree>(kQuickLookTreeName, kQuickLookTreeName)),
      waveform_len_(tpc_tree_layout.waveform_len),
      waveform_len_branch_(tpc_tree_layout.waveform_len)
{
  if (file_->IsZombie()) {
    throw std::runtime_error("Failed to create quicklook ROOT file: " +
                             output_path_.string());
  }

  adc_cmn_sub_.assign(NUM_VATA * NUM_CH_EACH_VATA, 0.0f);
  waveform_.assign(NUM_CH_DPP_ON * waveform_len_, 0);
  quicklook_tree_->SetDirectory(nullptr);
  bindBranches();
}

QuickLookTreeOutputWriter::~QuickLookTreeOutputWriter() = default;

void QuickLookTreeOutputWriter::fillEvent(int64_t raw_event_id,
                                          TPCEventType event_type,
                                          const TPCTreeBuffer& tpc_tree_buffer)
{
  if (event_type == TPCEventType::Error) {
    return;
  }

  raw_event_id_ = raw_event_id;
  event_type_   = static_cast<int16_t>(event_type);
  cmn_method_   = (event_type == TPCEventType::Cosmic) ? 1 : 0;

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    ti_[fec]         = tpc_tree_buffer.ti[fec];
    drift_time_[fec] = tpc_tree_buffer.drift_time[fec];
  }

  for (int ch = 0; ch < NUM_CH_DPP_ON; ++ch) {
    wave_compress_[ch] = tpc_tree_buffer.wave_compress[ch];
    registered_[ch]    = tpc_tree_buffer.dpp_enable_channels[ch];
  }
  waveform_ = tpc_tree_buffer.waveform;

  fillCmnSubtractedAdc(event_type, tpc_tree_buffer);
  quicklook_tree_->Fill();
}

std::string QuickLookTreeOutputWriter::close()
{
  file_->cd();
  quicklook_tree_->Write();
  file_->Write();
  const auto entries = quicklook_tree_->GetEntries();
  file_->Close();

  std::cout << "[ROOT] Saved quicklook file: " << output_path_.string()
            << " (entries=" << entries << ")\n";
  return output_path_.string();
}

void QuickLookTreeOutputWriter::bindBranches()
{
  adc_leaflist_           = "adc_cmn_sub[" + std::to_string(NUM_VATA) + "][" +
                              std::to_string(NUM_CH_EACH_VATA) + "]/F";
  cmn_leaflist_           = "cmn[" + std::to_string(NUM_VATA) + "]/F";
  ti_leaflist_            = "ti[" + std::to_string(NUM_VATA) + "]/i";
  drift_leaflist_         = "drift_time[" + std::to_string(NUM_VATA) + "]/i";
  wave_compress_leaflist_ = "wave_compress[" + std::to_string(NUM_CH_DPP_ON) + "]/s";
  registered_leaflist_    = "registered[" + std::to_string(NUM_CH_DPP_ON) + "]/O";
  waveform_leaflist_      = "waveform[" + std::to_string(NUM_CH_DPP_ON) + "][" +
                            std::to_string(waveform_len_) + "]/S";

  quicklook_tree_->Branch("raw_event_id", &raw_event_id_, "raw_event_id/L");
  quicklook_tree_->Branch("event_type",   &event_type_,   "event_type/S");
  quicklook_tree_->Branch("cmn_method",   &cmn_method_,   "cmn_method/S");
  quicklook_tree_->Branch("waveform_len", &waveform_len_branch_, "waveform_len/I");
  quicklook_tree_->Branch("adc_cmn_sub",  adc_cmn_sub_.data(),  adc_leaflist_.c_str());
  quicklook_tree_->Branch("cmn",          cmn_.data(),          cmn_leaflist_.c_str());
  quicklook_tree_->Branch("ti",           ti_.data(),           ti_leaflist_.c_str());
  quicklook_tree_->Branch("drift_time",   drift_time_.data(),   drift_leaflist_.c_str());
  quicklook_tree_->Branch("wave_compress", wave_compress_.data(),
                                                                wave_compress_leaflist_.c_str());
  quicklook_tree_->Branch("registered",   registered_.data(),   registered_leaflist_.c_str());
  quicklook_tree_->Branch("waveform",     waveform_.data(),     waveform_leaflist_.c_str());
}

void QuickLookTreeOutputWriter::fillCmnSubtractedAdc(
    TPCEventType event_type,
    const TPCTreeBuffer& tpc_tree_buffer)
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    PixelAdc adc_values{};
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      adc_values[ch] =
          static_cast<double>(tpc_tree_buffer.adc[fec * NUM_CH_EACH_VATA + ch]);
    }

    const double cmn = (event_type == TPCEventType::Cosmic)
        ? lowerMean(adc_values, 10)
        : median64(adc_values);
    cmn_[fec] = static_cast<float>(cmn);

    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      adc_cmn_sub_[fec * NUM_CH_EACH_VATA + ch] =
          static_cast<float>(adc_values[ch] - cmn);
    }
  }
}

} /* namespace ngUtil */
} /* namespace comptonsoft */
