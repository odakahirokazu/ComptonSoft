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

#include "SortEventTreeWithTime.hh"
#include "TChain.h"
#include "DetectorHit.hh"
#include "EventTreeIOWithInitialInfo.hh"
#include "CSHitCollection.hh"

#include <vector>

using namespace anlnext;

namespace comptonsoft
{

SortEventTreeWithTime::SortEventTreeWithTime()
  : anlgeant4::InitialInformation(true),
    hit_collection_(nullptr),
    tree_io_(new EventTreeIOWithInitialInfo)
{
  add_alias("InitialInformation");
}

SortEventTreeWithTime::~SortEventTreeWithTime() = default;

ANLStatus SortEventTreeWithTime::mod_define()
{
  define_parameter("file_list", &mod_class::file_list_);
  return AS_OK;
}

ANLStatus SortEventTreeWithTime::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module_NC("CSHitCollection", &hit_collection_);

  tree_ = new TChain("eventtree");
  for (const std::string& filename: file_list_) {
    tree_->Add(filename.c_str());
  }

  tree_io_->set_tree(tree_);
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

ANLStatus SortEventTreeWithTime::mod_begin_run()
{
  if (num_entries_ == 0) { return AS_OK; }

  while (entry_index_ < num_entries_) {
    tree_->GetEntry(entry_index_);
    event_list_.push_back( tree_io_->retrieveHits(entry_index_, false) );
  }

  event_list_.sort([](std::vector<DetectorHit_sptr>& a, std::vector<DetectorHit_sptr>& b) {
                    return a[0]->RealTime() < b[0]->RealTime();
                  });
  event_iter_ = event_list_.begin();

  return AS_OK;
}

ANLStatus SortEventTreeWithTime::mod_analyze()
{
  if (event_iter_ == event_list_.end()) {
    return AS_QUIT;
  }

  std::vector<DetectorHit_sptr>& hits = *event_iter_;
  for (auto& hit: hits){
    insertHit(hit);
  }

  ++event_iter_;

  return AS_OK;
}

void SortEventTreeWithTime::insertHit(const DetectorHit_sptr& hit)
{
  hit_collection_->insertHit(hit);
}

} /* namespace comptonsoft */
