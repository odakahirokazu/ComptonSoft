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

#include "NanoGRAMSDataReduction.hh"

#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <stdexcept>

using namespace anlnext;

namespace comptonsoft
{

NanoGRAMSDataReduction::NanoGRAMSDataReduction() = default;

NanoGRAMSDataReduction::~NanoGRAMSDataReduction() = default;

ANLStatus NanoGRAMSDataReduction::mod_define()
{
  define_parameter("config_file",     &mod_class::config_file_);
  define_parameter("tpctree_file",    &mod_class::tpctree_file_);
  define_parameter("rawhitdata_file", &mod_class::rawhitdata_file_);
  return AS_OK;
}

ANLStatus NanoGRAMSDataReduction::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  ngUtil::readConfig(cfg_, config_file_);

  if (tpctree_file_.empty()) {
    throw std::runtime_error("TPC tree input file path is empty.");
  }

  input_file_ = std::make_unique<TFile>(tpctree_file_.c_str(), "READ");
  if (input_file_->IsZombie()) {
    throw std::runtime_error("Failed to open input ROOT file: " + tpctree_file_);
  }

  TTree* tpc_tree = dynamic_cast<TTree*>(input_file_->Get(ngUtil::kTpcTreeName));
  if (!tpc_tree) {
    throw std::runtime_error("Missing TTree '" + std::string(ngUtil::kTpcTreeName) +
                             "' in " + tpctree_file_);
  }

  raw_hit_reader_ = std::make_unique<ngUtil::TPCTreeRawHitReader>(tpc_tree, cfg_);
  if (!rawhitdata_file_.empty()) {
    writer_ = std::make_unique<ngUtil::RawHitTreeOutputWriter>(rawhitdata_file_);
  } else {
    std::cout << "[INFO] rawhittree output is disabled.\n";
  }
  gamma_events_         = 0;
  current_has_event_    = false;
  current_event_id_     = -1;
  current_raw_event_id_ = -1;
  current_event_hits_.clear();

  return AS_OK;
}

ANLStatus NanoGRAMSDataReduction::mod_analyze()
{
  int64_t raw_event_id = 0;
  std::vector<ngUtil::RawFECHit> event_hits;
  if (!raw_hit_reader_ || !raw_hit_reader_->processNext(raw_event_id, event_hits)) {
    return AS_QUIT;
  }

  current_has_event_    = false;
  current_raw_event_id_ = raw_event_id;
  current_event_hits_.clear();

  if (!event_hits.empty()) {
    current_has_event_  = true;
    current_event_id_   = gamma_events_;
    current_event_hits_ = event_hits;
    if (writer_) {
      writer_->fillEvent(current_event_id_, raw_event_id, current_event_hits_);
    }
    ++gamma_events_;
  }

  return AS_OK;
}

ANLStatus NanoGRAMSDataReduction::mod_end_run()
{
  if (writer_) {
    std::cout << "Total gamma events: " << gamma_events_ << "\n";
    writer_->close();
    writer_.reset();
  } else {
    std::cout << "Total gamma events: " << gamma_events_ << "\n";
  }

  raw_hit_reader_.reset();
  input_file_.reset();
  current_has_event_ = false;
  current_event_hits_.clear();
  return AS_OK;
}

} /* namespace comptonsoft */
