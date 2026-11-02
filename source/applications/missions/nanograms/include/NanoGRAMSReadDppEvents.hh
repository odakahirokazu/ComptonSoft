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
 * @file NanoGRAMSReadDppEvents.hh
 * @brief Reader of the dpplist tree (nanograms-readout) into the light data of NanoGRAMS.
 *
 * Each entry of the dpplist tree has a waveform of a single DPP channel.
 * The input files are read as one TChain, and consecutive entries with the same trigger ID
 * are grouped into one event (also across the input files).
 * The waveforms are filled into LightData in the same way as the waveforms of the tpctree
 * (NanoGRAMSReadTPCEvents). Only the version 2 format (with trigger ID) is supported.
 * The DPP configuration is common to all the input files (dpp_config_file of NanoGRAMSLoadConfig).
 */

#ifndef COMPTONSOFT_NanoGRAMSReadDppEvents_H
#define COMPTONSOFT_NanoGRAMSReadDppEvents_H 1

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "NanoGRAMSConstants.hh"
#include "NanoGRAMSLoadConfig.hh"
#include "VCSModule.hh"

class TChain;

namespace ngUtil {
class DppRootIO;
}

namespace comptonsoft {

namespace grams {

/**
 * waveforms of one trigger, in the same format as the waveforms of TPCTreeBuffer
 */
struct DppEventBuffer
{
  void clear();
  bool waveRegistered(int channel) const { return wave_num[channel] > 0; }

  uint32_t trigger_id = 0;
  uint64_t real_time = 0;
  std::array<uint16_t, NUM_CH_DPP_MAX> wave_compress{};
  std::array<std::array<int16_t, MAX_WAVE_LENGTH>, NUM_CH_DPP_MAX> waveform{};
  std::array<int32_t, NUM_CH_DPP_MAX> wave_num{};
};

} /* namespace grams */

class NanoGRAMSReadDppEvents : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSReadDppEvents, 1.0);

public:
  NanoGRAMSReadDppEvents();
  ~NanoGRAMSReadDppEvents() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  int64_t currentRawEventId() const { return current_raw_event_id_; }
  const grams::DppEventBuffer& currentDppBuffer() const { return buffer_; }

private:
  void openDppChain();
  bool readNextEntry();
  bool readNextDppEvent();
  void addCurrentEntry();
  void fillEventsIntoDetectors();
  void setupLightDataLayout();
  void selectLightChannels();

  std::vector<std::string> dpplist_files_;
  std::string tree_name_ = "dpplist";

  // declared before dpp_root_io_, which refers to the chain
  std::unique_ptr<TChain> chain_;
  std::unique_ptr<ngUtil::DppRootIO> dpp_root_io_;
  int64_t num_entries_ = 0;
  int64_t current_entry_ = 0;
  // the entry read last is not yet added to an event (it belongs to the next trigger)
  bool pending_entry_ = false;
  bool layout_ready_ = false;
  // incomplete events skipped before the layout is set
  int events_waiting_for_layout_ = 0;

  grams::DppEventBuffer buffer_;
  int64_t processed_entries_ = 0;
  int64_t current_raw_event_id_ = -1;
  int64_t incomplete_events_ = 0;
  int64_t duplicated_channel_entries_ = 0;
  // DPP channels used in the light analysis (general or pileup); only these are filled into light data
  std::array<bool, NUM_CH_DPP_MAX> light_channel_used_{};

  NanoGRAMSLoadConfig* configLoader_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSReadDppEvents_H */
