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

#include "WriteHitTree.hh"
#include "TTree.h"
#include "InitialInformation.hh"
#include "DetectorHit.hh"
#include "HitTreeIOWithInitialInfo.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace comptonsoft
{

WriteHitTree::WriteHitTree()
  : hitCollection_(nullptr),
    initialInfo_(nullptr),
    hittree_(nullptr),
    treeIO_(new HitTreeIOWithInitialInfo)
{
}

ANLStatus WriteHitTree::mod_his()
{
  VCSModule::mod_his();
  hittree_ = new TTree("hittree", "hittree");
  treeIO_->setTree(hittree_);
  treeIO_->defineBranches();

  return AS_OK;
}

ANLStatus WriteHitTree::mod_init()
{
  VCSModule::mod_init();

  GetANLModule("CSHitCollection", &hitCollection_);
  EvsDef("WriteHitTree:Fill");

  if (ModuleExist("InitialInformation")) {
    GetANLModuleIF("InitialInformation", &initialInfo_);
    treeIO_->enableInitialInfoRecord();
  }
  else {
    treeIO_->disableInitialInfoRecord();
  }
  
  return AS_OK;
}

ANLStatus WriteHitTree::mod_ana()
{
  const int64_t eventID = hitCollection_->EventID();

  if (initialInfo_) {
    treeIO_->setInitialInfo(initialInfo_->InitialEnergy(),
                            initialInfo_->InitialDirection(),
                            initialInfo_->InitialTime(),
                            initialInfo_->InitialPosition(),
                            initialInfo_->InitialPolarization());
    treeIO_->setWeight(initialInfo_->Weight());
  }
  
  const int NumTimeGroups = hitCollection_->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    const std::vector<DetectorHit_sptr>& hits
      = hitCollection_->getHits(timeGroup);
    if (hits.size() > 0) {
      treeIO_->fillHits(eventID, hits);
      EvsSet("WriteHitTree:Fill");
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
