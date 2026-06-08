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
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/property_tree/ptree.hpp>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
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
  bool cosmic = false;
  bool pileup = false;
};

struct FECSelectionInput
{
  const Config& cfg;
  const FECSelectionContext& fec_selection;
  const PixelAdc& adc_values;
  const LightStatus& light;
  int fec = 0;
  double drift_us = 0.0;
  bool subset_mask = false;
};

struct FECSelectionResult
{
  bool accepted = false;
  std::vector<int16_t> channels;
  std::vector<float> adcs;
};

struct FECHitBuildContext
{
  const Config& cfg;
  const FECSelectionContext& fec_selection;
  const TPCTreeEntryData& tpc_tree_entry_data;
  FECTITracker& fec_ti_tracker;
  const LightStatus& light;
  bool subset_mask = false;
};

//from lower right
constexpr std::array<std::array<int, NUM_CH_CHARGE>, NUM_CHARGE_READOUT> kPlotNumAll = {{
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

//std::vector<int> readIntVector(const boost::property_tree::ptree& pt,
//                               const std::string& key,
//                               const std::vector<int>& default_value)
//{
//  const auto node = pt.get_child_optional(key);
//  if (!node) {
//    return default_value;
//  }
//
//  std::vector<int> values;
//  values.reserve(node->size());
//  for (const auto& item : *node) {
//    values.push_back(item.second.get_value<int>());
//  }
//  return values;
//}

//std::map<int, std::vector<int>> readExcludePixels(const boost::property_tree::ptree& pt)
//{
//  std::map<int, std::vector<int>> exclude_pix;
//
//  const auto node = pt.get_child_optional("exclude_pix");
//  if (!node) {
//    return exclude_pix;
//  }
//
//  for (const auto& item : *node) {
//    std::vector<int> pixels;
//    pixels.reserve(item.second.size());
//    for (const auto& pixel : item.second) {
//      pixels.push_back(pixel.second.get_value<int>());
//    }
//    exclude_pix[std::stoi(item.first)] = std::move(pixels);
//  }
//
//  return exclude_pix;
//}

//void readLightConfig(Config& cfg, const boost::property_tree::ptree& pt)
void readLightConfig(Config& cfg, const YAML::Node& node)
{
  //cfg.delay_counts      = pt.get<int>(   "delay_counts",      cfg.delay_counts);
  //cfg.light_peak_thr_mV = pt.get<double>("light_peak_thr_mV", cfg.light_peak_thr_mV);
  //cfg.late_window_us    = pt.get<double>("late_window_us",    cfg.late_window_us);
  //cfg.late_peak_thr_mV  = pt.get<double>("late_peak_thr_mV",  cfg.late_peak_thr_mV);
  //cfg.light_channels    = readIntVector(pt, "light_channels", cfg.light_channels);
  //cfg.light_channels    = readIntVector(pt, "light_channels");

  const auto nodeLight  = node["light"];
  cfg.delay_counts      = nodeLight[   "delay_counts"].as<int>();
  cfg.light_peak_thr_mV = nodeLight["light_peak_thr_mV"].as<double>();
  cfg.late_window_us    = nodeLight["late_window_us"].as<double>();
  cfg.late_peak_thr_mV  = nodeLight["late_peak_thr_mV"].as<double>();
  cfg.light_channels    = nodeLight["light_channels"].as<std::vector<int>>();

  std::cout << "readLightConfig()" << std::endl;
  std::cout << "delay_counts:        " << cfg.delay_counts        << std::endl;
  std::cout << "light_peak_thr_mV:   " << cfg.light_peak_thr_mV   << std::endl;
  std::cout << "late_window_us:      " << cfg.late_window_us      << std::endl;
  std::cout << "late_peak_thr_mV:    " << cfg.late_peak_thr_mV    << std::endl;

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

//void readChargeConfig(Config& cfg, const boost::property_tree::ptree& pt)
void readChargeConfig(Config& cfg, const YAML::Node& node)
{
  //cfg.pix_min           = pt.get<int>(   "pix_min",           cfg.pix_min);
  //cfg.pix_max           = pt.get<int>(   "pix_max",           cfg.pix_max);
  //cfg.circ_min_hits     = pt.get<int>(   "circ_min_hits",     cfg.circ_min_hits);
  //cfg.adc_min           = pt.get<double>("adc_min",           cfg.adc_min);
  //cfg.adc_max           = pt.get<double>("adc_max",           cfg.adc_max);
  //cfg.circ_thr          = pt.get<double>("circ_thr",          cfg.circ_thr);
  //cfg.spread_thr        = pt.get<double>("spread_thr",        cfg.spread_thr);
  //cfg.drift_time_max_us = pt.get<double>("drift_time_max_us", cfg.drift_time_max_us);
  //cfg.noise_th          = pt.get<double>("noise_th",          cfg.noise_th);
  //cfg.circ_min_ratio    = pt.get<double>("circ_min_ratio",    cfg.circ_min_ratio);
  const auto nodeCharge = node["charge"];
  cfg.pix_min           = nodeCharge["pix_min"].as<int>();
  cfg.pix_max           = nodeCharge["pix_max"].as<int>();
  cfg.circ_min_hits     = nodeCharge["circ_min_hits"].as<int>();
  cfg.adc_min           = nodeCharge["adc_min"].as<double>();
  cfg.adc_max           = nodeCharge["adc_max"].as<double>();
  cfg.circ_thr          = nodeCharge["circ_thr"].as<double>();
  cfg.spread_thr        = nodeCharge["spread_thr"].as<double>();
  cfg.drift_time_max_us = nodeCharge["drift_time_max_us"].as<double>();
  cfg.noise_th          = nodeCharge["noise_th"].as<double>();
  cfg.circ_min_ratio    = nodeCharge["circ_min_ratio"].as<double>();

  //cfg.exclude_pix = readExcludePixels(pt);
  //std::map<int, std::vector<int>> exclude_pix;
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
  std::cout << "drift_time_max_us: " << cfg.drift_time_max_us << std::endl;
  std::cout << "noise_th: "          << cfg.noise_th          << std::endl;
  std::cout << "circ_min_ratio: "    << cfg.circ_min_ratio    << std::endl;

  for (int fec = 0; fec < NUM_CHARGE_READOUT; ++fec) {
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

int lowerBoundTimeIndex(double late_window_us,
                        double trigger_delay_ns,
                        double dt_ns,
                        int waveform_len)
{
  const double raw = (late_window_us * 1000.0 + trigger_delay_ns) / dt_ns;
  int idx = static_cast<int>(std::ceil(raw - 1e-12));
  if (idx < 0) {
    idx = 0;
  }
  if (idx > waveform_len) {
    idx = waveform_len;
  }
  return idx;
}

double waveCompressToTimebinNs(uint16_t wave_compress)
{
    //if((0<=wave_compress)&&(wave_compress<=8)){
    //    return std::ldexp(1.0, static_cast<int>(wave_compress));
    //} else{
    //    std::cout << "Warning: wave_compress < 0 or wavecompress > 8" << std::endl;
    //    return 1.0;
    //}
    return 32.0; //temporary
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
  std::array<double, NUM_CH_CHARGE> sorted = values;
  std::sort(sorted.begin(), sorted.end());
  return 0.5 * (sorted[31] + sorted[32]);
}

FECChannelGeometry buildFECChannelGeometry()
{
  FECChannelGeometry geom;

  for (int fec = 0; fec < NUM_CHARGE_READOUT; ++fec) {
    const auto& disp_to_ch = kPlotNumAll[fec];

    for (int xx = 0; xx < 8; ++xx) {
      for (int yy = 0; yy < 8; ++yy) {
        const int ch = disp_to_ch[xx * 8 + yy];
        geom.xy_of_ch[fec][ch] = {xx, yy};
      }
    }

    for (int ch = 0; ch < NUM_CH_CHARGE; ++ch) {
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
      if (0 <= ch && ch < NUM_CH_CHARGE) {
        mask[ch] = 0;
      }
    }
  }
  return mask;
}

int waveformTotalLength(TTree* tpc_tree)
{
  const int total_len = tpc_tree->GetLeaf("waveform")->GetLenStatic();
  if (total_len <= 0 || total_len % NUM_CH_LIGHT != 0) {
    throw std::runtime_error("Unexpected waveform branch length.");
  }
  return total_len;
}

TPCTreeLayout inspectTPCTreeLayout(TTree* tpc_tree)
{
  TPCTreeLayout tpc_tree_layout;
  tpc_tree_layout.n_entries          = static_cast<int64_t>(tpc_tree->GetEntries());
  tpc_tree_layout.waveform_total_len = waveformTotalLength(tpc_tree);
  tpc_tree_layout.waveform_len       = tpc_tree_layout.waveform_total_len / NUM_CH_LIGHT;

  std::cout << "inspectTPCTreeLayout()" << std::endl;
  std::cout << "n_entries:          " << tpc_tree_layout.n_entries          << std::endl;
  std::cout << "reg_len:            " << tpc_tree_layout.reg_len            << std::endl;
  std::cout << "waveform_total_len: " << tpc_tree_layout.waveform_total_len << std::endl;
  std::cout << "waveform_len:       " << tpc_tree_layout.waveform_len       << std::endl;
  return tpc_tree_layout;
}

void bindTPCTreeEntryData(TTree* tpc_tree, TPCTreeEntryData& tpc_tree_entry_data)
{
  tpc_tree->SetBranchAddress("adc",           tpc_tree_entry_data.adc.data());
  tpc_tree->SetBranchAddress("drift_time",    tpc_tree_entry_data.drift.data());
  tpc_tree->SetBranchAddress("ti",            tpc_tree_entry_data.ti.data());
  tpc_tree->SetBranchAddress("waveform",      tpc_tree_entry_data.waveform.data());
  tpc_tree->SetBranchAddress("wave_compress", tpc_tree_entry_data.wave_compress.data());
  tpc_tree->SetBranchAddress("registered",    tpc_tree_entry_data.registered.get());
  tpc_tree->SetBranchAddress("error_flags",   &tpc_tree_entry_data.error_flags);
}

FECSelectionContext buildFECSelectionContext(const Config& cfg)
{
  FECSelectionContext ctx;
  ctx.geom         = buildFECChannelGeometry();
  ctx.include_diag = cfg.pix_max >= 3; //discussion needed
  ctx.masks.resize(static_cast<std::size_t>(NUM_CHARGE_READOUT));
  ctx.min_periph_hits.assign(static_cast<std::size_t>(NUM_CHARGE_READOUT), cfg.circ_min_hits);

  for (int fec=0;fec<NUM_CHARGE_READOUT;++fec) {
    ctx.masks[fec] = buildMaskIn(cfg, fec);
    const int perimeter = static_cast<int>(ctx.geom.periphery[fec].size());
    ctx.min_periph_hits[fec] = std::max(
        cfg.circ_min_hits, static_cast<int>(std::ceil(cfg.circ_min_ratio * perimeter)));
  }

  return ctx;
}

LightTimingState makeLightTimingState(const TPCTreeLayout& tpc_tree_layout)
{
  LightTimingState light_timing;
  light_timing.late_index.fill(tpc_tree_layout.waveform_len);
  return light_timing;
}

void initializeLightTimingIfNeeded(LightTimingState& light_timing,
                                   const Config& cfg,
                                   const TPCTreeLayout& tpc_tree_layout,
                                   const TPCTreeEntryData& tpc_tree_entry_data)
{
  if (light_timing.ready) {
    return;
  }

  std::cout << "[INFO] entries=" << tpc_tree_layout.n_entries
            << " waveform_len=" << tpc_tree_layout.waveform_len
            << " delay_counts=" << cfg.delay_counts << "\n";

  for (int light_ch = 0; light_ch < tpc_tree_layout.reg_len; ++light_ch) {
    const uint16_t wave_compress = tpc_tree_entry_data.wave_compress[light_ch];
    const double dt_ns = waveCompressToTimebinNs(wave_compress);
    const double trigger_delay_ns = static_cast<double>(cfg.delay_counts) * 8.0 * dt_ns;

    light_timing.wave_compress[light_ch] = wave_compress;
    light_timing.timebin_ns[light_ch] = dt_ns;
    light_timing.late_index[light_ch] =
        lowerBoundTimeIndex(cfg.late_window_us,
                            trigger_delay_ns,
                            dt_ns,
                            tpc_tree_layout.waveform_len);

    std::cout << "[INFO] light_ch=" << light_ch
              << " wave_compress=" << static_cast<int>(light_timing.wave_compress[light_ch])
              << " timebin_ns=" << light_timing.timebin_ns[light_ch]
              << " late_index=" << light_timing.late_index[light_ch] << "\n";
  }

  light_timing.ready = true;
}

void warnWaveCompressChangedIfNeeded(LightTimingState& light_timing,
                                     const TPCTreeLayout& tpc_tree_layout,
                                     const TPCTreeEntryData& tpc_tree_entry_data)
{
  if (light_timing.warned_wave_compress_entries) {
    return;
  }

  for (int light_ch = 0; light_ch < tpc_tree_layout.reg_len; ++light_ch) {
    const uint16_t current_wave_compress = tpc_tree_entry_data.wave_compress[light_ch];
    if (current_wave_compress == light_timing.wave_compress[light_ch]) {
      continue;
    }

    std::cerr << "[WARN] wave_compress changed across entries for light_ch=" << light_ch
              << "; first=" << static_cast<int>(light_timing.wave_compress[light_ch])
              << " current=" << static_cast<int>(current_wave_compress)
              << ". Continuing with first-entry timing.\n";
    light_timing.warned_wave_compress_entries = true;
    return;
  }
}

LightStatus analyzeLightEvent(const Config& cfg,
                              const TPCTreeLayout& tpc_tree_layout,
                              const TPCTreeEntryData& tpc_tree_entry_data,
                              const LightTimingState& light_timing,
                              bool light_ok)
{
  LightStatus status;

  for (int light_ch : cfg.light_channels) {
    status.valid_any = status.valid_any || (light_ok && tpc_tree_entry_data.registered[light_ch]);

    double peak      = -std::numeric_limits<double>::infinity();
    double late_peak = -std::numeric_limits<double>::infinity();
    const int waveform_offset = light_ch * tpc_tree_layout.waveform_len;
    const int late_index = light_timing.late_index[light_ch];

    for (int raw_idx = 0; raw_idx < tpc_tree_layout.waveform_len; ++raw_idx) {
      const double mv =
          static_cast<double>(tpc_tree_entry_data.waveform[waveform_offset + raw_idx]) * cfg.adc2mv;
      if (mv > peak) {
        peak = mv;
      }
      if (raw_idx >= late_index && mv > late_peak) {
        late_peak = mv;
      }
    }

    status.cosmic = status.cosmic || (std::isfinite(peak) && peak >= cfg.light_peak_thr_mV);
    status.pileup =
        status.pileup || (std::isfinite(late_peak) && late_peak >= cfg.late_peak_thr_mV);
  }

  return status;
}

FECSelectionResult selectFECEvent(const FECSelectionInput& input)
{
  const Config& cfg                 = input.cfg;
  const FECChannelGeometry& geom    = input.fec_selection.geom;
  const PixelMask& mask_in          = input.fec_selection.masks[input.fec];
  const PixelAdc& adc_values        = input.adc_values;
  const int fec                     = input.fec;
  const int min_periph_hits         = input.fec_selection.min_periph_hits[input.fec];
  const bool include_diag           = input.fec_selection.include_diag;
  const bool subset_mask            = input.subset_mask;
  const bool light_cosmic           = input.light.cosmic;
  const bool light_pileup           = input.light.pileup;

  FECSelectionResult result;

  PixelAdc adc_sub{};
  const double cmn = median64(adc_values);
  for (int ch = 0; ch < NUM_CH_CHARGE; ++ch) {
    adc_sub[ch] = adc_values[ch] - cmn;
  }

  bool pile_up = !std::isfinite(input.drift_us) || input.drift_us >= cfg.drift_time_max_us;

  int count_periph = 0;
  for (int ch : geom.periphery[fec]) {
    if (adc_sub[ch] > cfg.circ_thr) {
      ++count_periph;
    }
  }

  bool circle_noise = count_periph >= min_periph_hits;
  const double edge_sum = (std::isfinite(adc_sub[0]) ? adc_sub[0] : 0.0) +
                          (std::isfinite(adc_sub[63]) ? adc_sub[63] : 0.0);
  circle_noise = circle_noise || (edge_sum > cfg.noise_th);

  bool cosmic = light_cosmic;
  pile_up = pile_up || light_pileup;

  int core_idx = 0;
  double core_val = -std::numeric_limits<double>::infinity();
  for (int ch = 0; ch < NUM_CH_CHARGE; ++ch) {
    const double value = mask_in[ch] ? adc_sub[ch] : -std::numeric_limits<double>::infinity();
    if (value > core_val) {
      core_val = value;
      core_idx = ch;
    }
  }

  const bool core_has_val = core_val > cfg.adc_min;
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
    for (int ch : geom.cross_neighbors[fec][core_idx]) {
      allowed[ch] = 1;
    }
    if (include_diag) {
      for (int ch : geom.diag_neighbors[fec][core_idx]) {
        allowed[ch] = 1;
      }
    }

    for (int ch = 0; ch < NUM_CH_CHARGE; ++ch) {
      if (mask_in[ch] && !allowed[ch] && adc_sub[ch] > cfg.adc_min) {
        extra_high = true;
        break;
      }
    }

    for (int ch : geom.cross_neighbors[fec][core_idx]) {
      if (mask_in[ch] && adc_sub[ch] > cfg.spread_thr) {
        selected_list.push_back(ch);
      }
    }
    if (include_diag) {
      for (int ch : geom.diag_neighbors[fec][core_idx]) {
        if (mask_in[ch] && adc_sub[ch] > cfg.spread_thr) {
          selected_list.push_back(ch);
        }
      }
    }

    for (int ch : selected_list) {
      selected[ch] = 1;
    }
    adopted_count = static_cast<int>(selected_list.size());

    if (adopted_count == 3 &&
        collinear3(geom, fec, selected_list[0], selected_list[1], selected_list[2])) {
      cosmic = true;
    }
  }

  const bool count_ok = adopted_count >= cfg.pix_min && adopted_count <= cfg.pix_max;
  const bool base_valid = !pile_up && !circle_noise && core_has_val;
  const bool event_mask = base_valid && count_ok && !cosmic && !extra_high;
  const bool event_mask_subset = event_mask && subset_mask;

  if (event_mask) {
    for (int ch = 0; ch < NUM_CH_CHARGE; ++ch) {
      if (!selected[ch]) {
        continue;
      }
      result.channels.push_back(static_cast<int16_t>(ch));
      result.adcs.push_back(static_cast<float>(adc_sub[ch]));
    }
  }

  result.accepted = event_mask_subset;
  return result;
}

std::optional<RawFECHit>
buildFECHit(FECHitBuildContext& context, int fec)
{
  const Config& cfg = context.cfg;
  const TPCTreeEntryData& tpc_tree_entry_data = context.tpc_tree_entry_data;

  PixelAdc adc_values{};
  for (int pix = 0; pix < NUM_CH_CHARGE; ++pix) {
    adc_values[pix] =
        static_cast<double>(tpc_tree_entry_data.adc[fec * NUM_CH_CHARGE + pix]);
  }
  constexpr double dpp_response_time_us = 0.68;
  constexpr double clk_to_us = 0.01; //consider 1clk=10ns

  RawFECHit hit;
  hit.fec      = fec;
  hit.ti       = context.fec_ti_tracker.absoluteTi(fec, tpc_tree_entry_data.ti[fec]);
  hit.drift_us = static_cast<double>(tpc_tree_entry_data.drift[fec]) * clk_to_us
               + dpp_response_time_us;

  const FECSelectionInput input{
      cfg,
      context.fec_selection,
      adc_values,
      context.light,
      fec,
      hit.drift_us,
      context.subset_mask};
  FECSelectionResult selection_result = selectFECEvent(input);
  if (!selection_result.accepted || selection_result.channels.empty()) {
    return std::nullopt;
  }

  hit.channels = std::move(selection_result.channels);
  hit.adcs = std::move(selection_result.adcs);
  return hit;
}

std::vector<RawFECHit>
collectEventHits(FECHitBuildContext& context)
{
  std::vector<RawFECHit> event_hits;
  event_hits.reserve(static_cast<std::size_t>(NUM_CHARGE_READOUT));

  for (int fec = 0; fec < NUM_CHARGE_READOUT; ++fec) {
    auto hit = buildFECHit(context, fec);
    if (hit) {
      event_hits.push_back(std::move(*hit));
    }
  }

  return event_hits;
}

} // namespace

FECTITracker::FECTITracker()
    : overflow_(NUM_CHARGE_READOUT, 0),
      prev_ti_(NUM_CHARGE_READOUT, 0),
      have_prev_ti_(NUM_CHARGE_READOUT, 0)
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

void readConfig(Config& cfg, const std::string& config_path)
{
  //boost::property_tree::ptree pt;
  //boost::property_tree::read_json(config_path, pt);
  const auto configNode = YAML::LoadFile(config_path);

  //if (const auto light = pt.get_child_optional("light")) {
    //readLightConfig(cfg, *light);
  readLightConfig(cfg, configNode);
  //}

  //if (const auto charge = pt.get_child_optional("charge")) {
    //readChargeConfig(cfg, *charge);
  readChargeConfig(cfg, configNode);
  //}

  static_assert(NUM_CHARGE_READOUT == static_cast<int>(kPlotNumAll.size()),
                "kPlotNumAll must match NUM_CHARGE_READOUT.");

}

TPCTreeRawHitReader::TPCTreeRawHitReader(TTree* tpc_tree, const Config& cfg)
    : tpc_tree_(tpc_tree),
      cfg_(cfg),
      tpc_tree_layout_(inspectTPCTreeLayout(tpc_tree_)),
      tpc_tree_entry_data_(tpc_tree_layout_.waveform_len),
      fec_selection_(buildFECSelectionContext(cfg_)),
      light_timing_(makeLightTimingState(tpc_tree_layout_)),
      fec_ti_tracker_()
{
  bindTPCTreeEntryData(tpc_tree_, tpc_tree_entry_data_);
}

TPCTreeRawHitReader::~TPCTreeRawHitReader() = default;

bool TPCTreeRawHitReader::processNext(int64_t& raw_event_id,
                                      std::vector<RawFECHit>& event_hits)
{
  if (current_entry_ >= tpc_tree_layout_.n_entries) {
    return false;
  }

  raw_event_id = current_entry_;
  tpc_tree_->GetEntry(current_entry_);

  initializeLightTimingIfNeeded(light_timing_, cfg_, tpc_tree_layout_, tpc_tree_entry_data_);
  warnWaveCompressChangedIfNeeded(light_timing_, tpc_tree_layout_, tpc_tree_entry_data_);

  const int err = static_cast<int>(tpc_tree_entry_data_.error_flags);
  const bool tpc_ok       = (err == 0);
  const bool light_ok     = (err == 0 || err == 4);
  const LightStatus light =
      analyzeLightEvent(cfg_,
                        tpc_tree_layout_,
                        tpc_tree_entry_data_,
                        light_timing_,
                        light_ok);
  const bool subset_mask  = tpc_ok && light.valid_any;

  FECHitBuildContext hit_context{
      cfg_, fec_selection_, tpc_tree_entry_data_, fec_ti_tracker_, light, subset_mask};
  event_hits = collectEventHits(hit_context);
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

  for (std::size_t ih = 0; ih < hits.size(); ++ih) {
    const auto& hit = hits[ih];
    ihit_      = static_cast<int16_t>(ih);
    ti_        = static_cast<int64_t>(hit.ti);
    fecid_     = static_cast<int16_t>(hit.fec);
    drifttime_ = static_cast<float>(hit.drift_us);

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

} /* namespace ngUtil */
} /* namespace comptonsoft */
