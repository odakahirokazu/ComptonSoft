/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Shin Watanabe, Hirokazu Odaka                      *
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

#include "Geant4Body.hh"

#include <ctime>
#include <chrono>
#include <boost/lexical_cast.hpp>

#include "G4RunManagerFactory.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4UImanager.hh"

#include "ActionInitialization.hh"
#include "VANLGeometry.hh"
#include "VANLPhysicsList.hh"
#include "VANLPrimaryGen.hh"
#include "VUserActionAssembly.hh"

using namespace anlnext;

namespace anlgeant4
{

Geant4Body::Geant4Body()
  : run_manager_(nullptr),
    action_initialization_(nullptr),
    num_events_(1000),
    num_threads_(0),
    random_engine_("MixMaxRng"),
    random_seed_(0),
    verbose_level_(0),
    random_seed_initial_(0)
{
}

Geant4Body::~Geant4Body() = default;

ANLStatus Geant4Body::mod_define()
{
  define_parameter("num_events", &mod_class::num_events_);
  define_parameter("num_threads", &mod_class::num_threads_);
  define_parameter("print_beamon_time", &mod_class::print_beamon_time_);
  define_parameter("random_engine", &mod_class::random_engine_);
  define_parameter("random_engine", &mod_class::random_engine_);
  define_parameter("random_seed", &mod_class::random_seed_);
  define_parameter("verbose", &mod_class::verbose_level_);
  define_parameter("commands", &mod_class::user_commands_);

  define_result("random_seed_initial", &mod_class::random_seed_initial_);

  return AS_OK;
}

ANLStatus Geant4Body::mod_pre_initialize()
{
  initialize_random_generator();
  run_manager_.reset(G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default));
  action_initialization_ = new ActionInitialization;

  return AS_OK;
}

ANLStatus Geant4Body::mod_initialize()
{
  define_evs("Geant4Body:BeamOn");
  define_evs("Geant4Body:DataStored");

  set_user_initializations();
  apply_commands();

  if (num_threads_ > 0) {
    run_manager_->SetNumberOfThreads(num_threads_);
  }

  run_manager_->Initialize();

  return AS_OK;
}

void Geant4Body::initialize_random_generator()
{
  if (random_engine_ == "MixMaxRng") {
    random_engine_ptr_.reset(new CLHEP::MixMaxRng);
  }
  else if (random_engine_ == "MTwistEngine") {
    random_engine_ptr_.reset(new CLHEP::MTwistEngine);
  }
  else if (random_engine_ == "RanecuEngine") {
    random_engine_ptr_.reset(new CLHEP::RanecuEngine);
  }
  else if (random_engine_ == "HepJamesRandom") {
    random_engine_ptr_.reset(new CLHEP::HepJamesRandom);
  }

  CLHEP::HepRandom::setTheEngine(random_engine_ptr_.get());

  if (random_seed_ == 0) {
    random_seed_initial_ = std::time(0);
  }
  else {
    random_seed_initial_ = random_seed_;
  }

  CLHEP::HepRandom::setTheSeed(random_seed_initial_);

  std::cout << '\n'
            << "Random generator initialization\n"
            << "  Random engine : " << random_engine_ << '\n'
            << "  Random seed   : " << random_seed_initial_ << '\n';
  if (random_seed_ == 0) {
    std::cout << "  Random seed 0 was input ===> set the current time\n";
  }
  std::cout << std::endl;
}

void Geant4Body::set_user_initializations()
{
  VANLGeometry* geometry_module = nullptr;
  get_module_NC("VANLGeometry", &geometry_module);
  G4VUserDetectorConstruction* user_detector_construction = geometry_module->create();
  run_manager_->SetUserInitialization(user_detector_construction);

  VANLPhysicsList* physics_module = nullptr;
  get_module_NC("VANLPhysicsList", &physics_module);
  G4VUserPhysicsList* user_physics_list = physics_module->create();
  run_manager_->SetUserInitialization(user_physics_list);

  run_manager_->SetUserInitialization(action_initialization_);
}

void Geant4Body::register_user_action(VANLPrimaryGen* primary_gen)
{
  action_initialization_->registerUserAction(primary_gen);
}

void Geant4Body::register_user_action(VUserActionAssembly* uaa)
{
  action_initialization_->registerUserAction(uaa);
}

void Geant4Body::apply_commands()
{
  using boost::format;
  using boost::str;

  G4UImanager* ui = G4UImanager::GetUIpointer();

  std::vector<std::string> presetCommands;
  presetCommands.push_back( str(format("/run/verbose %d") % verbose_level_) );
  presetCommands.push_back( str(format("/event/verbose %d") % verbose_level_) );
  presetCommands.push_back( str(format("/tracking/verbose %d") % verbose_level_) );

  std::cout << "\nApplying preset commands:" << std::endl;
  for (const std::string& com: presetCommands) {
    std::cout << com << std::endl;
    ui->ApplyCommand(com);
  }

  std::cout << "\nApplying user commands:" << std::endl;
  for (const std::string& com: user_commands_) {
    std::cout << com << std::endl;
    ui->ApplyCommand(com);
  }
  std::cout << std::endl;
}

ANLStatus Geant4Body::mod_begin_run()
{
  return AS_OK;
}

ANLStatus Geant4Body::mod_analyze()
{
  if (!evs("Geant4Body:DataStored")) {
    set_evs("Geant4Body:BeamOn");

    const auto t0 = std::chrono::steady_clock::now();

    run_manager_->BeamOn(num_events_);

    const auto t1 = std::chrono::steady_clock::now();
    const double duration =
      std::chrono::duration<double>(t1 - t0).count();

    if (print_beamon_time_) {
      std::cout << "BeamOn time: " << duration << " s"
                << "  events: " << num_events_
                << "  rate: " << num_events_ / duration << " events/s"
                << std::endl;
    }
  }

  return AS_OK;
}

ANLStatus Geant4Body::mod_end_run()
{
  return AS_OK;
}

ANLStatus Geant4Body::mod_finalize()
{
  run_manager_.reset(nullptr);

  return AS_OK;
}

} /* namespace anlgeant4 */
