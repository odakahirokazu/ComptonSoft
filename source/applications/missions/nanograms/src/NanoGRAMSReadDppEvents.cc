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

#include "NanoGRAMSReadDppEvents.hh"

#include <TChain.h>

#include "AstroUnits.hh"

#include "LightData.hh"
#include "NanoGRAMSDppRootIO.hh"

#include "RealDetectorUnitNanoGRAMS.hh"

#include "boost/format.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

static_assert(MAX_WAVE_LENGTH <= ngUtil::MAX_WAVENUM, "dpplist waveform buffer must hold MAX_WAVE_LENGTH points.");

namespace grams {

void DppEventBuffer::clear()
{
  trigger_id = 0;
  real_time = 0;
  wave_compress.fill(0);
  wave_num.fill(0);
}

} /* namespace grams */

NanoGRAMSReadDppEvents::NanoGRAMSReadDppEvents() = default;

NanoGRAMSReadDppEvents::~NanoGRAMSReadDppEvents() = default;

ANLStatus NanoGRAMSReadDppEvents::mod_define()
{
  define_parameter("dpplist_files", &mod_class::dpplist_files_);
  define_parameter("tree_name", &mod_class::tree_name_);

  return AS_OK;
}

ANLStatus NanoGRAMSReadDppEvents::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  if (dpplist_files_.empty()) {
    throw std::runtime_error("dpplist input file path is empty.");
  }

  processed_entries_ = 0;
  current_raw_event_id_ = -1;
  incomplete_events_ = 0;
  duplicated_channel_entries_ = 0;
  layout_ready_ = false;
  events_waiting_for_layout_ = 0;

  if (!exist_module("NanoGRAMSLoadConfig")) {
    std::cerr << "NanoGRAMSLoadConfig not found" << std::endl;
    return AS_QUIT_ALL_ERROR;
  }

  get_module_NC("NanoGRAMSLoadConfig", &configLoader_);

  // the DPP configuration (light_delay_counts) is common to all the input files
  if (!configLoader_->IsDppConfigSet()) {
    std::cerr << "dpp_config_file of NanoGRAMSLoadConfig is not set" << std::endl;
    return AS_QUIT_ALL_ERROR;
  }

  selectLightChannels();

  openDppChain();

  return AS_OK;
}

void NanoGRAMSReadDppEvents::selectLightChannels()
{
  const auto& cfg = configLoader_->config();
  light_channel_used_.fill(false);
  for (const auto* channels : {&cfg.general_analysis_channels, &cfg.pileup_analysis_channels}) {
    for (const int dppChannel : *channels) {
      light_channel_used_.at(dppChannel) = true;
    }
  }
}

void NanoGRAMSReadDppEvents::openDppChain()
{
  chain_ = std::make_unique<TChain>(tree_name_.c_str());
  for (const std::string& input_path : dpplist_files_) {
    if (input_path.empty()) {
      continue;
    }
    // nentries = 0: the file is opened here to check the tree
    if (chain_->Add(input_path.c_str(), 0) == 0) {
      throw std::runtime_error((boost::format("Missing TTree '%1%' in %2%") % tree_name_ % input_path).str());
    }
    std::cout << "[NanoGRAMSReadDppEvents] input file: " << input_path << "\n";
  }

  num_entries_ = static_cast<int64_t>(chain_->GetEntries());
  current_entry_ = 0;
  pending_entry_ = false;
  dpp_root_io_ = std::make_unique<ngUtil::DppRootIO>(false, 2);
  dpp_root_io_->LoadTree(chain_.get());
  dpp_root_io_->SetBranchAddress();
  std::cout << "[NanoGRAMSReadDppEvents] dpplist entries: " << num_entries_ << "\n";
}

bool NanoGRAMSReadDppEvents::readNextEntry()
{
  if (current_entry_ >= num_entries_) {
    return false;
  }
  dpp_root_io_->GetEntry(current_entry_++);
  ++processed_entries_;
  return true;
}

void NanoGRAMSReadDppEvents::addCurrentEntry()
{
  const int dppChannel = static_cast<int>(dpp_root_io_->GetChannel());
  if (dppChannel >= NUM_CH_DPP_MAX) {
    throw std::runtime_error(
        (boost::format("DPP channel %1% is out of range (entry %2%)") % dppChannel % (current_entry_ - 1)).str());
  }
  if (buffer_.waveRegistered(dppChannel)) {
    // the same channel appears twice in one trigger; the first one is kept
    ++duplicated_channel_entries_;
    return;
  }
  const int waveformLength = static_cast<int>(dpp_root_io_->GetWaveNum());
  if (waveformLength > MAX_WAVE_LENGTH) {
    throw std::runtime_error(
        (boost::format("Waveform length of DPP ch %1% exceeds the maximum: %2%") % dppChannel % waveformLength).str());
  }
  const auto& waveData = dpp_root_io_->GetWaveData();
  std::copy(waveData.begin(), waveData.begin() + waveformLength, buffer_.waveform[dppChannel].begin());
  buffer_.wave_num[dppChannel] = waveformLength;
  buffer_.wave_compress[dppChannel] = dpp_root_io_->GetWaveCompress();
}

bool NanoGRAMSReadDppEvents::readNextDppEvent()
{
  buffer_.clear();
  bool started = false;
  while (pending_entry_ || readNextEntry()) {
    pending_entry_ = false;
    const uint32_t triggerID = dpp_root_io_->GetTriggerID();
    if (started && triggerID != buffer_.trigger_id) {
      // this entry belongs to the next event
      pending_entry_ = true;
      return true;
    }
    if (!started) {
      buffer_.trigger_id = triggerID;
      buffer_.real_time = dpp_root_io_->GetRealTime();
      started = true;
    }
    addCurrentEntry();
  }
  return started;
}

void NanoGRAMSReadDppEvents::fillEventsIntoDetectors()
{
  DetectorSystem* detectorManager = getDetectorManager();
  if (detectorManager == nullptr) {
    return;
  }

  const auto& cfg = configLoader_->config();

  for (auto& detector : detectorManager->getDetectors()) {
    if (!detector->checkType(DetectorType::NanoGRAMS)) {
      continue;
    }
    // always RealDetectorUnitNanoGRAMS
    auto* nanograms = static_cast<RealDetectorUnitNanoGRAMS*>(detector.get());

    // light waveforms (index of light data = DPP channel):
    // RawWaveform = digitizer ADC converted into voltage; Waveform is initialized with it and corrected later
    const double adcToVoltage = cfg.adc2mv * (unit::volt / 1000.0);
    for (int dppChannel = 0; dppChannel < nanograms->NumberOfLightData(); ++dppChannel) {
      LightData* lightData = nanograms->getLightData(dppChannel);
      if (!lightData->isValid()) {
        continue;
      }
      // the waveform length is fixed by the first complete event (setupLightDataLayout)
      const int waveformLength = static_cast<int>(buffer_.wave_num[dppChannel]);
      if (waveformLength != lightData->NumberOfPoints()) {
        throw std::runtime_error((boost::format("Waveform length of DPP ch %1% changed: %2% -> %3%") % dppChannel %
                                  lightData->NumberOfPoints() % waveformLength)
                                     .str());
      }
      std::vector<double>& rawWaveform = lightData->RawWaveform();
      const auto begin = buffer_.waveform[dppChannel].begin();
      std::transform(begin, begin + waveformLength, rawWaveform.begin(),
                     [adcToVoltage](int16_t adc) { return adc * adcToVoltage; });
      lightData->Waveform() = rawWaveform;
    }
  }
}

void NanoGRAMSReadDppEvents::setupLightDataLayout()
{
  DetectorSystem* detectorManager = getDetectorManager();
  if (detectorManager == nullptr) {
    return;
  }
  const auto& cfg = configLoader_->config();

  for (auto& detector : detectorManager->getDetectors()) {
    if (!detector->checkType(DetectorType::NanoGRAMS)) {
      continue;
    }
    auto* nanograms = static_cast<RealDetectorUnitNanoGRAMS*>(detector.get());
    std::cout << "[NanoGRAMSReadDppEvents] light data (detector " << nanograms->getID() << ", "
              << nanograms->NumberOfLightData() << " channels)\n";
    for (int dppChannel = 0; dppChannel < nanograms->NumberOfLightData(); ++dppChannel) {
      const bool registered = buffer_.waveRegistered(dppChannel);
      const bool valid = (registered && light_channel_used_[dppChannel]);
      nanograms->getLightData(dppChannel)->setValid(valid);
      std::cout << "  DPP ch " << dppChannel << ": registered=" << registered
                << " used=" << light_channel_used_[dppChannel] << " valid=" << valid;
      if (!valid) {
        std::cout << "\n";
        continue;
      }
      const double timeWidth = static_cast<double>(buffer_.wave_compress[dppChannel]) * unit::ns;
      // time = 0 at the trigger
      const double triggerDelay = static_cast<double>(cfg.light_delay_counts[dppChannel]) * 8.0 * timeWidth;
      const int waveformLength = static_cast<int>(buffer_.wave_num[dppChannel]);
      nanograms->getLightData(dppChannel)->setLayout(waveformLength, timeWidth, -triggerDelay);
      std::cout << " length=" << waveformLength << " time_width=" << timeWidth / unit::ns << " ns"
                << " time_start=" << -triggerDelay / unit::us << " us\n";
    }
  }
  layout_ready_ = true;
}

ANLStatus NanoGRAMSReadDppEvents::mod_analyze()
{
  if (!readNextDppEvent()) {
    std::cout << "[NanoGRAMSReadDppEvents] AS_QUIT after processing " << processed_entries_ << " dpplist entries ("
              << current_raw_event_id_ + 1 << " events).\n";
    return AS_QUIT;
  }

  ++current_raw_event_id_;
  if (!layout_ready_) {
    // the layout is taken from the first event having all the light channels in use
    // (or the current event if no such event is found within kMaxEventsWaitingForLayout events)
    constexpr int kMaxEventsWaitingForLayout = 100;
    bool complete = true;
    for (int dppChannel = 0; dppChannel < NUM_CH_DPP_MAX; ++dppChannel) {
      if (light_channel_used_[dppChannel] && !buffer_.waveRegistered(dppChannel)) {
        complete = false;
        break;
      }
    }
    if (!complete && events_waiting_for_layout_ < kMaxEventsWaitingForLayout) {
      ++events_waiting_for_layout_;
      ++incomplete_events_;
      return AS_SKIP;
    }
    if (!complete) {
      std::cout << "[NanoGRAMSReadDppEvents] no event with all the light channels in use found in the first "
                << kMaxEventsWaitingForLayout << " events; the layout is taken from the current event\n";
    }
    setupLightDataLayout();
  }

  // events lacking a waveform of a light channel in use are not used at all
  DetectorSystem* detectorManager = getDetectorManager();
  if (detectorManager != nullptr) {
    for (auto& detector : detectorManager->getDetectors()) {
      if (!detector->checkType(DetectorType::NanoGRAMS)) {
        continue;
      }
      auto* nanograms = static_cast<RealDetectorUnitNanoGRAMS*>(detector.get());
      for (int dppChannel = 0; dppChannel < nanograms->NumberOfLightData(); ++dppChannel) {
        if (nanograms->getLightData(dppChannel)->isValid() && !buffer_.waveRegistered(dppChannel)) {
          ++incomplete_events_;
          return AS_SKIP;
        }
      }
    }
  }

  fillEventsIntoDetectors();

  return AS_OK;
}

ANLStatus NanoGRAMSReadDppEvents::mod_end_run()
{
  std::cout << "Skipped events lacking light channels: " << incomplete_events_ << "\n";
  std::cout << "Ignored entries of duplicated channels in a trigger: " << duplicated_channel_entries_ << "\n";
  std::cout << "Total processed dpplist entries: " << processed_entries_ << " (" << current_raw_event_id_ + 1
            << " events)\n";

  dpp_root_io_.reset();
  chain_.reset();
  return AS_OK;
}

} /* namespace comptonsoft */
