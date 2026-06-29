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
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "AstroUnits.hh"
#include "NanoGRAMSConfig.hh"
#include "NanoGRAMSFECGeometry.hh"
#include "NanoGRAMSLightAnalysis.hh"
#include "NanoGRAMSTPCTreeIO.hh"

class TFile;
class TTree;

namespace comptonsoft
{
namespace unit = anlgeant4::unit;

class TPCProperty;

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
  const std::array<PixelADU, NUM_VATA>& hit_selection_energy_values;
  const std::array<double, NUM_VATA>& drift_times;
  std::array<PixelMask, NUM_VATA>& claimed_pixels;
  int fec           = 0;
  bool charge_selection_enabled = false;
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
  TimeUp = 4,
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
  FECChargeSelector(const Config& cfg, const TPCProperty& tpc_property);

  std::vector<RawFECHit> selectHits(const TPCTreeBuffer& tpc_tree_buffer,
                                    FECTITracker& fec_ti_tracker,
                                    bool charge_selection_enabled,
                                    bool light_cosmic,
                                    bool light_pileup,
                                    bool& rejected_by_excluded_core) const;

private:
  bool fillSelectedChannels(const FECSelectionInput& input, RawFECHit& hit) const;
  bool isTimeUp(const FECSelectionInput& input) const;
  bool isRejectedByTiming(const FECSelectionInput& input) const;
  bool hasExcludedCorePixel(int fec, const PixelADU& hit_selection_energy) const;
  PixelMask buildAllowedPixelMask(int fec, int core_ch) const;
  bool hasExtraHighPixel(const FECSelectionInput& input,
                         const PixelMask& allowed_pixels) const;
  std::vector<std::pair<int, int>> collectClusterPixels(const FECSelectionInput& input,
                                                        int core_ch) const;
  void fillHitChannels(const FECSelectionInput& input,
                       const std::vector<std::pair<int, int>>& selected_pixels,
                       RawFECHit& hit) const;
  double hitSelectionEnergy(int fec, int ch, double adu_cmn_sub) const;

  const Config& cfg_;
  const TPCProperty& tpc_property_;
  AnodeChannelTopology anode_topology_;
  bool include_diag_ = false;
  std::array<PixelMask, NUM_VATA> masks_{};
};

class TPCTreeReader
{
public:
  using GainCorrectionUpdater = std::function<void(uint32_t)>;

  TPCTreeReader(TTree* tpc_tree,
                const Config& cfg,
                const TPCProperty& tpc_property,
                GainCorrectionUpdater gain_correction_updater);
  ~TPCTreeReader();

  bool processNext(int64_t& raw_event_id, std::vector<RawFECHit>& event_hits);
  const TPCTreeBuffer& currentBuffer() const { return tpc_tree_buffer_; }
  TPCEventType currentEventType() const { return current_event_type_; }
  uint32_t currentUnixTime() const { return current_unix_time_; }

private:
  Config cfg_;
  TPCTreeBuffer       tpc_tree_buffer_;
  FECChargeSelector   fec_selector_;
  LightTimingState    light_timing_;
  FECTITracker        fec_ti_tracker_;
  GainCorrectionUpdater gain_correction_updater_;
  int64_t current_entry_ = 0;
  TPCEventType current_event_type_ = TPCEventType::Error;
  uint32_t current_unix_time_ = 0;
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

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSTPCDataProcessor_H */
