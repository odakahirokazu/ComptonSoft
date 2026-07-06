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
#include "ObservationEventStore.hh"

using namespace anlnext;

namespace comptonsoft
{

WriteObservationTree::WriteObservationTree()
  : tree_io_(std::make_unique<ObservationTreeIOWithInitialInfo>())
{
}

ANLStatus WriteObservationTree::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module("ObservationEventStore", &event_store_);
  define_evs("WriteObservationTree:Fill");

  if (exist_module("InitialInformation")) {
    get_module_IF("InitialInformation", &initial_info_);
    tree_io_->enableInitialInfoRecord();
  }
  else {
    tree_io_->disableInitialInfoRecord();
  }

  tree_ = new TTree("otree", "Observation tree");
  tree_io_->set_tree(tree_);
  tree_io_->define_branches();

  return AS_OK;
}

ANLStatus WriteObservationTree::mod_analyze()
{
  int run_id = -1;
  int event_id = -1;

  if (initial_info_) {
    run_id = initial_info_->run_id();
    event_id = initial_info_->event_id();
    tree_io_->setInitialInfo(initial_info_->initial_energy(),
                             initial_info_->initial_direction(),
                             initial_info_->initial_time(),
                             initial_info_->initial_position(),
                             initial_info_->initial_polarization());
    tree_io_->setWeight(initial_info_->weight());
  }
  else {
    event_id = get_loop_index();
  }

  const std::vector<ObservedParticle>& particles = event_store_->get_observed_particles();
  if (particles.size() > 0) {
    tree_io_->fill_particles(run_id, event_id, particles);
    set_evs("WriteObservationTree:Fill");
  }

  return AS_OK;
}

} /* namespace comptonsoft */
