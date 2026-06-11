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
 * @file NanoGRAMSTPCDataProcessor.hh
 * @brief TPC tree processing and output writers for NanoGRAMS data reduction.
 * @author Satoshi Takashima
 * @date 2026-05-17
 */

#ifndef COMPTONSOFT_NanoGRAMSTPCDataProcessor_H
#define COMPTONSOFT_NanoGRAMSTPCDataProcessor_H 1

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "AstroUnits.hh"
#include "NanoGRAMSConfig.hh"
#include "NanoGRAMSFECGeometry.hh"

class TFile;
class TTree;

namespace comptonsoft
{
namespace unit = anlgeant4::unit;

namespace grams
{

struct RawFECHit
{
  int fec           = 0;
  uint64_t ti       = 0;
  double drift_time = 0.0 * unit::us;
  std::vector<int16_t> channel_fecs;
  std::vector<int16_t> channels;
  std::vector<float>   adus;
};

struct FECSelectionInput
{
  const std::array<PixelADU, NUM_VATA>& adu_cmn_sub_values;
  const std::array<double, NUM_VATA>& drift_times;
  std::array<PixelMask, NUM_VATA>& claimed_pixels;
  int fec = 0;
  bool subset_mask = false;
  bool light_cosmic = false;
  bool light_pileup = false;
};

enum class TPCEventType : int16_t
{
  Error  = -1,
  Other  = 0,
  Gamma  = 1,
  Cosmic = 2,
  PileUp = 3,
};

struct TPCTreeLayout
{
  int64_t n_entries      = 0;
  int num_dpp_enable_ch  = NUM_CH_DPP_ON;
  int waveform_total_len = 0;
  int waveform_len       = 0;
};

class TPCTreeBuffer
{
public:
  explicit TPCTreeBuffer(TTree* tpc_tree);

  const TPCTreeLayout& layout() const { return layout_; }
  int64_t nEntries() const { return layout_.n_entries; }
  void getEntry(int64_t entry);

  std::vector<uint16_t>   wave_compress;
  std::unique_ptr<bool[]> dpp_enable_channels;
  std::vector<uint16_t>   adc;
  std::vector<uint32_t>   drift_time;
  std::vector<uint32_t>   ti;
  std::vector<int16_t>    waveform;
  uint16_t error_flags = 0;

private:
  void bindBranches(TTree* tpc_tree);

  TTree* tpc_tree_ = nullptr;
  TPCTreeLayout layout_;
};

struct LightTimingState
{
  bool ready = false;
  std::array<uint16_t, NUM_CH_DPP_ON> wave_compress{};
  std::array<double,   NUM_CH_DPP_ON> timebin{};
  //std::array<int,      NUM_CH_DPP_ON> late_index{};
  std::array<int,      NUM_CH_DPP_ON> pre_roi_index{};
  std::array<int,      NUM_CH_DPP_ON> post_roi_index{};
};

class FECTITracker
{
public:
  FECTITracker();
  uint64_t absoluteTi(int fec, uint32_t ti_value);

private:
  std::vector<uint64_t> overflow_;
  std::vector<uint32_t> prev_ti_;
  std::vector<uint8_t>  have_prev_ti_;
};

class FECChargeSelector
{
public:
  explicit FECChargeSelector(const Config& cfg);

  std::vector<RawFECHit> selectHits(const TPCTreeBuffer& tpc_tree_buffer,
                                    FECTITracker& fec_ti_tracker,
                                    bool subset_mask,
                                    bool light_cosmic,
                                    bool light_pileup) const;

private:
  bool fillSelectedChannels(const FECSelectionInput& input, RawFECHit& hit) const;

  const Config& cfg_;
  FECChannelGeometry geom_;
  bool include_diag_ = false;
  std::array<PixelMask, NUM_VATA> masks_{};
  std::array<int, NUM_VATA> min_periph_hits_{};
};

class TPCTreeReader
{
public:
  TPCTreeReader(TTree* tpc_tree, const Config& cfg);
  ~TPCTreeReader();

  bool processNext(int64_t& raw_event_id, std::vector<RawFECHit>& event_hits);
  const TPCTreeBuffer& currentBuffer() const { return tpc_tree_buffer_; }
  TPCEventType currentEventType() const { return current_event_type_; }

private:
  Config cfg_;
  TPCTreeBuffer       tpc_tree_buffer_;
  FECChargeSelector   fec_selector_;
  LightTimingState    light_timing_;
  FECTITracker        fec_ti_tracker_;
  int64_t current_entry_ = 0;
  TPCEventType current_event_type_ = TPCEventType::Error;
};

class RawHitTreeOutputWriter
{
public:
  explicit RawHitTreeOutputWriter(const std::string& output_file_path);
  ~RawHitTreeOutputWriter();

  void fillEvent(int64_t event_id,
                 int64_t raw_event_id,
                 const   std::vector<RawFECHit>& hits);
  std::string close();

private:
  void bindBranches();

  std::filesystem::path  output_path_;
  std::unique_ptr<TFile> file_;
  std::unique_ptr<TTree> rawhit_tree_;
  int64_t eventid_    = 0;
  int64_t raweventid_ = 0;
  int16_t ihit_       = 0;
  int64_t ti_         = 0;
  int32_t num_hits_   = 0;
  float adu_          = 0.0;
  int16_t fecid_      = 0;
  int16_t ch_         = 0;
  float drifttime_    = 0.0 * unit::us;
};

class QuickLookTreeOutputWriter
{
public:
  explicit QuickLookTreeOutputWriter(const std::string& output_file_path,
                                     const TPCTreeLayout& tpc_tree_layout);
  ~QuickLookTreeOutputWriter();

  void fillEvent(int64_t raw_event_id,
                 TPCEventType event_type,
                 const TPCTreeBuffer& tpc_tree_buffer);
  std::string close();

private:
  void bindBranches();
  void fillCmnSubtractedADU(TPCEventType event_type,
                            const TPCTreeBuffer& tpc_tree_buffer);

  std::filesystem::path  output_path_;
  std::unique_ptr<TFile> file_;
  std::unique_ptr<TTree> quicklook_tree_;
  int waveform_len_ = 0;
  std::string adu_leaflist_;
  std::string cmn_leaflist_;
  std::string ti_leaflist_;
  std::string drift_leaflist_;
  std::string wave_compress_leaflist_;
  std::string registered_leaflist_;
  std::string waveform_leaflist_;

  int64_t raw_event_id_ = 0;
  int16_t event_type_   = 0;
  int16_t cmn_method_   = 0;
  int32_t waveform_len_branch_ = 0;
  std::vector<float> adu_cmn_sub_;
  std::array<float, NUM_VATA> cmn_{};
  std::array<uint32_t, NUM_VATA> ti_{};
  std::array<uint32_t, NUM_VATA> drift_time_{};
  std::array<uint16_t, NUM_CH_DPP_ON> wave_compress_{};
  std::array<bool, NUM_CH_DPP_ON> registered_{};
  std::vector<int16_t> waveform_;
};

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSTPCDataProcessor_H */
