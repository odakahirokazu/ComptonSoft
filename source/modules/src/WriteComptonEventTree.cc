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

#include "WriteComptonEventTree.hh"
#include "TTree.h"
#include "InitialInformation.hh"
#include "BasicComptonEvent.hh"
#include "ComptonEventTreeIOWithInitialInfo.hh"
#include "EventReconstruction.hh"

using namespace anl;

namespace comptonsoft
{

WriteComptonEventTree::WriteComptonEventTree()
  : cetree_(nullptr),
    eventReconstruction_(nullptr),
    initialInfo_(nullptr),
    treeIO_(new ComptonEventTreeIOWithInitialInfo)
{
}

WriteComptonEventTree::~WriteComptonEventTree() = default;

ANLStatus WriteComptonEventTree::mod_startup()
{
  return AS_OK;
}

ANLStatus WriteComptonEventTree::mod_his()
{
  VCSModule::mod_his();

  cetree_ = new TTree("cetree", "cetree");
  treeIO_->setTree(cetree_);
  treeIO_->defineBranches();

  return AS_OK;
}

ANLStatus WriteComptonEventTree::mod_init()
{
  GetANLModuleNC("EventReconstruction", &eventReconstruction_);

  if (ModuleExist("InitialInformation")) {
    GetANLModuleIF("InitialInformation", &initialInfo_);
    treeIO_->enableInitialInfoRecord();
  }
  else {
    treeIO_->disableInitialInfoRecord();
  }

  return AS_OK;
}

ANLStatus WriteComptonEventTree::mod_ana()
{
  if (initialInfo_) {
    treeIO_->setInitialInfo(initialInfo_->InitialEnergy(),
                                 initialInfo_->InitialDirection(),
                                 initialInfo_->InitialTime(),
                                 initialInfo_->InitialPosition(),
                                 initialInfo_->InitialPolarization());
    treeIO_->setWeight(initialInfo_->Weight());
  }
  
  const BasicComptonEvent& event
    = eventReconstruction_->getComptonEvent();
  treeIO_->fillEvent(event);

  return AS_OK;
}

} /* namespace comptonsoft */
