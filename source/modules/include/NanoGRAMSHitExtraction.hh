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

#ifndef COMPTONSOFT_NanoGRAMSHitExtraction_H
#define COMPTONSOFT_NanoGRAMSHitExtraction_H 1

#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "NanoGRAMSCalibrationData.hh"
#include "NanoGRAMSQuickLookTreeIO.hh"
#include "NanoGRAMSTPCDataProcessor.hh"
#include "NanoGRAMSTPCProperty.hh"
#include "VCSModule.hh"

class TFile;

namespace comptonsoft
{

class NanoGRAMSHitExtraction : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSHitExtraction, 1.0);

public:
  NanoGRAMSHitExtraction();
  ~NanoGRAMSHitExtraction() override;

  anlnext::ANLStatus mod_define()     override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze()    override;
  anlnext::ANLStatus mod_end_run()    override;

  bool hasCurrentEvent() const { return !current_event_hits_.empty(); }
  int64_t currentEventId() const
  {
    if (hasCurrentEvent()) {
      return gamma_events_ - 1;
    }
    return -1;
  }
  int64_t currentRawEventId() const { return current_raw_event_id_; }
  uint32_t currentUnixTime() const { return current_unix_time_; }
  const std::vector<grams::RawFECHit>& currentEventHits() const
  {
    return current_event_hits_;
  }
  const std::string& configFilePath() const { return config_file_; }
  const std::string& tpcTreeFilePath() const { return tpctree_file_; }

private:
  void setupTPCPropertyForHitSelection();
  void updateGainCorrectionForCurrentEvent(uint32_t unix_time);
  bool shouldWriteQuickLook(grams::TPCEventType event_type,
                            const std::vector<grams::RawFECHit>& event_hits) const;

  std::string config_file_     = "";
  std::string tpctree_file_    = "";
  std::string rawhittree_file_ = "";
  std::string quicklook_file_  = "";
  std::string gain_tp_file_    = "";
  std::map<std::string, double> gain_tp_dict_;
  double gain_tp_value_ = 0.0;
  double gain_cache_seconds_ = 60.0;
  std::vector<std::string> quicklook_event_types_;
  int quicklook_num_hits_ = -1;
  bool quicklook_save_waveforms_ = true;

  grams::Config cfg_;
  CalibrationConfig calibration_config_;
  TestPulseGainTable gain_tp_table_;
  TPCProperty tpc_property_;
  std::unique_ptr<TFile> input_file_;
  std::unique_ptr<grams::TPCTreeReader> tpc_tree_reader_;
  std::unique_ptr<grams::RawHitTreeOutputWriter> rawhit_tree_writer_;
  std::unique_ptr<grams::QuickLookTreeOutputWriter> quicklook_tree_writer_;
  int64_t gamma_events_         = 0;
  int64_t processed_entries_    = 0;
  int64_t expected_tpc_entries_ = 0;
  int64_t current_raw_event_id_ = -1;
  int64_t cached_gain_time_bin_ = std::numeric_limits<int64_t>::min();
  uint32_t current_unix_time_ = 0;
  bool use_event_time_gain_ = false;
  std::vector<grams::RawFECHit> current_event_hits_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSHitExtraction_H */
