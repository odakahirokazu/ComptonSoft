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

#include "ReadEventTree.hh"
#include "TChain.h"
#include "DetectorHit.hh"
#include "EventTreeIOWithInitialInfo.hh"
#include "CSHitCollection.hh"

using namespace anlnext;

namespace comptonsoft
{

ReadEventTree::ReadEventTree()
  : anlgeant4::InitialInformation(false),
    hit_collection_(nullptr),
    tree_io_(new EventTreeIOWithInitialInfo)
{
  add_alias("InitialInformation");
}

ReadEventTree::~ReadEventTree() = default;

ANLStatus ReadEventTree::mod_define()
{
  define_parameter("file_list", &mod_class::file_list_);
  return AS_OK;
}

ANLStatus ReadEventTree::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module_NC("CSHitCollection", &hit_collection_);

  tree_ = std::make_unique<TChain>("eventtree");
  for (const std::string& filename: file_list_) {
    tree_->Add(filename.c_str());
  }

  tree_io_->set_tree(tree_.get());
  if (tree_->GetBranch("ini_energy")) {
    set_initial_information_stored();
    tree_io_->enableInitialInfoRecord();
  }
  else {
    tree_io_->disableInitialInfoRecord();
  }
  tree_io_->set_branch_addresses();

  num_entries_ = tree_->GetEntries();
  std::cout << "Number of entries: " << num_entries_ << std::endl;

  return AS_OK;
}

ANLStatus ReadEventTree::mod_begin_run()
{
  if (num_entries_ == 0) { return AS_OK; }

  tree_->GetEntry(0);
  const int32_t RunID = tree_io_->getRunID();
  const int32_t EventID = tree_io_->getEventID();
  set_run_id(RunID);
  set_event_id(EventID);

  return AS_OK;
}

ANLStatus ReadEventTree::mod_analyze()
{
  if (entry_index_ == num_entries_) {
    return AS_QUIT;
  }

  tree_->GetEntry(entry_index_);

  const int32_t RunID = tree_io_->getRunID();
  const int32_t EventID = tree_io_->getEventID();
  set_run_id(RunID);
  set_event_id(EventID);

  if (initial_information_stored()) {
    set_initial_energy(tree_io_->getInitialEnergy());
    set_initial_direction(tree_io_->getInitialDirection());
    set_initial_time(tree_io_->getInitialTime());
    set_initial_position(tree_io_->getInitialPosition());
    set_initial_polarization(tree_io_->getInitialPolarization());
    set_weight(tree_io_->getWeight());
  }

  std::vector<DetectorHit_sptr> hits = tree_io_->retrieveHits(entry_index_, false);
  for (auto& hit: hits) {
    insertHit(hit);
  }

  return AS_OK;
}

void ReadEventTree::insertHit(const DetectorHit_sptr& hit)
{
  hit_collection_->insertHit(hit);
}

} /* namespace comptonsoft */
