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

/**
 * @file NanoGRAMSTPCTreeUtil.hh
 * @brief Public utility API for NanoGRAMS data reduction.
 * @author Satoshi Takashima
 * @date 2026-05-17
 */

#ifndef COMPTONSOFT_NanoGRAMSTPCTreeUtil_H
#define COMPTONSOFT_NanoGRAMSTPCTreeUtil_H 1

#include <array>
#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "NanoGRAMSEvent.hh"

class TFile;
class TTree;

namespace comptonsoft
{
namespace ngUtil
{

constexpr const char* kTpcTreeName    = "tpctree";
constexpr const char* kRawHitTreeName = "rawhittree";
constexpr const char* kHitTreeName    = "hittree";
constexpr const char* kErrorFlagsBranchName = "error_flags";

struct Config
{
  int daq_time      = 0;
  int delay_counts  = 0;
  int pix_min       = 0;
  int pix_max       = 0;
  int circ_min_hits = 0;

  double adc2mv              = (1.0 / 8192.0) * 1000.0;
  double adc_min             = 0.0;
  double adc_max             = 0.0;
  double light_peak_thr_mV   = 0.0;
  double circ_thr            = 0.0;
  double spread_thr          = 0.0;
  double drift_time_max_us   = 0.0;
  double late_window_us      = 0.0;
  double late_peak_thr_mV    = 0.0;
  double noise_th            = 0.0;
  double circ_min_ratio      = 0.0;
  double timebin_ns_override = 0.0;

  std::vector<int> light_channels = {4, 6, 5, 7};
  std::map<int, std::vector<int>> exclude_pix;
};

struct RawFECHit
{
  int fec           = 0;
  uint64_t ti       = 0;
  double drift_us   = 0.0;
  std::vector<int16_t> channels;
  std::vector<float>   adcs;
};

void readConfig(Config& cfg, const std::string& config_path);

using PixelAdc  = std::array<double, NUM_CH_CHARGE>;
using PixelMask = std::array<uint8_t, NUM_CH_CHARGE>;

struct FECChannelGeometry
{
  std::array<std::array<std::pair<int, int>, NUM_CH_CHARGE>, NUM_CHARGE_READOUT> xy_of_ch{};
  std::array<std::array<std::vector<int>, NUM_CH_CHARGE>, NUM_CHARGE_READOUT> cross_neighbors{};
  std::array<std::array<std::vector<int>, NUM_CH_CHARGE>, NUM_CHARGE_READOUT> diag_neighbors{};
  std::array<std::vector<int>, NUM_CHARGE_READOUT> periphery{};
};

struct TPCTreeLayout
{
  int64_t n_entries      = 0;
  int reg_len            = NUM_CH_LIGHT;
  int waveform_total_len = 0;
  int waveform_len       = 0;
  // int waveform_len_ds    = 0;
};

struct TPCTreeEntryData
{
  explicit TPCTreeEntryData(int waveform_len)
      : wave_compress(NUM_CH_LIGHT, 0),
        registered(std::make_unique<bool[]>(NUM_CH_LIGHT)),
        adc(NUM_CHARGE_READOUT * NUM_CH_CHARGE, 0),
        drift(NUM_CHARGE_READOUT, 0),
        ti(NUM_CHARGE_READOUT, 0),
        waveform(NUM_CH_LIGHT * waveform_len, 0)
  {
  }

  std::vector<uint16_t> wave_compress;
  std::unique_ptr<bool[]> registered;
  std::vector<uint16_t> adc;
  std::vector<uint32_t> drift;
  std::vector<uint32_t> ti;
  std::vector<int16_t> waveform;
  uint16_t error_flags = 0;
};

struct FECSelectionContext
{
  FECChannelGeometry geom;
  bool include_diag = false;
  std::vector<PixelMask> masks;
  std::vector<int> min_periph_hits;
};

struct LightTimingState
{
  bool ready = false;
  bool warned_wave_compress_entries = false;
  std::array<uint16_t, NUM_CH_LIGHT> wave_compress{};
  std::array<double, NUM_CH_LIGHT> timebin_ns{};
  std::array<int, NUM_CH_LIGHT> late_index{};
};

class FECTITracker
{
public:
  FECTITracker();
  uint64_t absoluteTi(int fec, uint32_t ti_value);

private:
  std::vector<uint64_t> overflow_;
  std::vector<uint32_t> prev_ti_;
  std::vector<uint8_t> have_prev_ti_;
};

class TPCTreeRawHitReader
{
public:
  TPCTreeRawHitReader(TTree* tpc_tree, const Config& cfg);
  ~TPCTreeRawHitReader();

  bool processNext(int64_t& raw_event_id, std::vector<RawFECHit>& event_hits);

private:
  TTree* tpc_tree_ = nullptr;
  Config cfg_;
  TPCTreeLayout tpc_tree_layout_;
  TPCTreeEntryData tpc_tree_entry_data_;
  FECSelectionContext fec_selection_;
  LightTimingState light_timing_;
  FECTITracker fec_ti_tracker_;
  int64_t current_entry_ = 0;
};

class RawHitTreeOutputWriter
{
public:
  explicit RawHitTreeOutputWriter(const std::string& output_file_path);
  ~RawHitTreeOutputWriter();

  void fillEvent(int64_t event_id,
                 int64_t raw_event_id,
                 const std::vector<RawFECHit>& hits);
  std::string close();

private:
  void bindBranches();

  std::filesystem::path output_path_;
  std::unique_ptr<TFile> file_;
  std::unique_ptr<TTree> rawhit_tree_;
  int64_t eventid_    = 0;
  int64_t raweventid_ = 0;
  int16_t ihit_       = 0;
  int64_t ti_         = 0;
  int32_t num_hits_   = 0;
  float adc_          = 0.0F;
  int16_t fecid_      = 0;
  int16_t ch_         = 0;
  float drifttime_    = 0.0F;
};

} /* namespace ngUtil */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSTPCTreeUtil_H */
