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
#include "ComptonEventTreeIOWithInitialInfo.hh"
#include "VEventReconstructionAlgorithm.hh"


using namespace anlnext;

namespace comptonsoft
{

ReadComptonEventTree::ReadComptonEventTree()
  : anlgeant4::InitialInformation(false),
    cetree_(nullptr),
    tree_io_(new ComptonEventTreeIOWithInitialInfo)
{
  add_alias("InitialInformation");
}

ReadComptonEventTree::~ReadComptonEventTree() = default;

ANLStatus ReadComptonEventTree::mod_define()
{
  EventReconstruction::mod_define();
  hide_parameter("max_hits");
  hide_parameter("reconstruction_method");
  hide_parameter("parameter_file");
  define_parameter("file_list", &mod_class::file_list_);
  return AS_OK;
}

ANLStatus ReadComptonEventTree::mod_initialize()
{
  VCSModule::mod_initialize();

  define_evs("EventReconstruction:OK");
  define_evs("EventReconstruction:NG");
  initializeHitPatternData();

  cetree_ = std::make_unique<TChain>("cetree");
  for (const std::string& filename: file_list_) {
    cetree_->Add(filename.c_str());
  }

  tree_io_->set_tree(cetree_.get());
  if (cetree_->GetBranch("ini_energy")) {
    set_initial_information_stored();
    tree_io_->enableInitialInfoRecord();
  }
  else {
    tree_io_->disableInitialInfoRecord();
  }
  tree_io_->set_branch_addresses();

  num_entries_ = cetree_->GetEntries();
  std::cout << "Number of entries: " << num_entries_ << std::endl;

  return AS_OK;
}

ANLStatus ReadComptonEventTree::mod_analyze()
{
  if (entry_index_ == num_entries_) {
    return AS_QUIT;
  }

  EventReconstruction::initializeEvent();

  cetree_->GetEntry(entry_index_);

  const int32_t RunID = tree_io_->getRunID();
  const int32_t EventID = tree_io_->getEventID();
  set_run_id(RunID);
  set_event_id(EventID);

  if (initial_information_stored()) {
    set_initial_energy(tree_io_->getInitialEnergy());
    set_initial_direction(tree_io_->getInitialDirection());
    set_initial_time(tree_io_->getInitialTime());
    set_initial_position(tree_io_->getInitialPosition());
    set_initial_polarization(tree_io_->getInitialPolarization());
    set_weight(tree_io_->getWeight());
  }

  do {
    auto event = std::make_shared<BasicComptonEvent>();
    assignSourceInformation(*event);
    tree_io_->retrieveEvent(*event);
    pushReconstructedEvent(event);
    retrieveHitPatterns();
    set_evs("EventReconstruction:OK");
    ++entry_index_;

    if (entry_index_ == num_entries_) {
      return AS_QUIT;
    }
    cetree_->GetEntry(entry_index_);
  } while (tree_io_->getEventID() == EventID && tree_io_->getRunID() == RunID);

  return AS_OK;
}

} /* namespace comptonsoft */
