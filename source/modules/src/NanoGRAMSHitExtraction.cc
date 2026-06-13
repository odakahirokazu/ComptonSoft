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

  grams::readConfig(cfg_, config_file_);

  if (tpctree_file_.empty()) {
    throw std::runtime_error("TPC tree input file path is empty.");
  }

  input_file_ = std::make_unique<TFile>(tpctree_file_.c_str(), "READ");
  if (input_file_->IsZombie()) {
    throw std::runtime_error("Failed to open input ROOT file: " + tpctree_file_);
  }

  TTree* tpc_tree = dynamic_cast<TTree*>(input_file_->Get(grams::kTpcTreeName));
  if (!tpc_tree) {
    throw std::runtime_error("Missing TTree '" + std::string(grams::kTpcTreeName) +
                             "' in " + tpctree_file_);
  }

  tpc_tree_reader_ = std::make_unique<grams::TPCTreeReader>(tpc_tree, cfg_);
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
  current_raw_event_id_ = -1;
  current_event_hits_.clear();

  return AS_OK;
}

ANLStatus NanoGRAMSHitExtraction::mod_analyze()
{
  int64_t raw_event_id = 0;
  std::vector<grams::RawFECHit> event_hits;
  if (!tpc_tree_reader_ || !tpc_tree_reader_->processNext(raw_event_id, event_hits)) {
    return AS_QUIT;
  }

  current_raw_event_id_ = raw_event_id;
  current_event_hits_.clear();

  if (quicklook_tree_writer_) {
    quicklook_tree_writer_->fillEvent(raw_event_id,
                                      tpc_tree_reader_->currentEventType(),
                                      tpc_tree_reader_->currentBuffer());
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
