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
    hitCollection_(nullptr),
    treeIO_(new EventTreeIOWithInitialInfo)
{
  add_alias("InitialInformation");
}

SortEventTreeWithTime::~SortEventTreeWithTime() = default;

ANLStatus SortEventTreeWithTime::mod_define()
{
  register_parameter(&fileList_, "file_list");
  return AS_OK;
}

ANLStatus SortEventTreeWithTime::mod_initialize()
{
  VCSModule::mod_initialize();
  
  get_module_NC("CSHitCollection", &hitCollection_);

  tree_ = new TChain("eventtree");
  for (const std::string& filename: fileList_) {
    tree_->Add(filename.c_str());
  }

  treeIO_->setTree(tree_);
  if (tree_->GetBranch("ini_energy")) {
    setInitialInformationStored();
    treeIO_->enableInitialInfoRecord();
  }
  else {
    treeIO_->disableInitialInfoRecord();
  }
  treeIO_->setBranchAddresses();

  numEntries_ = tree_->GetEntries();
  std::cout << "Number of entries: " << numEntries_ << std::endl;

  return AS_OK;
}

ANLStatus SortEventTreeWithTime::mod_begin_run()
{
  if (numEntries_ == 0) { return AS_OK; }

  while (entryIndex_ < numEntries_) {
    tree_->GetEntry(entryIndex_);
    eventList_.push_back( treeIO_->retrieveHits(entryIndex_, false) );
  }

  eventList_.sort([](std::vector<DetectorHit_sptr>& a, std::vector<DetectorHit_sptr>& b) {
                    return a[0]->RealTime() < b[0]->RealTime();
                  });
  eventIter_ = eventList_.begin();

  return AS_OK;
}

ANLStatus SortEventTreeWithTime::mod_analyze()
{
  if (eventIter_ == eventList_.end()) {
    return AS_QUIT;
  }

  std::vector<DetectorHit_sptr>& hits = *eventIter_;
  for (auto& hit: hits){
    insertHit(hit);
  }

  ++eventIter_;

  return AS_OK;
}

void SortEventTreeWithTime::insertHit(const DetectorHit_sptr& hit)
{
  hitCollection_->insertHit(hit);
}

} /* namespace comptonsoft */
