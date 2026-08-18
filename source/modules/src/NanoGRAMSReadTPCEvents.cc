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

#include <cmath>
#include <filesystem>
#include <format>
#include <iostream>
#include <stdexcept>

using namespace anlnext;

namespace comptonsoft
{

namespace
{

int64_t gainTimeBin(double unix_time, double cache_seconds)
{
  if (cache_seconds > 0.0) {
    return static_cast<int64_t>(std::floor(unix_time / cache_seconds));
  }
  return static_cast<int64_t>(unix_time);
}

} // namespace

NanoGRAMSReadTPCEvents::NanoGRAMSReadTPCEvents() = default;

NanoGRAMSReadTPCEvents::~NanoGRAMSReadTPCEvents() = default;

ANLStatus NanoGRAMSReadTPCEvents::mod_define()
{
  define_parameter("config_file",         &mod_class::config_file_);
  define_parameter("dpp_config_file",     &mod_class::dpp_config_file_);
  define_parameter("tpctree_files",       &mod_class::tpctree_files_);
  define_parameter("gain_tp_file",        &mod_class::gain_tp_file_);
  define_parameter("gain_tp_hash",        &mod_class::gain_tp_dict_);
  define_parameter("gain_cache_seconds",  &mod_class::gain_cache_seconds_);
  define_parameter("run_id",              &mod_class::run_id_);
  define_map_key("fec", "0");
  add_value_element("gain", &mod_class::gain_tp_value_);
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

  gamma_events_         = 0;
  processed_entries_    = 0;
  expected_tpc_entries_ = 0;
  input_file_index_     = 0;
  current_raw_event_offset_ = 0;
  current_raw_event_id_ = -1;
  current_event_hits_.clear();
  openNextTPCFile();

  return AS_OK;
}

bool NanoGRAMSReadTPCEvents::openNextTPCFile()
{
  tpc_tree_reader_.reset();
  input_file_.reset();

  while (input_file_index_ < tpctree_files_.size()) {
    const std::string input_path = tpctree_files_[input_file_index_++];
    if (input_path.empty()) {
      continue;
    }

    grams::readConfig(cfg_, config_file_);
    if (dpp_config_file_.empty()) {
      grams::readDPPConfig(cfg_, input_path);
    } else {
      grams::readDPPConfigFile(cfg_, dpp_config_file_);
    }
    setupTPCPropertyForHitSelection();

    auto input_file = std::make_unique<TFile>(input_path.c_str(), "READ");
    if (input_file->IsZombie()) {
      throw std::runtime_error("Failed to open input ROOT file: " + input_path);
    }
    TTree* tpc_tree = dynamic_cast<TTree*>(input_file->Get(grams::kTpcTreeName));
    if (!tpc_tree) {
      throw std::runtime_error(std::format("Missing TTree '{}' in {}",
                                           grams::kTpcTreeName,
                                           input_path));
    }

    const int64_t entries = static_cast<int64_t>(tpc_tree->GetEntries());
    if (entries == 0) {
      std::cout << "[NanoGRAMSReadTPCEvents] skip empty tpctree: "
                << input_path << "\n";
      continue;
    }
    input_file_ = std::move(input_file);
    tpc_tree_reader_ = std::make_unique<grams::TPCTreeReader>(
        tpc_tree,
        cfg_,
        tpc_property_);
    current_raw_event_offset_ = processed_entries_;
    expected_tpc_entries_ += entries;
    std::cout << "[NanoGRAMSReadTPCEvents] input file: " << input_path << "\n"
              << "[NanoGRAMSReadTPCEvents] tpctree entries: " << entries << "\n";

    return true;
  }
  return false;
}

void NanoGRAMSReadTPCEvents::setupTPCPropertyForHitSelection()
{
  calibration_config_ = readCalibrationConfig(config_file_);

  if (!gain_tp_file_.empty()) {
    const std::filesystem::path gain_tp_path =
        resolveCalibrationPath(calibration_config_.config_dir, gain_tp_file_);
    gain_tp_table_ = readTestPulseGainTable(gain_tp_path);
    use_event_time_gain_ = true;
    std::cout << "[NanoGRAMSReadTPCEvents] gain_tp_file for hit selection: "
              << gain_tp_path << std::endl;
  } else if (!gain_tp_dict_.empty()) {
    calibration_config_.energy.tp_adc_values =
        fixedTestPulseGainsFromHash(gain_tp_dict_);
    std::cout << "[NanoGRAMSReadTPCEvents] gain_tp_hash for hit selection."
              << std::endl;
  } else {
    std::cout << "[NanoGRAMSReadTPCEvents] WARNING: no gain_tp_file/hash for "
              << "keV hit selection. Temperature correction factors are 1."
              << std::endl;
  }

  const std::filesystem::path gain_info_path =
      resolveCalibrationPath(calibration_config_.config_dir,
                             calibration_config_.energy.gain_info_file);
  tpc_property_.setChargeToEnergySpline(calibration_config_.general.efield);
  tpc_property_.loadParamGainMatrices(gain_info_path);
  tpc_property_.setDriftVelocity(electronDriftVelocity(
      calibration_config_.general.temperature,
      calibration_config_.general.efield));
  tpc_property_.setAnodePosZ(calibration_config_.position.anode_pos_z);

  if (!use_event_time_gain_ && !gain_tp_dict_.empty()) {
    tpc_property_.applyTemperatureCorrection(
        calibration_config_.energy.tp_channel,
        calibration_config_.energy.ccal,
        calibration_config_.energy.tp_adc_values);
  }
}

void NanoGRAMSReadTPCEvents::updateGainCorrectionForCurrentEvent(uint32_t unix_time)
{
  if (!use_event_time_gain_) {
    return;
  }

  if (unix_time == 0) {
    throw std::runtime_error(
        "TPC tree unixtime is zero; cannot apply event-time gain correction "
        "for keV hit selection.");
  }

  const int64_t time_bin = gainTimeBin(
      static_cast<double>(unix_time),
      gain_cache_seconds_);
  if (time_bin == cached_gain_time_bin_) {
    return;
  }

  calibration_config_.energy.tp_adc_values =
      interpolatedTestPulseGains(gain_tp_table_, static_cast<double>(unix_time));
  tpc_property_.applyTemperatureCorrection(
      calibration_config_.energy.tp_channel,
      calibration_config_.energy.ccal,
      calibration_config_.energy.tp_adc_values);
  cached_gain_time_bin_ = time_bin;
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
    std::cout << "[NanoGRAMSReadTPCEvents] AS_QUIT after processing "
              << processed_entries_ << " / "
              << expected_tpc_entries_ << " tpctree entries.\n";
    return AS_QUIT;
  }

  std::vector<grams::RawFECHit> event_hits;
  raw_event_id          += current_raw_event_offset_;
  current_raw_event_id_ = raw_event_id;
  current_unix_time_    = tpc_tree_reader_->currentUnixTime();
  updateGainCorrectionForCurrentEvent(current_unix_time_);
  tpc_tree_reader_->extractCurrentEventHits(event_hits);
  ++processed_entries_;
  current_event_hits_.clear();

  if (!event_hits.empty()) {
    current_event_hits_ = event_hits;
    ++gamma_events_;
  }

  return AS_OK;
}

ANLStatus NanoGRAMSReadTPCEvents::mod_end_run()
{

  std::cout << "Total gamma events: " << gamma_events_ << "\n";
  std::cout << "Total processed tpctree entries: " << processed_entries_
            << " / " << expected_tpc_entries_ << "\n";

  tpc_tree_reader_.reset();
  input_file_.reset();
  current_event_hits_.clear();
  return AS_OK;
}

} /* namespace comptonsoft */
