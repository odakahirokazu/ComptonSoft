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

#include "G4ScoringManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4UImanager.hh"

#include "VANLGeometry.hh"
#include "VANLPhysicsList.hh"
#include "VANLPrimaryGen.hh"
#include "VPickUpData.hh"

using namespace anl;

namespace anlgeant4
{

Geant4Body::Geant4Body()
  : m_G4RunManager(0),
    m_RandomEnginePtr(0),
    m_RandomEngine("MTwistEngine"),
    m_RandomInitMode(1),
    m_RandomSeed1(0),
    m_OutputRandomStatus(true),
    m_RandomInitialStatusFileName("RandomSeed_i.dat"),
    m_RandomFinalStatusFileName("RandomSeed_f.dat"),
    m_VerboseLevel(0)
{
  m_G4RunManager = new ANLG4RunManager;
  
  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
  scoringManager->SetVerboseLevel(1);
  
  require_module_access("VPickUpData");
  require_module_access("VANLGeometry");
  require_module_access("VANLPhysicsList");
  require_module_access("VANLPrimaryGen");
}

Geant4Body::~Geant4Body()
{
  if (m_G4RunManager) delete m_G4RunManager;
  if (m_RandomEnginePtr) delete m_RandomEnginePtr;
}

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
  
  return AS_OK;
}

ANLStatus Geant4Body::mod_com()
{
  ask_parameter("random_initialization_mode");
  hide_parameter("random_initialization_mode");
  if (m_RandomInitMode!=1) {
    hide_parameter("random_seed");
  }
  return BasicModule::mod_com();
}

ANLStatus Geant4Body::mod_init()
{
  using CLHEP::HepRandom;

  if (m_RandomEngine=="MTwistEngine") {
    m_RandomEnginePtr = new CLHEP::MTwistEngine;
    HepRandom::setTheEngine(m_RandomEnginePtr);
  }
  else if (m_RandomEngine=="RanecuEngine") {
    m_RandomEnginePtr = new CLHEP::RanecuEngine;
    HepRandom::setTheEngine(m_RandomEnginePtr);
  }
  else if (m_RandomEngine=="HepJamesRandom") {
    m_RandomEnginePtr = new CLHEP::HepJamesRandom;
    HepRandom::setTheEngine(m_RandomEnginePtr);
  }
  
  if (m_RandomInitMode==0) {
    m_RandomSeed1 = std::time(0);
    HepRandom::setTheSeed(m_RandomSeed1);
    std::cout << "Random seed: " << m_RandomSeed1 << std::endl;

    if (m_OutputRandomStatus) {
      HepRandom::saveEngineStatus(m_RandomInitialStatusFileName.c_str());
    }
  }
  else if (m_RandomInitMode==1) {
    HepRandom::setTheSeed(m_RandomSeed1);
    std::cout << "Random seed: " << m_RandomSeed1 << std::endl;

    if (m_OutputRandomStatus) {
      HepRandom::saveEngineStatus(m_RandomInitialStatusFileName.c_str());
    }
  }
  else if (m_RandomInitMode==2) {
    HepRandom::restoreEngineStatus(m_RandomInitialStatusFileName.c_str());
  }
  else {
    std::cout << "Invalid value [Random initialization mode] : "
              << m_RandomInitMode << std::endl;
    return AS_QUIT_ERR;
  }

  set_user_initializations();
  set_user_primarygen_action();
  set_user_std_actions();
  apply_commands();

  m_G4RunManager->Initialize();

  return AS_OK;
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

void Geant4Body::set_user_primarygen_action()
{
  VANLPrimaryGen* primarygen;
  GetANLModuleNC("VANLPrimaryGen", &primarygen); 
  G4VUserPrimaryGeneratorAction* userPrimaryGeneratorAction
    = primarygen->create();
  m_G4RunManager->SetUserAction(userPrimaryGeneratorAction);
}

void Geant4Body::set_user_std_actions()
{
  VPickUpData* pickupdata;
  GetANLModuleNC("VPickUpData", &pickupdata);
  pickupdata->RegisterUserActions(m_G4RunManager);
}

void Geant4Body::apply_commands()
{
  // set verbose levels
  G4String runverbose = "/run/verbose ";
  G4String eventverbose = "/event/verbose ";
  G4String trackingverbose = "/tracking/verbose ";
  std::string verbose = boost::lexical_cast<std::string>(m_VerboseLevel);
  runverbose = runverbose + verbose;
  eventverbose = eventverbose + verbose;
  trackingverbose = trackingverbose + verbose;
  G4cout << runverbose << G4endl;
  G4cout << eventverbose << G4endl;
  G4cout << trackingverbose << G4endl;
  G4UImanager * UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand(runverbose);
  UI->ApplyCommand(eventverbose);
  UI->ApplyCommand(trackingverbose);
}

ANLStatus Geant4Body::mod_bgnrun()
{
  m_G4RunManager->ANLbgnrunfunc();
  return AS_OK;
}

ANLStatus Geant4Body::mod_ana()
{
  m_G4RunManager->ANLanafunc();
  return AS_OK;
}

ANLStatus Geant4Body::mod_endrun()
{
  m_G4RunManager->ANLendrunfunc();
  return AS_OK;
}

ANLStatus Geant4Body::mod_exit()
{
  if (m_OutputRandomStatus) {
    CLHEP::HepRandom::saveEngineStatus(m_RandomFinalStatusFileName.c_str());
  }

  delete m_G4RunManager;
  m_G4RunManager = 0;

  return AS_OK;
}

} /* namespace anlgeant4 */
