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

using namespace anl;

namespace comptonsoft
{

ReadComptonEventTree::ReadComptonEventTree()
  : anlgeant4::InitialInformation(false),
    cetree_(nullptr),
    treeIO_(new ComptonEventTreeIOWithInitialInfo)
{
  add_alias("InitialInformation");
}

ANLStatus ReadComptonEventTree::mod_startup()
{
  register_parameter(&fileList_, "file_list", "seq", "compton_events.root");
  
  return AS_OK;
}

ANLStatus ReadComptonEventTree::mod_init()
{
  EvsDef("EventReconstruction:OK");
  EvsDef("EventReconstruction:NG");
  initializeHitPatternData();
  
  return AS_OK;
}

ANLStatus ReadComptonEventTree::mod_his()
{
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

ANLStatus ReadComptonEventTree::mod_ana()
{
  if (entryIndex_ == numEntries_) {
    return AS_QUIT;
  }
  
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

  BasicComptonEvent* event = new BasicComptonEvent;
  treeIO_->retrieveEvent(*event);
  resetComptonEvent(event);
  EvsSet("EventReconstruction:OK");

  ++entryIndex_;
  return AS_OK;
}

} /* namespace comptonsoft */
