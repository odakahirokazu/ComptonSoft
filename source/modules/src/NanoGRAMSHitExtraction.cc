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

#include <format>
#include <iostream>
#include <stdexcept>

using namespace anlnext;

namespace comptonsoft
{

NanoGRAMSHitExtraction::NanoGRAMSHitExtraction() = default;

NanoGRAMSHitExtraction::~NanoGRAMSHitExtraction() = default;

ANLStatus NanoGRAMSHitExtraction::mod_define()
{
  define_parameter("config_file",         &mod_class::config_file_);
  define_parameter("tpctree_file",        &mod_class::tpctree_file_);
  define_parameter("rawhittree_file",     &mod_class::rawhittree_file_);
  define_parameter("quicklook_file",      &mod_class::quicklook_file_);
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

  tpc_tree_reader_ = std::make_unique<grams::TPCTreeReader>(tpc_tree, cfg_);
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
        tpc_tree_reader_->currentBuffer().layout());
  } else {
    std::cout << "[INFO] tpcquicklook output is disabled.\n";
  }

  if (!rawhittree_file_.empty()) {
    rawhit_tree_writer_ = std::make_unique<grams::RawHitTreeOutputWriter>(rawhittree_file_);
  } else {
    std::cout << "[INFO] rawhittree output is disabled.\n";
  }
  gamma_events_         = 0;
  processed_entries_    = 0;
  current_raw_event_id_ = -1;
  current_event_hits_.clear();

  return AS_OK;
}

ANLStatus NanoGRAMSHitExtraction::mod_analyze()
{
  int64_t raw_event_id = 0;
  std::vector<grams::RawFECHit> event_hits;
  if (!tpc_tree_reader_ || !tpc_tree_reader_->processNext(raw_event_id, event_hits)) {
    std::cout << "[NanoGRAMSHitExtraction] AS_QUIT after processing "
              << processed_entries_ << " / "
              << expected_tpc_entries_ << " tpctree entries.\n";
    return AS_QUIT;
  }

  current_raw_event_id_ = raw_event_id;
  current_unix_time_ = tpc_tree_reader_->currentUnixTime();
  ++processed_entries_;
  current_event_hits_.clear();

  if (quicklook_tree_writer_) {
    quicklook_tree_writer_->fillEvent(raw_event_id,
                                      tpc_tree_reader_->currentEventType(),
                                      tpc_tree_reader_->currentBuffer(),
                                      event_hits);
  }

  if (!event_hits.empty()) {
    current_event_hits_ = event_hits;
    if (rawhit_tree_writer_) {
      rawhit_tree_writer_->fillEvent(gamma_events_, raw_event_id, current_event_hits_);
    }
    ++gamma_events_;
  }

  return AS_OK;
}

ANLStatus NanoGRAMSHitExtraction::mod_end_run()
{

  std::cout << "Total gamma events: " << gamma_events_ << "\n";
  std::cout << "Total processed tpctree entries: " << processed_entries_
            << " / " << expected_tpc_entries_ << "\n";

  if (rawhit_tree_writer_) {
    rawhit_tree_writer_->close();
    rawhit_tree_writer_.reset();
  }
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
