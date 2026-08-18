/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                    *
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

#include "NanoGRAMSWriteHitTree.hh"

#include "NanoGRAMSConstants.hh"
#include "NanoGRAMSHitCalibrator.hh"
#include "NanoGRAMSReadTPCEvents.hh"

#include <TFile.h>
#include <TTree.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include "HitTreeIOWithInitialInfo.hh"

using namespace anlnext;

namespace comptonsoft
{

namespace fs = std::filesystem;

NanoGRAMSWriteHitTree::NanoGRAMSWriteHitTree() = default;

NanoGRAMSWriteHitTree::~NanoGRAMSWriteHitTree() = default;

ANLStatus NanoGRAMSWriteHitTree::mod_define()
{
  define_parameter("hittree_file", &mod_class::hittree_file_);
  define_parameter("output_flush_entries", &mod_class::output_flush_entries_);
  return AS_OK;
}

ANLStatus NanoGRAMSWriteHitTree::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  if (!exist_module("NanoGRAMSReadTPCEvents")) {
    throw std::runtime_error(
        "NanoGRAMSWriteHitTree requires NanoGRAMSReadTPCEvents in the same ANL chain.");
  }
  get_module("NanoGRAMSReadTPCEvents", &tpc_events_);

  calibration_config_ = readCalibrationConfig(tpc_events_->configFilePath());

  const fs::path output_path =
      hittree_file_.empty() ? fs::path("hittree.root") : fs::path(hittree_file_);
  if (!output_path.parent_path().empty()) {
    fs::create_directories(output_path.parent_path());
  }

  output_file_ = std::make_unique<TFile>(output_path.string().c_str(), "RECREATE");
  if (!output_file_ || output_file_->IsZombie()) {
    throw std::runtime_error("Failed to create hit tree ROOT file: " + output_path.string());
  }

  hit_tree_ = new TTree(grams::kHitTreeName, grams::kHitTreeName);
  hit_tree_->SetDirectory(output_file_.get());
  const int flush_entries = std::max(1, output_flush_entries_);
  hit_tree_->SetAutoFlush(-flush_entries);
  hit_tree_->SetAutoSave(-flush_entries);

  tree_io_ = std::make_unique<HitTreeIOWithInitialInfo>();
  tree_io_->enableInitialInfoRecord();
  tree_io_->setTree(hit_tree_);
  tree_io_->defineBranches();
  tree_io_->setInitialInfo(0.0,
                           vector3_t(0.0, 0.0, 0.0),
                           0.0,
                           vector3_t(0.0, 0.0, 0.0));
  tree_io_->setInitialPolarization(0.0, 0.0, 0.0);
  tree_io_->setWeight(1.0);

  written_events_ = 0;
  define_evs("NanoGRAMSHitTree:Fill");
  return AS_OK;
}

ANLStatus NanoGRAMSWriteHitTree::mod_analyze()
{
  if (!tpc_events_ || !tpc_events_->hasCurrentEvent()) {
    return AS_OK;
  }

  const auto hits =
      buildCalibratedHits(calibration_config_,
                          tpc_events_->tpcProperty(),
                          tpc_events_->currentEventHits());
  if (hits.empty()) {
    return AS_OK;
  }

  const int32_t run_id = tpc_events_->runId();
  const int32_t event_id = static_cast<int32_t>(tpc_events_->currentEventId());
  tree_io_->fillHits(run_id, event_id, hits);
  ++written_events_;
  if (written_events_ % std::max(1, output_flush_entries_) == 0) {
    flushOutput();
  }
  set_evs("NanoGRAMSHitTree:Fill");
  return AS_OK;
}

void NanoGRAMSWriteHitTree::flushOutput()
{
  output_file_->cd();
  hit_tree_->FlushBaskets();
  hit_tree_->AutoSave();
  output_file_->Flush();
}

ANLStatus NanoGRAMSWriteHitTree::mod_end_run()
{
  if (output_file_ && hit_tree_) {
    output_file_->cd();
    hit_tree_->Write();
    output_file_->Write();
    std::cout << "[ROOT] Saved hit tree: " << output_file_->GetName()
              << " (events=" << written_events_
              << ", entries=" << hit_tree_->GetEntries() << ")\n";
    output_file_->Close();
  }

  tree_io_.reset();
  hit_tree_ = nullptr;
  output_file_.reset();
  tpc_events_ = nullptr;
  return AS_OK;
}

} /* namespace comptonsoft */
