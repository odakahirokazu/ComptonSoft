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

#include "ReadComptonEventTree.hh"
#include "TChain.h"
#include "BasicComptonEvent.hh"
#include "VEventReconstructionAlgorithm.hh"
#include "ComptonEventTreeIOWithInitialInfo.hh"

using namespace anlnext;

namespace comptonsoft
{

ReadComptonEventTree::ReadComptonEventTree()
  : anlgeant4::InitialInformation(false),
    cetree_(nullptr),
    treeIO_(new ComptonEventTreeIOWithInitialInfo)
{
  add_alias("InitialInformation");
}

ANLStatus ReadComptonEventTree::mod_define()
{
  register_parameter(&fileList_, "file_list");
  
  return AS_OK;
}

ANLStatus ReadComptonEventTree::mod_initialize()
{
  VCSModule::mod_initialize();

  define_evs("EventReconstruction:OK");
  define_evs("EventReconstruction:NG");
  initializeHitPatternData();
  
  cetree_ = new TChain("cetree");
  for (const std::string& filename: fileList_) {
    cetree_->Add(filename.c_str());
  }

  treeIO_->setTree(cetree_);
  if (cetree_->GetBranch("ini_energy")) {
    setInitialInformationStored();
    treeIO_->enableInitialInfoRecord();
  }
  else {
    treeIO_->disableInitialInfoRecord();
  }
  treeIO_->setBranchAddresses();

  numEntries_ = cetree_->GetEntries();
  std::cout << "Number of entries: " << numEntries_ << std::endl;

  return AS_OK;
}

ANLStatus ReadComptonEventTree::mod_analyze()
{
  if (entryIndex_ == numEntries_) {
    return AS_QUIT;
  }

  initializeEvent();

  cetree_->GetEntry(entryIndex_);
    
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

  do {
    auto event = std::make_shared<BasicComptonEvent>();
    treeIO_->retrieveEvent(*event);
    pushReconstructedEvent(event);
    retrieveHitPatterns();
    set_evs("EventReconstruction:OK");
    ++entryIndex_;

    if (entryIndex_ == numEntries_) {
      return AS_QUIT;
    }
    cetree_->GetEntry(entryIndex_);
  } while (treeIO_->getEventID() == EventID);

  return AS_OK;
}

} /* namespace comptonsoft */
