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

using namespace anl;

namespace comptonsoft
{

ReadEventTree::ReadEventTree()
  : anlgeant4::InitialInformation(false),
    hitCollection_(nullptr),
    treeIO_(new EventTreeIOWithInitialInfo)
{
  add_alias("InitialInformation");
}

ReadEventTree::~ReadEventTree() = default;

ANLStatus ReadEventTree::mod_define()
{
  register_parameter(&fileList_, "file_list");
  return AS_OK;
}

ANLStatus ReadEventTree::mod_initialize()
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

ANLStatus ReadEventTree::mod_analyze()
{
  if (entryIndex_ == numEntries_) {
    return AS_QUIT;
  }

  tree_->GetEntry(entryIndex_);

  const int64_t EventID = treeIO_->getEventID();
  setEventID(EventID);

  if (InitialInformationStored()) {
    setInitialEnergy(treeIO_->getInitialEnergy());
    setInitialDirection(treeIO_->getInitialDirection());
    setInitialTime(treeIO_->getInitialTime());
    setInitialPosition(treeIO_->getInitialPosition());
    setInitialPolarization(treeIO_->getInitialPolarization());
  }
  
  if (WeightStored()) {
    setWeight(treeIO_->getWeight());
  }

  std::vector<DetectorHit_sptr> hits = treeIO_->retrieveHits(entryIndex_, false);
  for (auto& hit: hits) {
    insertHit(hit);
  }
  
  return AS_OK;
}

void ReadEventTree::insertHit(const DetectorHit_sptr& hit)
{
  hitCollection_->insertHit(hit);
}

} /* namespace comptonsoft */
