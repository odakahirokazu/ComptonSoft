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

using namespace anlnext;

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

ANLStatus WriteComptonEventTree::mod_define()
{
  return AS_OK;
}

ANLStatus WriteComptonEventTree::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module_NC("EventReconstruction", &eventReconstruction_);

  if (exist_module("InitialInformation")) {
    get_module_IF("InitialInformation", &initialInfo_);
    treeIO_->enableInitialInfoRecord();
  }
  else {
    treeIO_->disableInitialInfoRecord();
  }

  cetree_ = new TTree("cetree", "cetree");
  treeIO_->setTree(cetree_);
  treeIO_->defineBranches();

  return AS_OK;
}

ANLStatus WriteComptonEventTree::mod_analyze()
{
  int32_t runID = -1;
  int32_t eventID = -1;

  if (initialInfo_) {
    runID = initialInfo_->run_id();
    eventID = initialInfo_->event_id();
    treeIO_->setInitialInfo(initialInfo_->initial_energy(),
                            initialInfo_->initial_direction(),
                            initialInfo_->initial_time(),
                            initialInfo_->initial_position(),
                            initialInfo_->initial_polarization());
    treeIO_->setWeight(initialInfo_->weight());
  }
  else {
    runID = 0;
    eventID = get_loop_index();
  }

  treeIO_->fillEvents(runID, eventID, eventReconstruction_->getReconstructedEvents());

  return AS_OK;
}

} /* namespace comptonsoft */
