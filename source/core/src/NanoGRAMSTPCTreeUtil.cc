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

#include <TBranch.h>
#include <TFile.h>
#include <TLeaf.h>
#include <TTree.h>

#include <algorithm>
#include <array>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

namespace comptonsoft
{
namespace ngUtil
{

namespace
{

using PixelAdc = std::array<double, kNPixel>;
using PixelMask = std::array<std::uint8_t, kNPixel>;

struct Geometry
{
  std::array<std::array<std::pair<int, int>, kNPixel>, kDefaultFecNum> xy_of_ch{};
  std::array<std::array<std::vector<int>, kNPixel>, kDefaultFecNum> cross_neighbors{};
  std::array<std::array<std::vector<int>, kNPixel>, kDefaultFecNum> diag_neighbors{};
  std::array<std::vector<int>, kDefaultFecNum> periphery{};
};

struct TreeShape
{
  std::int64_t n_entries = 0;
  int reg_len = 0;
  int waveform_total_len = 0;
  int waveform_len = 0;
  int waveform_len_ds = 0;
  int reference_ch = 0;
};

bool hasBranch(TTree* tree, const std::string& name);
int leafLen(TTree* tree, const std::string& branch_name);
std::string leafType(TTree* tree, const std::string& branch_name);

class OptionalIntBranch
{
public:
  void bind(TTree* tree, const std::string& name)
  {
    branch_name_ = name;
    if (!hasBranch(tree, name)) {
      kind_ = Kind::Missing;
      return;
    }

    const std::string type = leafType(tree, name);
    if (type == "UInt_t") {
      kind_ = Kind::UInt32;
      tree->SetBranchAddress(name.c_str(), &u32_);
    } else if (type == "Int_t" || type == "int") {
      kind_ = Kind::Int32;
      tree->SetBranchAddress(name.c_str(), &i32_);
    } else if (type == "UShort_t") {
      kind_ = Kind::UInt16;
      tree->SetBranchAddress(name.c_str(), &u16_);
    } else if (type == "Short_t" || type == "short") {
      kind_ = Kind::Int16;
      tree->SetBranchAddress(name.c_str(), &i16_);
    } else {
      throw std::runtime_error("Unsupported scalar branch type for " + name + ": " + type);
    }
  }

  bool present() const { return kind_ != Kind::Missing; }

  int value() const
  {
    switch (kind_) {
      case Kind::UInt32:
        return static_cast<int>(u32_);
      case Kind::Int32:
        return static_cast<int>(i32_);
      case Kind::UInt16:
        return static_cast<int>(u16_);
      case Kind::Int16:
        return static_cast<int>(i16_);
      case Kind::Missing:
        return 0;
    }

    return 0;
  }

  const std::string& branchName() const { return branch_name_; }

private:
  enum class Kind { Missing, UInt32, Int32, UInt16, Int16 };

  Kind kind_ = Kind::Missing;
  std::string branch_name_;
  std::uint32_t u32_ = 0;
  std::int32_t i32_ = 0;
  std::uint16_t u16_ = 0;
  std::int16_t i16_ = 0;
};

struct InputBranches
{
  InputBranches() = default;

  InputBranches(const TreeShape& shape, int fec_num)
      : wave_compress(shape.reg_len, 0),
        registered(std::make_unique<bool[]>(shape.reg_len)),
        adc(fec_num * kNPixel, 0),
        drift(fec_num, 0),
        ti(fec_num, 0),
        waveform(shape.waveform_total_len, 0)
  {
  }

  std::vector<std::uint16_t> wave_compress;
  std::unique_ptr<bool[]> registered;
  std::vector<std::uint16_t> adc;
  std::vector<std::uint32_t> drift;
  std::vector<std::uint32_t> ti;
  std::vector<std::int16_t> waveform;
  OptionalIntBranch error_branch;
};

struct SelectionContext
{
  Geometry geom;
  bool include_diag = false;
  std::vector<PixelMask> masks;
  std::vector<int> min_periph_hits;
};

struct TimingState
{
  bool ready = false;
  bool warned_wave_compress_channels = false;
  bool warned_wave_compress_entries = false;
  int reference_wave_compress = 0;
  double timebin_ns = 0.0;
  int late_index = 0;
};

class FecTimingState
{
public:
  explicit FecTimingState(int fec_num)
      : overflow_(fec_num, 0), prev_ti_(fec_num, 0), have_prev_ti_(fec_num, 0)
  {
  }

  std::uint64_t absoluteTi(int fec, std::uint32_t ti_value)
  {
    if (have_prev_ti_[fec] && ti_value < prev_ti_[fec]) {
      overflow_[fec] += (std::uint64_t{1} << 32);
    }
    const std::uint64_t ti_abs = static_cast<std::uint64_t>(ti_value) + overflow_[fec];
    prev_ti_[fec] = ti_value;
    have_prev_ti_[fec] = 1;
    return ti_abs;
  }

private:
  std::vector<std::uint64_t> overflow_;
  std::vector<std::uint32_t> prev_ti_;
  std::vector<std::uint8_t> have_prev_ti_;
};

struct LightStatus
{
  bool valid_any = false;
  bool cosmic = false;
  bool pileup = false;
};

const std::array<std::array<int, kNPixel>, 4> kPlotNumAll = {{
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

bool hasBranch(TTree* tree, const std::string& name)
{
  return tree->GetBranch(name.c_str()) != nullptr;
}

int leafLen(TTree* tree, const std::string& branch_name)
{
  TBranch* branch = tree->GetBranch(branch_name.c_str());
  if (!branch) {
    throw std::runtime_error("Missing required branch: " + branch_name);
  }

  TLeaf* leaf = static_cast<TLeaf*>(branch->GetListOfLeaves()->At(0));
  if (!leaf) {
    throw std::runtime_error("Branch has no leaf: " + branch_name);
  }

  return leaf->GetLenStatic() > 0 ? leaf->GetLenStatic() : leaf->GetLen();
}

std::string leafType(TTree* tree, const std::string& branch_name)
{
  TBranch* branch = tree->GetBranch(branch_name.c_str());
  if (!branch) {
    return "";
  }

  TLeaf* leaf = static_cast<TLeaf*>(branch->GetListOfLeaves()->At(0));
  return leaf ? leaf->GetTypeName() : "";
}

std::vector<int> readIntVector(const boost::property_tree::ptree& pt,
                               const std::string& key,
                               const std::vector<int>& default_value)
{
  const auto node = pt.get_child_optional(key);
  if (!node) {
    return default_value;
  }

  std::vector<int> values;
  values.reserve(node->size());
  for (const auto& item : *node) {
    values.push_back(item.second.get_value<int>());
  }
  return values;
}

std::map<int, std::vector<int>> readExcludePixels(const boost::property_tree::ptree& pt, int fec_num)
{
  std::map<int, std::vector<int>> exclude_pix;
  for (int fec = 0; fec < fec_num; ++fec) {
    exclude_pix[fec] = {0, 63};
  }

  const auto node = pt.get_child_optional("exclude_pix");
  if (!node) {
    return exclude_pix;
  }

  for (const auto& item : *node) {
    std::vector<int> pixels;
    pixels.reserve(item.second.size());
    for (const auto& pixel : item.second) {
      pixels.push_back(pixel.second.get_value<int>());
    }
    exclude_pix[std::stoi(item.first)] = std::move(pixels);
  }

  return exclude_pix;
}

void readLightConfig(Config& cfg, const boost::property_tree::ptree& pt)
{
  cfg.delay_counts = pt.get<int>("delay_counts", cfg.delay_counts);
  cfg.downsample_every = pt.get<int>("downsample_every", cfg.downsample_every);

  cfg.light_peak_thr_mV = pt.get<double>("light_peak_thr_mV", cfg.light_peak_thr_mV);
  cfg.late_window_us = pt.get<double>("late_window_us", cfg.late_window_us);
  cfg.late_peak_thr_mV = pt.get<double>("late_peak_thr_mV", cfg.late_peak_thr_mV);
  cfg.timebin_ns_override = pt.get<double>("timebin_ns_override", cfg.timebin_ns_override);

  cfg.delay_units = pt.get<std::string>("delay_units", cfg.delay_units);
  cfg.light_channels = readIntVector(pt, "light_channels", cfg.light_channels);
}

void readChargeConfig(Config& cfg, const boost::property_tree::ptree& pt)
{
  cfg.pix_min = pt.get<int>("pix_min", cfg.pix_min);
  cfg.pix_max = pt.get<int>("pix_max", cfg.pix_max);
  cfg.circ_min_hits = pt.get<int>("circ_min_hits", cfg.circ_min_hits);

  cfg.adc_min = pt.get<double>("adc_min", cfg.adc_min);
  cfg.adc_max = pt.get<double>("adc_max", cfg.adc_max);
  cfg.circ_thr = pt.get<double>("circ_thr", cfg.circ_thr);
  cfg.spread_thr = pt.get<double>("spread_thr", cfg.spread_thr);
  cfg.drift_time_max_us = pt.get<double>("drift_time_max_us", cfg.drift_time_max_us);
  cfg.noise_th = pt.get<double>("noise_th", cfg.noise_th);
  cfg.circ_min_ratio = pt.get<double>("circ_min_ratio", cfg.circ_min_ratio);

  cfg.exclude_pix = readExcludePixels(pt, cfg.fec_num);
}

double delayFactor(const Config& cfg)
{
  return cfg.delay_units == "samples" ? 1.0 : 8.0;
}

int lowerBoundTimeIndex(double late_window_us,
                        double trigger_delay_ns,
                        double dt_ns,
                        int waveform_len_ds)
{
  if (dt_ns <= 0.0) {
    return waveform_len_ds;
  }

  const double raw = (late_window_us * 1000.0 + trigger_delay_ns) / dt_ns;
  int idx = static_cast<int>(std::ceil(raw - 1e-12));
  if (idx < 0) {
    idx = 0;
  }
  if (idx > waveform_len_ds) {
    idx = waveform_len_ds;
  }
  return idx;
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
  std::array<double, kNPixel> sorted = values;
  std::sort(sorted.begin(), sorted.end());
  return 0.5 * (sorted[31] + sorted[32]);
}

Geometry buildGeometry()
{
  Geometry geom;

  for (int fec = 0; fec < kDefaultFecNum; ++fec) {
    const auto& disp_to_ch = kPlotNumAll[fec];

    for (int xx = 0; xx < 8; ++xx) {
      for (int yy = 0; yy < 8; ++yy) {
        const int ch = disp_to_ch[xx * 8 + yy];
        geom.xy_of_ch[fec][ch] = {xx, yy};
      }
    }

    for (int ch = 0; ch < kNPixel; ++ch) {
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

bool collinear3(const Geometry& geom, int fec, int a, int b, int c)
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
      if (0 <= ch && ch < kNPixel) {
        mask[ch] = 0;
      }
    }
  }

  return mask;
}

TreeShape inspectTreeShape(TTree* tree, const Config& cfg)
{
  const int adc_len     = leafLen(tree, "adc");
  const int drift_len   = leafLen(tree, "drift_time");
  const int ti_len      = leafLen(tree, "ti");
  const int reg_len     = leafLen(tree, "registered");
  const int waveform_total_len = leafLen(tree, "waveform");
  const int wcmp_len           = leafLen(tree, "wave_compress");

  if (adc_len != cfg.fec_num * kNPixel) {
    throw std::runtime_error("adc branch length does not match fec_num * 64.");
  }
  if (drift_len != cfg.fec_num || ti_len != cfg.fec_num) {
    throw std::runtime_error("drift_time/ti branch length does not match fec_num.");
  }
  if (reg_len <= 0 || waveform_total_len % reg_len != 0 || wcmp_len != reg_len) {
    throw std::runtime_error("light waveform branch dimensions are inconsistent.");
  }
  for (int ch : cfg.light_channels) {
    if (ch < 0 || ch >= reg_len) {
      throw std::runtime_error(
          "Configured light channel is outside registered/waveform dimensions.");
    }
  }

  TreeShape shape;
  shape.n_entries = static_cast<std::int64_t>(tree->GetEntries());
  shape.reg_len            = reg_len;
  shape.waveform_total_len = waveform_total_len;
  shape.waveform_len       = waveform_total_len / reg_len;
  shape.waveform_len_ds    = (shape.waveform_len + cfg.downsample_every - 1) / cfg.downsample_every;
  shape.reference_ch = reg_len > 4 ? 4 : 0;
  return shape;
}

InputBranches bindInputBranches(TTree* tree, const Config& cfg, const TreeShape& shape)
{
  InputBranches branches(shape, cfg.fec_num);
  tree->SetBranchAddress("adc", branches.adc.data());
  tree->SetBranchAddress("drift_time", branches.drift.data());
  tree->SetBranchAddress("ti", branches.ti.data());
  tree->SetBranchAddress("waveform", branches.waveform.data());
  tree->SetBranchAddress("wave_compress", branches.wave_compress.data());
  tree->SetBranchAddress("registered", branches.registered.get());
  branches.error_branch.bind(tree, cfg.error_flag_branch);
  return branches;
}

SelectionContext buildSelectionContext(const Config& cfg)
{
  SelectionContext ctx;
  ctx.geom = buildGeometry();
  ctx.include_diag = cfg.pix_max >= 3;
  ctx.masks.resize(static_cast<std::size_t>(cfg.fec_num));
  ctx.min_periph_hits.assign(static_cast<std::size_t>(cfg.fec_num), cfg.circ_min_hits);

  for (int fec = 0; fec < cfg.fec_num; ++fec) {
    ctx.masks[fec] = buildMaskIn(cfg, fec);
    const int perimeter = static_cast<int>(ctx.geom.periphery[fec].size());
    ctx.min_periph_hits[fec] = std::max(
        cfg.circ_min_hits, static_cast<int>(std::ceil(cfg.circ_min_ratio * perimeter)));
  }

  return ctx;
}

TimingState makeTimingState(const Config& cfg, const TreeShape& shape)
{
  TimingState timing;
  timing.timebin_ns = cfg.timebin_ns_override;
  timing.late_index = shape.waveform_len_ds;
  return timing;
}

void initializeTimingIfNeeded(TimingState& timing,
                              const Config& cfg,
                              const TreeShape& shape,
                              const InputBranches& branches)
{
  if (timing.ready) {
    return;
  }

  timing.reference_wave_compress = static_cast<int>(branches.wave_compress[shape.reference_ch]);
  timing.timebin_ns = cfg.timebin_ns_override > 0.0
                          ? cfg.timebin_ns_override
                          : static_cast<double>(timing.reference_wave_compress);

  const double trigger_delay_ns =
      static_cast<double>(cfg.delay_counts) * delayFactor(cfg) * timing.timebin_ns;
  const double dt_ns = timing.timebin_ns * static_cast<double>(cfg.downsample_every);
  timing.late_index =
      lowerBoundTimeIndex(cfg.late_window_us, trigger_delay_ns, dt_ns, shape.waveform_len_ds);

  std::cout << "[INFO] entries=" << shape.n_entries
            << " waveform_len=" << shape.waveform_len
            << " timebin_ns=" << timing.timebin_ns
            << " delay_counts=" << cfg.delay_counts
            << " late_index=" << timing.late_index << "\n";
  timing.ready = true;
}

//substitute timing.warned_wave_compress_channels/entries
void warnWaveCompressIfNeeded(TimingState& timing,
                              const Config& cfg,
                              const TreeShape& shape,
                              const InputBranches& branches)
{
  for (int light_ch = 0; light_ch < shape.reg_len; ++light_ch) {
    if (!timing.warned_wave_compress_channels &&
        static_cast<int>(branches.wave_compress[light_ch]) != timing.reference_wave_compress) {
      std::cerr << "[WARN] wave_compress differs across channels; using channel "
                << shape.reference_ch << " value " << timing.reference_wave_compress
                << " ns/bin.\n";
      timing.warned_wave_compress_channels = true;
      break;
    }
  }

  if (!timing.warned_wave_compress_entries &&
      cfg.timebin_ns_override <= 0.0 &&
      static_cast<int>(branches.wave_compress[shape.reference_ch]) !=
          timing.reference_wave_compress) {
    std::cerr << "[WARN] wave_compress changed across entries; continuing with first entry value "
              << timing.reference_wave_compress << " ns/bin.\n";
    timing.warned_wave_compress_entries = true;
  }
}

LightStatus analyzeLightEvent(const Config& cfg,
                              const TreeShape& shape,
                              const InputBranches& branches,
                              bool light_ok,
                              int late_index)
{
  LightStatus status;

  for (int light_ch : cfg.light_channels) {
    status.valid_any = status.valid_any || (light_ok && branches.registered[light_ch]);

    double peak = -std::numeric_limits<double>::infinity();
    double late_peak = -std::numeric_limits<double>::infinity();
    const int waveform_offset = light_ch * shape.waveform_len;

    int ds_idx = 0;
    for (int raw_idx = 0; raw_idx < shape.waveform_len; raw_idx += cfg.downsample_every, ++ds_idx) {
      const double mv =
          static_cast<double>(branches.waveform[waveform_offset + raw_idx]) * cfg.adc2mv;
      if (mv > peak) {
        peak = mv;
      }
      if (ds_idx >= late_index && mv > late_peak) {
        late_peak = mv;
      }
    }

    status.cosmic = status.cosmic || (std::isfinite(peak) && peak >= cfg.light_peak_thr_mV);
    status.pileup =
        status.pileup || (std::isfinite(late_peak) && late_peak >= cfg.late_peak_thr_mV);
  }

  return status;
}

bool selectFecEvent(const Config& cfg,
                    const Geometry& geom,
                    const PixelMask& mask_in,
                    int fec,
                    int min_periph_hits,
                    bool include_diag,
                    const PixelAdc& adc_values,
                    double drift_us,
                    bool subset_mask,
                    bool light_cosmic,
                    bool light_pileup,
                    std::vector<std::int16_t>& hit_channels,
                    std::vector<float>& hit_adcs)
{
  hit_channels.clear();
  hit_adcs.clear();

  PixelAdc adc_sub{};
  const double cmn = median64(adc_values);
  for (int ch = 0; ch < kNPixel; ++ch) {
    adc_sub[ch] = adc_values[ch] - cmn;
  }

  bool pile_up = !std::isfinite(drift_us) || drift_us >= cfg.drift_time_max_us;

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
  for (int ch = 0; ch < kNPixel; ++ch) {
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

    for (int ch = 0; ch < kNPixel; ++ch) {
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
    for (int ch = 0; ch < kNPixel; ++ch) {
      if (!selected[ch]) {
        continue;
      }
      hit_channels.push_back(static_cast<std::int16_t>(ch));
      hit_adcs.push_back(static_cast<float>(adc_sub[ch]));
    }
  }

  return event_mask_subset;
}

std::optional<StreamingFecHit> buildFecHit(const Config& cfg,
                                           const SelectionContext& selection,
                                           const InputBranches& branches,
                                           FecTimingState& timing,
                                           int fec,
                                           bool subset_mask,
                                           const LightStatus& light)
{
  PixelAdc adc_values{};
  for (int pix = 0; pix < kNPixel; ++pix) {
    adc_values[pix] = static_cast<double>(branches.adc[fec * kNPixel + pix]);
  }

  StreamingFecHit hit;
  hit.fec = fec;
  hit.ti = timing.absoluteTi(fec, branches.ti[fec]);
  hit.drift_us = (static_cast<int>(branches.drift[fec]) + 68) / 100.0;

  const bool selected = selectFecEvent(cfg,
                                       selection.geom,
                                       selection.masks[fec],
                                       fec,
                                       selection.min_periph_hits[fec],
                                       selection.include_diag,
                                       adc_values,
                                       hit.drift_us,
                                       subset_mask,
                                       light.cosmic,
                                       light.pileup,
                                       hit.channels,
                                       hit.adcs);
  if (!selected || hit.channels.empty()) {
    return std::nullopt;
  }

  return hit;
}

std::vector<StreamingFecHit> collectEventHits(const Config& cfg,
                                              const SelectionContext& selection,
                                              const InputBranches& branches,
                                              FecTimingState& timing,
                                              bool subset_mask,
                                              const LightStatus& light)
{
  std::vector<StreamingFecHit> event_hits;
  event_hits.reserve(static_cast<std::size_t>(cfg.fec_num));

  for (int fec = 0; fec < cfg.fec_num; ++fec) {
    auto hit = buildFecHit(cfg, selection, branches, timing, fec, subset_mask, light);
    if (hit) {
      event_hits.push_back(std::move(*hit));
    }
  }

  return event_hits;
}

} // namespace

class StreamingProcessor::Impl
{
public:
  Impl(TTree* tree, const Config& cfg)
      : tree_(tree),
        cfg_(cfg),
        shape_(inspectTreeShape(tree_, cfg_)),
        branches_(bindInputBranches(tree_, cfg_, shape_)),
        selection_(buildSelectionContext(cfg_)),
        timing_(makeTimingState(cfg_, shape_)),
        fec_timing_(cfg_.fec_num)
  {
  }

  bool hasErrorBranch() const { return branches_.error_branch.present(); }

  const std::string& errorBranchName() const { return branches_.error_branch.branchName(); }

  bool processNext(std::int64_t& raw_event_id, std::vector<StreamingFecHit>& event_hits)
  {
    if (current_entry_ >= shape_.n_entries) {
      return false;
    }

    raw_event_id = current_entry_;
    tree_->GetEntry(current_entry_);

    initializeTimingIfNeeded(timing_, cfg_, shape_, branches_);
    warnWaveCompressIfNeeded(timing_, cfg_, shape_, branches_);

    const int err = branches_.error_branch.present() ? branches_.error_branch.value() : 0;
    const bool tpc_ok = branches_.error_branch.present() ? (err == 0) : true;
    const bool light_ok = branches_.error_branch.present() ? (err == 0 || err == 4) : true;
    const LightStatus light = analyzeLightEvent(cfg_, shape_, branches_, light_ok, timing_.late_index);
    const bool subset_mask = tpc_ok && light.valid_any;

    event_hits = collectEventHits(cfg_, selection_, branches_, fec_timing_, subset_mask, light);
    ++current_entry_;
    return true;
  }

private:
  TTree* tree_ = nullptr;
  Config cfg_;
  TreeShape shape_;
  InputBranches branches_;
  SelectionContext selection_;
  TimingState timing_;
  FecTimingState fec_timing_;
  std::int64_t current_entry_ = 0;
};

class RawHitTreeOutputWriter::Impl
{
public:
  explicit Impl(const Config& cfg)
      : output_path_(prepareOutputPath(cfg.output_file_path)),
        file_(output_path_.string().c_str(), "RECREATE"),
        tree_(cfg.output_tree_name.c_str(), cfg.output_tree_name.c_str())
  {
    if (file_.IsZombie()) {
      throw std::runtime_error("Failed to create output ROOT file: " + output_path_.string());
    }

    tree_.SetDirectory(&file_);
    bindBranches();
  }

  void fillEvent(std::int64_t event_id,
                 std::int64_t raw_event_id,
                 const std::vector<StreamingFecHit>& hits)
  {
    eventid_ = event_id;
    raweventid_ = raw_event_id;
    num_hits_ = static_cast<std::int32_t>(hits.size());

    for (std::size_t ih = 0; ih < hits.size(); ++ih) {
      const auto& hit = hits[ih];
      ihit_ = static_cast<std::int16_t>(ih);
      ti_ = static_cast<std::int64_t>(hit.ti);
      fecid_ = static_cast<std::int16_t>(hit.fec);
      drifttime_ = static_cast<float>(hit.drift_us);

      for (std::size_t j = 0; j < hit.channels.size(); ++j) {
        ch_ = hit.channels[j];
        adc_ = j < hit.adcs.size() ? hit.adcs[j] : std::numeric_limits<float>::quiet_NaN();
        tree_.Fill();
      }
    }
  }

  std::string close()
  {
    file_.cd();
    tree_.Write();
    file_.Write();
    const auto entries = tree_.GetEntries();
    file_.Close();

    std::cout << "[ROOT] Saved file: " << output_path_.string()
              << " (entries=" << entries << ")\n";
    return output_path_.string();
  }

private:
  void bindBranches()
  {
    tree_.Branch("eventid", &eventid_, "eventid/L");
    tree_.Branch("raweventid", &raweventid_, "raweventid/L");
    tree_.Branch("ihit", &ihit_, "ihit/S");
    tree_.Branch("ti", &ti_, "ti/L");
    tree_.Branch("num_hits", &num_hits_, "num_hits/I");
    tree_.Branch("adc", &adc_, "adc/F");
    tree_.Branch("fecid", &fecid_, "fecid/S");
    tree_.Branch("ch", &ch_, "ch/S");
    tree_.Branch("drifttime", &drifttime_, "drifttime/F");
  }

  std::filesystem::path output_path_;
  TFile file_;
  TTree tree_;
  std::int64_t eventid_ = 0;
  std::int64_t raweventid_ = 0;
  std::int16_t ihit_ = 0;
  std::int64_t ti_ = 0;
  std::int32_t num_hits_ = 0;
  float adc_ = 0.0F;
  std::int16_t fecid_ = 0;
  std::int16_t ch_ = 0;
  float drifttime_ = 0.0F;
};

void readConfig(Config& cfg, const std::string& config_path)
{
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(config_path, pt);

  cfg.file_path = pt.get<std::string>("file_path", cfg.file_path);
  cfg.output_file_path = pt.get<std::string>("output_file_path", cfg.output_file_path);
  cfg.error_flag_branch = pt.get<std::string>("error_flag_branch", cfg.error_flag_branch);

  if (const auto light = pt.get_child_optional("light")) {
    readLightConfig(cfg, *light);
  } else {
    readLightConfig(cfg, pt);
  }

  if (const auto charge = pt.get_child_optional("charge")) {
    readChargeConfig(cfg, *charge);
  } else {
    readChargeConfig(cfg, pt);
  }

  if (cfg.fec_num <= 0 || cfg.fec_num > static_cast<int>(kPlotNumAll.size())) {
    throw std::runtime_error("fec_num must be between 1 and 4 for the current mapping table.");
  }
  if (cfg.downsample_every <= 0) {
    throw std::runtime_error("downsample_every must be positive.");
  }
}

StreamingProcessor::StreamingProcessor(TTree* tree, const Config& cfg)
    : impl_(std::make_unique<Impl>(tree, cfg))
{
}

StreamingProcessor::~StreamingProcessor() = default;

bool StreamingProcessor::hasErrorBranch() const
{
  return impl_->hasErrorBranch();
}

const std::string& StreamingProcessor::errorBranchName() const
{
  return impl_->errorBranchName();
}

bool StreamingProcessor::processNext(std::int64_t& raw_event_id,
                                     std::vector<StreamingFecHit>& event_hits)
{
  return impl_->processNext(raw_event_id, event_hits);
}

RawHitTreeOutputWriter::RawHitTreeOutputWriter(const Config& cfg)
    : impl_(std::make_unique<Impl>(cfg))
{
}

RawHitTreeOutputWriter::~RawHitTreeOutputWriter() = default;

void RawHitTreeOutputWriter::fillEvent(std::int64_t event_id,
                                       std::int64_t raw_event_id,
                                       const std::vector<StreamingFecHit>& hits)
{
  impl_->fillEvent(event_id, raw_event_id, hits);
}

std::string RawHitTreeOutputWriter::close()
{
  return impl_->close();
}

} /* namespace ngUtil */
} /* namespace comptonsoft */
