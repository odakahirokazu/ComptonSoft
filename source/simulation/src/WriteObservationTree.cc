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

#include "WriteObservationTree.hh"
#include "TTree.h"
#include "InitialInformation.hh"
#include "ObservedParticle.hh"
#include "ObservationTreeIOWithInitialInfo.hh"
#include "ObservationPickUpData.hh"

using namespace anlnext;

namespace comptonsoft
{

WriteObservationTree::WriteObservationTree()
  : observationPUD_(nullptr),
    initialInfo_(nullptr),
    tree_(nullptr),
    treeIO_(new ObservationTreeIOWithInitialInfo)
{
}

ANLStatus WriteObservationTree::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module("ObservationPickUpData", &observationPUD_);
  define_evs("WriteObservationTree:Fill");

  if (exist_module("InitialInformation")) {
    get_module_IF("InitialInformation", &initialInfo_);
    treeIO_->enableInitialInfoRecord();
  }
  else {
    treeIO_->disableInitialInfoRecord();
  }

  tree_ = new TTree("otree", "Observation tree");
  treeIO_->setTree(tree_);
  treeIO_->defineBranches();

  return AS_OK;
}

ANLStatus WriteObservationTree::mod_analyze()
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
  
  const std::vector<ObservedParticle_sptr>& particles
    = observationPUD_->getParticleVector();
  if (particles.size() > 0) {
    treeIO_->fillParticles(eventID, particles);
    set_evs("WriteObservationTree:Fill");
  }

  return AS_OK;
}

} /* namespace comptonsoft */
