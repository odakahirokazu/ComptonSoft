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

#include "NanoGRAMSReadTPCEvents.hh"

#include <TFile.h>
#include <TTree.h>

#include "AstroUnits.hh"

#include "LightData.hh"
#include "NanoGRAMSConstants.hh"

#include "RealDetectorUnitNanoGRAMS.hh"

#include "boost/format.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

static_assert(RealDetectorUnitNanoGRAMS::NumFECs == NUM_VATA, "NanoGRAMS detector sections must correspond to FECs.");
static_assert(RealDetectorUnitNanoGRAMS::NumChannelsPerFEC == NUM_CH_EACH_VATA,
              "NanoGRAMS section channels must correspond to VATA channels.");

NanoGRAMSReadTPCEvents::NanoGRAMSReadTPCEvents() = default;

NanoGRAMSReadTPCEvents::~NanoGRAMSReadTPCEvents() = default;

ANLStatus NanoGRAMSReadTPCEvents::mod_define()
{
  define_parameter("tpctree_files", &mod_class::tpctree_files_);

  define_parameter("run_id", &mod_class::run_id_);

  return AS_OK;
}

ANLStatus NanoGRAMSReadTPCEvents::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  if (tpctree_files_.empty()) {
    throw std::runtime_error("TPC tree input file path is empty.");
  }

  skipped_error_events_ = 0;
  processed_entries_ = 0;
  expected_tpc_entries_ = 0;
  input_file_index_ = 0;
  current_raw_event_offset_ = 0;
  current_raw_event_id_ = -1;

  if (!exist_module("NanoGRAMSLoadConfig")) {
    std::cerr << "NanoGRAMSLoadConfig not found" << std::endl;
    return AS_QUIT_ALL_ERROR;
  }

  get_module_NC("NanoGRAMSLoadConfig", &configLoader_);

  selectLightChannels();

  openNextTPCFile();

  return AS_OK;
}

void NanoGRAMSReadTPCEvents::selectLightChannels()
{
  const auto& cfg = configLoader_->config();
  light_channel_used_.fill(false);
  for (const auto* channels : {&cfg.general_analysis_channels, &cfg.pileup_analysis_channels}) {
    for (const int dppChannel : *channels) {
      light_channel_used_.at(dppChannel) = true;
    }
  }
}

bool NanoGRAMSReadTPCEvents::openNextTPCFile()
{
  tpc_tree_reader_.reset();
  input_file_.reset();
  auto& cfg = configLoader_->config();

  while (input_file_index_ < tpctree_files_.size()) {
    const std::string input_path = tpctree_files_[input_file_index_++];
    if (input_path.empty()) {
      continue;
    }

    // the light data settings (DPP configuration) can be different for each input file
    if (!configLoader_->IsDppConfigSet()) {
      grams::readDPPConfig(cfg, input_path);
    }

    auto input_file = std::make_unique<TFile>(input_path.c_str(), "READ");
    if (input_file->IsZombie()) {
      throw std::runtime_error("Failed to open input ROOT file: " + input_path);
    }
    TTree* tpc_tree = dynamic_cast<TTree*>(input_file->Get(grams::kTpcTreeName));
    if (!tpc_tree) {
      throw std::runtime_error((boost::format("Missing TTree '%1%' in %2%") % grams::kTpcTreeName % input_path).str());
    }

    const int64_t entries = static_cast<int64_t>(tpc_tree->GetEntries());
    if (entries == 0) {
      std::cout << "[NanoGRAMSReadTPCEvents] skip empty tpctree: " << input_path << "\n";
      continue;
    }
    input_file_ = std::move(input_file);
    tpc_tree_reader_ = std::make_unique<grams::TPCTreeReader>(tpc_tree);
    setupLightDataLayout();
    current_raw_event_offset_ = processed_entries_;
    expected_tpc_entries_ += entries;
    std::cout << "[NanoGRAMSReadTPCEvents] input file: " << input_path << "\n"
              << "[NanoGRAMSReadTPCEvents] tpctree entries: " << entries << "\n";

    return true;
  }
  return false;
}

void NanoGRAMSReadTPCEvents::fillEventsIntoDetectors()
{
  DetectorSystem* detectorManager = getDetectorManager();
  if (detectorManager == nullptr) {
    return;
  }

  const auto& cfg = configLoader_->config();

  const grams::TPCTreeBuffer& buffer = tpc_tree_reader_->currentBuffer();
  for (auto& detector : detectorManager->getDetectors()) {
    if (!detector->checkType(DetectorType::NanoGRAMS)) {
      continue;
    }
    // always RealDetectorUnitNanoGRAMS
    auto* nanograms = static_cast<RealDetectorUnitNanoGRAMS*>(detector.get());
    nanograms->setUnixTime(current_unix_time_);
    nanograms->setRunID(run_id_);
    for (int fec = 0; fec < NUM_VATA; ++fec) {
      MultiChannelData* mcd = nanograms->getMultiChannelData(fec);
      for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
        mcd->setRawADC(ch, buffer.adc[fec * NUM_CH_EACH_VATA + ch]);
        mcd->setDataValid(ch, 1);
      }
      // raw TI counter; the wrap-around is handled by the detector unit
      nanograms->setRawTI(fec, buffer.ti[fec]);
      nanograms->setRawDriftTime(fec, buffer.drift_time[fec]);
      nanograms->setDriftTime(fec, grams::driftTimeFromClock(buffer.drift_time[fec]));
    }

    // light waveforms (index of light data = DPP channel):
    // RawWaveform = digitizer ADC converted into voltage; Waveform is initialized with it and corrected later
    const double adcToVoltage = cfg.adc2mv * (unit::volt / 1000.0);
    for (int dppChannel = 0; dppChannel < nanograms->NumberOfLightData(); ++dppChannel) {
      LightData* lightData = nanograms->getLightData(dppChannel);
      if (!lightData->isValid()) {
        continue;
      }
      // the waveform length is fixed by the first entry of the file (setupLightDataLayout)
      const int waveformLength = static_cast<int>(buffer.wave_num[dppChannel]);
      if (waveformLength != lightData->NumberOfPoints()) {
        throw std::runtime_error((boost::format("Waveform length of DPP ch %1% changed within a file: %2% -> %3%")
                                  % dppChannel % lightData->NumberOfPoints() % waveformLength)
                                   .str());
      }
      std::vector<double>& rawWaveform = lightData->RawWaveform();
      const auto begin = buffer.waveform[dppChannel].begin();
      std::transform(begin, begin + waveformLength, rawWaveform.begin(),
                     [adcToVoltage](int16_t adc) { return adc * adcToVoltage; });
      lightData->Waveform() = rawWaveform;
    }
  }
}

void NanoGRAMSReadTPCEvents::setupLightDataLayout()
{
  DetectorSystem* detectorManager = getDetectorManager();
  if (detectorManager == nullptr) {
    return;
  }
  const auto& cfg = configLoader_->config();

  // the layout is taken from the first entry of the file (the same as the light timing of TPCTreeReader)
  const grams::TPCTreeBuffer& buffer = tpc_tree_reader_->currentBuffer();
  for (auto& detector : detectorManager->getDetectors()) {
    if (!detector->checkType(DetectorType::NanoGRAMS)) {
      continue;
    }
    auto* nanograms = static_cast<RealDetectorUnitNanoGRAMS*>(detector.get());
    std::cout << "[NanoGRAMSReadTPCEvents] light data (detector " << nanograms->getID() << ", "
              << nanograms->NumberOfLightData() << " channels)\n";
    for (int dppChannel = 0; dppChannel < nanograms->NumberOfLightData(); ++dppChannel) {
      const bool registered = buffer.waveRegistered(dppChannel);
      const bool valid = (registered && light_channel_used_[dppChannel]);
      nanograms->getLightData(dppChannel)->setValid(valid);
      std::cout << "  DPP ch " << dppChannel << ": registered=" << registered
                << " used=" << light_channel_used_[dppChannel] << " valid=" << valid;
      if (!valid) {
        std::cout << "\n";
        continue;
      }
      const double timeWidth = static_cast<double>(buffer.wave_compress[dppChannel]) * unit::ns;
      // time = 0 at the trigger
      const double triggerDelay = static_cast<double>(cfg.light_delay_counts[dppChannel]) * 8.0 * timeWidth;
      const int waveformLength = static_cast<int>(buffer.wave_num[dppChannel]);
      nanograms->getLightData(dppChannel)->setLayout(waveformLength, timeWidth, -triggerDelay);
      std::cout << " length=" << waveformLength
                << " time_width=" << timeWidth / unit::ns << " ns"
                << " time_start=" << -triggerDelay / unit::us << " us\n";
    }
  }
}

bool NanoGRAMSReadTPCEvents::readNextTPCEvent(int64_t& raw_event_id)
{
  while (true) {
    if (!tpc_tree_reader_ && !openNextTPCFile()) {
      return false;
    }
    if (tpc_tree_reader_->readNextEntry(raw_event_id)) {
      return true;
    }
    tpc_tree_reader_.reset();
    input_file_.reset();
  }
}

ANLStatus NanoGRAMSReadTPCEvents::mod_analyze()
{
  int64_t raw_event_id = 0;
  if (!readNextTPCEvent(raw_event_id)) {
    std::cout << "[NanoGRAMSReadTPCEvents] AS_QUIT after processing " << processed_entries_ << " / "
              << expected_tpc_entries_ << " tpctree entries.\n";
    return AS_QUIT;
  }

  raw_event_id += current_raw_event_offset_;
  current_raw_event_id_ = raw_event_id;
  current_unix_time_ = tpc_tree_reader_->currentUnixTime();
  ++processed_entries_;

  // events with data inconsistency are not used at all
  if (!grams::isTPCDataUsable(tpc_tree_reader_->currentBuffer().error_flags)) {
    ++skipped_error_events_;
    return AS_SKIP;
  }

  fillEventsIntoDetectors();

  return AS_OK;
}

ANLStatus NanoGRAMSReadTPCEvents::mod_end_run()
{
  std::cout << "Skipped events by TPC error flags: " << skipped_error_events_ << "\n";
  std::cout << "Total processed tpctree entries: " << processed_entries_ << " / " << expected_tpc_entries_ << "\n";

  tpc_tree_reader_.reset();
  input_file_.reset();
  return AS_OK;
}

} /* namespace comptonsoft */
