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
#include <boost/lexical_cast.hpp>

#include "ANLG4RunManager.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4UImanager.hh"

#include "VANLGeometry.hh"
#include "VANLPhysicsList.hh"
#include "VANLPrimaryGen.hh"
#include "VUserActionAssembly.hh"

using namespace anl;

namespace anlgeant4
{

Geant4Body::Geant4Body()
  : m_G4RunManager(new ANLG4RunManager),
    m_EventIndex(0),
    m_RandomEngine("MTwistEngine"),
    m_RandomInitMode(1),
    m_RandomSeed1(0),
    m_OutputRandomStatus(true),
    m_RandomInitialStatusFileName("RandomSeed_i.dat"),
    m_RandomFinalStatusFileName("RandomSeed_f.dat"),
    m_VerboseLevel(0)
{
  require_module_access("VUserActionAssembly");
  require_module_access("VANLGeometry");
  require_module_access("VANLPhysicsList");
  require_module_access("VANLPrimaryGen");
}

Geant4Body::~Geant4Body() = default;

ANLStatus Geant4Body::mod_startup()
{
  register_parameter(&m_RandomEngine, "random_engine");
  register_parameter(&m_RandomInitMode, "random_initialization_mode");
  set_parameter_question("Random initialization mode (0: auto, 1: interger, 2: state file)");
  register_parameter(&m_RandomSeed1, "random_seed");
  register_parameter(&m_OutputRandomStatus, "output_random_status");
  register_parameter(&m_RandomInitialStatusFileName,
                     "random_initial_status_file");
  register_parameter(&m_RandomFinalStatusFileName,
                     "random_final_status_file");

  register_parameter(&m_VerboseLevel, "verbose");
  register_parameter(&m_UserCommands, "commands");
  
  return AS_OK;
}

ANLStatus Geant4Body::mod_init()
{
  if (m_RandomInitMode==0 || m_RandomInitMode==1 || m_RandomInitMode==2) {
    initialize_random_generator();
  }
  else {
    std::cout << "Invalid value [Random initialization mode] : "
              << m_RandomInitMode << std::endl;
    return AS_QUIT_ERR;
  }

  set_user_initializations();
  set_user_primary_generator_action();
  set_user_defined_actions();
  apply_commands();

  m_G4RunManager->Initialize();

  return AS_OK;
}

void Geant4Body::initialize_random_generator()
{
  if (m_RandomEngine=="MTwistEngine") {
    m_RandomEnginePtr.reset(new CLHEP::MTwistEngine);
    CLHEP::HepRandom::setTheEngine(m_RandomEnginePtr.get());
  }
  else if (m_RandomEngine=="RanecuEngine") {
    m_RandomEnginePtr.reset(new CLHEP::RanecuEngine);
    CLHEP::HepRandom::setTheEngine(m_RandomEnginePtr.get());
  }
  else if (m_RandomEngine=="HepJamesRandom") {
    m_RandomEnginePtr.reset(new CLHEP::HepJamesRandom);
    CLHEP::HepRandom::setTheEngine(m_RandomEnginePtr.get());
  }
  
  if (m_RandomInitMode==0) {
    m_RandomSeed1 = std::time(0);
    CLHEP::HepRandom::setTheSeed(m_RandomSeed1);
    std::cout << "Random seed: " << m_RandomSeed1 << std::endl;

    if (m_OutputRandomStatus) {
      CLHEP::HepRandom::saveEngineStatus(m_RandomInitialStatusFileName.c_str());
    }
  }
  else if (m_RandomInitMode==1) {
    CLHEP::HepRandom::setTheSeed(m_RandomSeed1);
    std::cout << "Random seed: " << m_RandomSeed1 << std::endl;

    if (m_OutputRandomStatus) {
      CLHEP::HepRandom::saveEngineStatus(m_RandomInitialStatusFileName.c_str());
    }
  }
  else if (m_RandomInitMode==2) {
    CLHEP::HepRandom::restoreEngineStatus(m_RandomInitialStatusFileName.c_str());
  }
}

void Geant4Body::set_user_initializations()
{
  VANLGeometry* geometry;
  GetANLModuleNC("VANLGeometry", &geometry);
  G4VUserDetectorConstruction* userDetectorConstruction = geometry->create();
  m_G4RunManager->SetUserInitialization(userDetectorConstruction);
  
  VANLPhysicsList* physics;
  GetANLModuleNC("VANLPhysicsList", &physics);
  G4VUserPhysicsList* userPhysicsList = physics->create();
  m_G4RunManager->SetUserInitialization(userPhysicsList);
}

void Geant4Body::set_user_primary_generator_action()
{
  VANLPrimaryGen* primaryGen;
  GetANLModuleNC("VANLPrimaryGen", &primaryGen);
  G4VUserPrimaryGeneratorAction* userPrimaryGeneratorAction
    = primaryGen->create();
  m_G4RunManager->SetUserAction(userPrimaryGeneratorAction);
}

void Geant4Body::set_user_defined_actions()
{
  VUserActionAssembly* userActionAssembly;
  GetANLModuleNC("VUserActionAssembly", &userActionAssembly);
  userActionAssembly->registerUserActions(m_G4RunManager.get());
}

void Geant4Body::apply_commands()
{
  using boost::format;
  using boost::str;

  G4UImanager* ui = G4UImanager::GetUIpointer();
  
  std::vector<std::string> presetCommands;
  presetCommands.push_back( str(format("/run/verbose %d") % m_VerboseLevel) );
  presetCommands.push_back( str(format("/event/verbose %d") % m_VerboseLevel) );
  presetCommands.push_back( str(format("/tracking/verbose %d") % m_VerboseLevel) );

  std::cout << "\nApplying preset commands:" << std::endl;
  for (const std::string& com: presetCommands) {
    std::cout << com << std::endl;
    ui->ApplyCommand(com);
  }

  std::cout << "\nApplying user commands:" << std::endl;
  for (const std::string& com: m_UserCommands) {
    std::cout << com << std::endl;
    ui->ApplyCommand(com);
  }
  std::cout << std::endl;
}

ANLStatus Geant4Body::mod_bgnrun()
{
  const G4bool cond = m_G4RunManager->ConfirmBeamOnCondition();
  if (cond) {
    m_G4RunManager->ConstructScoringWorlds();
    m_G4RunManager->RunInitialization();
    m_G4RunManager->InitializeEventLoop(0, nullptr, 0);
  }
  else {
    return AS_QUIT_ERR;
  }

  return AS_OK;
}

ANLStatus Geant4Body::mod_ana()
{
  const ANLStatus status = m_G4RunManager->performOneEvent(m_EventIndex);
  ++m_EventIndex;

  return status;
}

ANLStatus Geant4Body::mod_endrun()
{
  m_G4RunManager->TerminateEventLoop();
  m_G4RunManager->RunTermination();

  return AS_OK;
}

ANLStatus Geant4Body::mod_exit()
{
  if (m_OutputRandomStatus) {
    CLHEP::HepRandom::saveEngineStatus(m_RandomFinalStatusFileName.c_str());
  }

  return AS_OK;
}

} /* namespace anlgeant4 */
