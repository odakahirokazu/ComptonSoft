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

#include "ReadHitTree.hh"
#include "TChain.h"
#include "DetectorHit.hh"
#include "HitTreeIOWithInitialInfo.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace comptonsoft
{

ReadHitTree::ReadHitTree()
  : anlgeant4::InitialInformation(false),
    trustNumHits_(true),
    hitCollection_(nullptr),
    treeIO_(new HitTreeIOWithInitialInfo)
{
  add_alias("InitialInformation");
}

ReadHitTree::~ReadHitTree() = default;

ANLStatus ReadHitTree::mod_define()
{
  register_parameter(&fileList_, "file_list", "seq", "hittree.root");
  register_parameter(&trustNumHits_, "trust_num_hits");
  return AS_OK;
}

ANLStatus ReadHitTree::mod_initialize()
{
  VCSModule::mod_initialize();
  
  get_module_NC("CSHitCollection", &hitCollection_);

  hittree_ = new TChain("hittree");
  for (const std::string& filename: fileList_) {
    hittree_->Add(filename.c_str());
  }

  treeIO_->setTree(hittree_);
  if (hittree_->GetBranch("ini_energy")) {
    setInitialInformationStored();
    setWeightStored();
    treeIO_->enableInitialInfoRecord();
  }
  else {
    treeIO_->disableInitialInfoRecord();
  }
  treeIO_->setBranchAddresses();

  numEntries_ = hittree_->GetEntries();
  std::cout << "Number of entries: " << numEntries_ << std::endl;

  return AS_OK;
}

ANLStatus ReadHitTree::mod_analyze()
{
  if (entryIndex_ == numEntries_) {
    return AS_QUIT;
  }

  hittree_->GetEntry(entryIndex_);

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

  if (trustNumHits_) {
    std::vector<DetectorHit_sptr> hits = treeIO_->retrieveHits(entryIndex_, false);
    for (auto& hit: hits) {
      insertHit(hit);
    }
  }
  else {
    do {
      DetectorHit_sptr hit = treeIO_->retrieveHit();
      insertHit(hit);
      entryIndex_++;
      hittree_->GetEntry(entryIndex_);
    } while (treeIO_->getEventID() == EventID);
  }
  
  return AS_OK;
}

void ReadHitTree::insertHit(const DetectorHit_sptr& hit)
{
  hitCollection_->insertHit(hit);
}

} /* namespace comptonsoft */
