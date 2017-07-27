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

#include "WriteEventTree.hh"
#include "TTree.h"
#include "InitialInformation.hh"
#include "DetectorHit.hh"
#include "EventTreeIOWithInitialInfo.hh"
#include "CSHitCollection.hh"

using namespace anlnext;

namespace comptonsoft
{

WriteEventTree::WriteEventTree()
  : hitCollection_(nullptr),
    initialInfo_(nullptr),
    tree_(nullptr),
    treeIO_(new EventTreeIOWithInitialInfo)
{
}

ANLStatus WriteEventTree::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module("CSHitCollection", &hitCollection_);
  define_evs("WriteEventTree:Fill");

  if (exist_module("InitialInformation")) {
    get_module_IF("InitialInformation", &initialInfo_);
    treeIO_->enableInitialInfoRecord();
  }
  else {
    treeIO_->disableInitialInfoRecord();
  }
  
  tree_ = new TTree("eventtree", "eventtree");
  treeIO_->setTree(tree_);
  treeIO_->defineBranches();

  return AS_OK;
}

ANLStatus WriteEventTree::mod_analyze()
{
  int64_t eventID = -1;
  
  if (initialInfo_) {
    eventID = initialInfo_->EventID();
    treeIO_->setInitialInfo(initialInfo_->InitialEnergy(),
                            initialInfo_->InitialDirection(),
                            initialInfo_->InitialTime(),
                            initialInfo_->InitialPosition(),
                            initialInfo_->InitialPolarization());
    treeIO_->setWeight(initialInfo_->Weight());
  }
  else {
    eventID = get_loop_index();
  }

  const int NumTimeGroups = hitCollection_->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    const std::vector<DetectorHit_sptr>& hits
      = hitCollection_->getHits(timeGroup);
    if (hits.size() > 0) {
      treeIO_->fillHits(eventID, hits);
      set_evs("WriteEventTree:Fill");
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
