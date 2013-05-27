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

#include "Geant4Simple.hh"

#include <ctime>
#include <boost/lexical_cast.hpp>

#include "G4RunManager.hh"

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
using namespace anlgeant4;


Geant4Simple::Geant4Simple()
  : m_G4RunManager(0),
    m_RandomEnginePtr(0),
    m_RandomSeed1(0),
    m_NumberOfTrial(10000),
    m_VerboseLevel(0)
{
  m_G4RunManager = new G4RunManager;
  
  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
  scoringManager->SetVerboseLevel(1);
  
  require_module_access("VPickUpData");
  require_module_access("VANLGeometry");
  require_module_access("VANLPhysicsList");
  require_module_access("VANLPrimaryGen");
}


Geant4Simple::~Geant4Simple()
{
  if (m_G4RunManager) delete m_G4RunManager;
  if (m_RandomEnginePtr) delete m_RandomEnginePtr;
}


ANLStatus Geant4Simple::mod_startup()
{
  register_parameter(&m_RandomSeed1, "Random seed");
  register_parameter(&m_NumberOfTrial, "Number of trials");
  register_parameter(&m_VerboseLevel, "Verbose level");
  
  return AS_OK;
}


ANLStatus Geant4Simple::mod_init()
{
  using CLHEP::HepRandom;

  m_RandomEnginePtr = new CLHEP::MTwistEngine;
  HepRandom::setTheEngine(m_RandomEnginePtr);
  
  HepRandom::setTheSeed(m_RandomSeed1);
  std::cout << "Random seed: " << m_RandomSeed1 << std::endl;
  
  set_user_initializations();
  set_user_primarygen_action();
  set_user_std_actions();
  apply_commands();

  m_G4RunManager->Initialize();

  return AS_OK;
}


void Geant4Simple::set_user_initializations()
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


void Geant4Simple::set_user_primarygen_action()
{
  VANLPrimaryGen* primaryGen;
  GetANLModuleNC("VANLPrimaryGen", &primaryGen); 
  G4VUserPrimaryGeneratorAction* userPrimaryGeneratorAction
    = primaryGen->create();
  m_G4RunManager->SetUserAction(userPrimaryGeneratorAction);
}


void Geant4Simple::set_user_std_actions()
{
  VPickUpData* pickupData;
  GetANLModuleNC("VPickUpData", &pickupData);
  pickupData->RegisterUserActions(m_G4RunManager);
}


void Geant4Simple::apply_commands()
{
  // set verbose levels
  G4String runVerbose = "/run/verbose 1";
  G4String eventVerbose = "/event/verbose ";
  G4String trackingVerbose = "/tracking/verbose ";
  std::string verbose = boost::lexical_cast<std::string>(m_VerboseLevel);
  eventVerbose = eventVerbose + verbose;
  trackingVerbose = trackingVerbose + verbose;
  G4cout << runVerbose << G4endl;
  G4cout << eventVerbose << G4endl;
  G4cout << trackingVerbose << G4endl;
  G4UImanager* ui = G4UImanager::GetUIpointer();
  ui->ApplyCommand(runVerbose);
  ui->ApplyCommand(eventVerbose);
  ui->ApplyCommand(trackingVerbose);
}


ANLStatus Geant4Simple::mod_ana()
{
  m_G4RunManager->BeamOn(m_NumberOfTrial);
  return AS_OK;
}


ANLStatus Geant4Simple::mod_exit()
{
  delete m_G4RunManager;
  m_G4RunManager = 0;

  return AS_OK;
}
