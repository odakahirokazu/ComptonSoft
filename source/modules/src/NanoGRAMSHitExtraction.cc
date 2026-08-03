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

#include "NanoGRAMSHitExtraction.hh"

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

grams::TPCEventType parseQuickLookEventType(const std::string& name)
{
  if (name == "error") {
    return grams::TPCEventType::Error;
  }
  if (name == "other") {
    return grams::TPCEventType::Other;
  }
  if (name == "gamma") {
    return grams::TPCEventType::Gamma;
  }
  if (name == "cosmic") {
    return grams::TPCEventType::Cosmic;
  }
  if (name == "pileup") {
    return grams::TPCEventType::PileUp;
  }
  if (name == "timeup") {
    return grams::TPCEventType::TimeUp;
  }

  throw std::runtime_error(
      "quicklook_event_types accepts only error, other, gamma, cosmic, pileup, or timeup.");
}

} // namespace

NanoGRAMSHitExtraction::NanoGRAMSHitExtraction() = default;

NanoGRAMSHitExtraction::~NanoGRAMSHitExtraction() = default;

ANLStatus NanoGRAMSHitExtraction::mod_define()
{
  define_parameter("config_file",         &mod_class::config_file_);
  define_parameter("tpctree_file",        &mod_class::tpctree_file_);
  define_parameter("quicklook_file",      &mod_class::quicklook_file_);
  define_parameter("gain_tp_file",        &mod_class::gain_tp_file_);
  define_parameter("gain_tp_hash",        &mod_class::gain_tp_dict_);
  define_parameter("gain_cache_seconds",  &mod_class::gain_cache_seconds_);
  define_parameter("run_id",              &mod_class::run_id_);
  define_parameter("quicklook_event_types", &mod_class::quicklook_event_types_);
  define_parameter("quicklook_num_hits",    &mod_class::quicklook_num_hits_);
  define_parameter("quicklook_save_waveforms", &mod_class::quicklook_save_waveforms_);
  define_map_key("fec", "0");
  add_value_element("gain", &mod_class::gain_tp_value_);
  return AS_OK;
}

ANLStatus NanoGRAMSHitExtraction::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  if (tpctree_file_.empty()) {
    throw std::runtime_error("TPC tree input file path is empty.");
  }

  grams::readConfig(cfg_, config_file_);
  grams::readDPPConfig(cfg_, tpctree_file_);
  setupTPCPropertyForHitSelection();

  input_file_ = std::make_unique<TFile>(tpctree_file_.c_str(), "READ");
  if (input_file_->IsZombie()) {
    throw std::runtime_error("Failed to open input ROOT file: " + tpctree_file_);
  }

  TTree* tpc_tree = dynamic_cast<TTree*>(input_file_->Get(grams::kTpcTreeName));
  if (!tpc_tree) {
    throw std::runtime_error(std::format("Missing TTree '{}' in {}",
                                         grams::kTpcTreeName,
                                         tpctree_file_));
  }
  expected_tpc_entries_ = static_cast<int64_t>(tpc_tree->GetEntries());
  std::cout << "[NanoGRAMSHitExtraction] input file: " << tpctree_file_ << "\n"
            << "[NanoGRAMSHitExtraction] tpctree entries: "
            << expected_tpc_entries_ << "\n";

  tpc_tree_reader_ = std::make_unique<grams::TPCTreeReader>(
      tpc_tree,
      cfg_,
      tpc_property_);
  const int64_t reader_entries = tpc_tree_reader_->currentBuffer().nEntries();
  std::cout << "[NanoGRAMSHitExtraction] TPCTreeReader entries: "
            << reader_entries << "\n";
  if (reader_entries != expected_tpc_entries_) {
    std::cout << "[NanoGRAMSHitExtraction] WARNING: TTree entries and "
              << "TPCTreeReader entries differ.\n";
  }
  if (!quicklook_file_.empty()) {
    quicklook_tree_writer_ = std::make_unique<grams::QuickLookTreeOutputWriter>(
        quicklook_file_,
        tpc_tree_reader_->currentBuffer(),
        tpc_property_,
        quicklook_save_waveforms_);
  } else {
    std::cout << "[INFO] tpcquicklook output is disabled.\n";
  }

  gamma_events_         = 0;
  processed_entries_    = 0;
  current_raw_event_id_ = -1;
  current_event_hits_.clear();

  return AS_OK;
}

void NanoGRAMSHitExtraction::setupTPCPropertyForHitSelection()
{
  calibration_config_ = readCalibrationConfig(config_file_);

  if (!gain_tp_file_.empty()) {
    const std::filesystem::path gain_tp_path =
        resolveCalibrationPath(calibration_config_.config_dir, gain_tp_file_);
    gain_tp_table_ = readTestPulseGainTable(gain_tp_path);
    use_event_time_gain_ = true;
    std::cout << "[NanoGRAMSHitExtraction] gain_tp_file for hit selection: "
              << gain_tp_path << std::endl;
  } else if (!gain_tp_dict_.empty()) {
    calibration_config_.energy.tp_adc_values =
        fixedTestPulseGainsFromHash(gain_tp_dict_);
    std::cout << "[NanoGRAMSHitExtraction] gain_tp_hash for hit selection."
              << std::endl;
  } else {
    std::cout << "[NanoGRAMSHitExtraction] WARNING: no gain_tp_file/hash for "
              << "keV hit selection. Temperature correction factors are 1."
              << std::endl;
  }

  const std::filesystem::path gain_info_path =
      resolveCalibrationPath(calibration_config_.config_dir,
                             calibration_config_.energy.gain_info_file);
  const std::filesystem::path spline_path =
      resolveCalibrationPath(calibration_config_.config_dir,
                             calibration_config_.energy.q_to_kev_spline_file);

  tpc_property_.loadParamCoulomb2keVForSpline3D(spline_path,
                                                calibration_config_.general.efield);
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

void NanoGRAMSHitExtraction::updateGainCorrectionForCurrentEvent(uint32_t unix_time)
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

bool NanoGRAMSHitExtraction::shouldWriteQuickLook(
    grams::TPCEventType event_type,
    const std::vector<grams::RawFECHit>& event_hits) const
{
  if (!quicklook_event_types_.empty()) {
    bool event_type_ok = false;
    for (const std::string& name : quicklook_event_types_) {
      if (event_type == parseQuickLookEventType(name)) {
        event_type_ok = true;
        break;
      }
    }
    if (!event_type_ok) {
      return false;
    }
  }

  if (quicklook_num_hits_ >= 0 &&
      static_cast<int>(event_hits.size()) != quicklook_num_hits_) {
    return false;
  }

  return true;
}

ANLStatus NanoGRAMSHitExtraction::mod_analyze()
{
  int64_t raw_event_id = 0;
  std::vector<grams::RawFECHit> event_hits;
  if (!tpc_tree_reader_ || !tpc_tree_reader_->readNextEntry(raw_event_id)) {
    std::cout << "[NanoGRAMSHitExtraction] AS_QUIT after processing "
              << processed_entries_ << " / "
              << expected_tpc_entries_ << " tpctree entries.\n";
    return AS_QUIT;
  }

  current_raw_event_id_ = raw_event_id;
  current_unix_time_ = tpc_tree_reader_->currentUnixTime();
  updateGainCorrectionForCurrentEvent(current_unix_time_);
  tpc_tree_reader_->extractCurrentEventHits(event_hits);
  ++processed_entries_;
  current_event_hits_.clear();

  const grams::TPCEventType event_type = tpc_tree_reader_->currentEventType();
  if (quicklook_tree_writer_ &&
      shouldWriteQuickLook(event_type, event_hits)) {
    quicklook_tree_writer_->fillEvent(raw_event_id,
                                      event_type,
                                      tpc_tree_reader_->currentBuffer(),
                                      event_hits);
  }

  if (!event_hits.empty()) {
    current_event_hits_ = event_hits;
    ++gamma_events_;
  }

  return AS_OK;
}

ANLStatus NanoGRAMSHitExtraction::mod_end_run()
{

  std::cout << "Total gamma events: " << gamma_events_ << "\n";
  std::cout << "Total processed tpctree entries: " << processed_entries_
            << " / " << expected_tpc_entries_ << "\n";

  if (quicklook_tree_writer_) {
    quicklook_tree_writer_->close();
    quicklook_tree_writer_.reset();
  }

  tpc_tree_reader_.reset();
  input_file_.reset();
  current_event_hits_.clear();
  return AS_OK;
}

} /* namespace comptonsoft */
