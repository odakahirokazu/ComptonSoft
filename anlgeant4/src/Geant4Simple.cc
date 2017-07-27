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
#include "VUserActionAssembly.hh"

using namespace anlnext;

namespace anlgeant4
{

Geant4Simple::Geant4Simple()
  : m_G4RunManager(new G4RunManager),
    m_RandomSeed1(0),
    m_NumberOfTrial(10000),
    m_VerboseLevel(0)
{
  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
  scoringManager->SetVerboseLevel(1);
}

Geant4Simple::~Geant4Simple() = default;

ANLStatus Geant4Simple::mod_define()
{
  register_parameter(&m_RandomSeed1, "random_seed");
  register_parameter(&m_NumberOfTrial, "number_of_trials");
  register_parameter(&m_VerboseLevel, "verbose");
  
  return AS_OK;
}

ANLStatus Geant4Simple::mod_initialize()
{
  m_RandomEnginePtr.reset(new CLHEP::MTwistEngine);
  CLHEP::HepRandom::setTheEngine(m_RandomEnginePtr.get());
  CLHEP::HepRandom::setTheSeed(m_RandomSeed1);
  
  set_user_initializations();
  set_user_primary_generator_action();
  set_user_defined_actions();
  apply_commands();

  m_G4RunManager->Initialize();

  return AS_OK;
}

void Geant4Simple::set_user_initializations()
{
  VANLGeometry* geometry;
  get_module_NC("VANLGeometry", &geometry);
  G4VUserDetectorConstruction* userDetectorConstruction = geometry->create();
  m_G4RunManager->SetUserInitialization(userDetectorConstruction);
  
  VANLPhysicsList* physics;
  get_module_NC("VANLPhysicsList", &physics);
  G4VUserPhysicsList* userPhysicsList = physics->create();
  m_G4RunManager->SetUserInitialization(userPhysicsList);
}

void Geant4Simple::set_user_primary_generator_action()
{
  VANLPrimaryGen* primaryGen;
  get_module_NC("VANLPrimaryGen", &primaryGen);
  G4VUserPrimaryGeneratorAction* userPrimaryGeneratorAction
    = primaryGen->create();
  m_G4RunManager->SetUserAction(userPrimaryGeneratorAction);
}

void Geant4Simple::set_user_defined_actions()
{
  VUserActionAssembly* userActionAssembly;
  get_module_NC("VUserActionAssembly", &userActionAssembly);
  userActionAssembly->registerUserActions(m_G4RunManager.get());
}

void Geant4Simple::apply_commands()
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
  std::cout << std::endl;
}

ANLStatus Geant4Simple::mod_analyze()
{
  m_G4RunManager->BeamOn(m_NumberOfTrial);
  return AS_OK;
}

} /* namespace anlgeant4 */
