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

#include "CSPhysicsList.hh"

#include "globals.hh"
#include "AstroUnits.hh"

#include "G4EmLivermorePhysics.hh"
#include "G4EmLivermorePolarizedPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_AllHP.hh"
#include "G4HadronPhysicsINCLXX.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

#include "CustomizedEmLivermorePhysics.hh"
#include "CustomizedEmLivermorePolarizedPhysics.hh"

#include "G4ParallelWorldProcess.hh"
#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserParallelWorld.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ChargedGeantino.hh"

namespace comptonsoft
{

CSPhysicsOption::CSPhysicsOption()
  : electronRangeRatio_(0.2), electronFinalRange_(100.0*anlgeant4::unit::um)
{
}

CSPhysicsList::CSPhysicsList(CSPhysicsOption option)
{
  SetVerboseLevel(1);
  
  // EM Physics
  const CSPhysicsOption::EMModel modelEM = option.getEMPhysicsModel();
  if (modelEM == CSPhysicsOption::EMModel::Livermore) {
    this->RegisterPhysics( new G4EmLivermorePhysics );
  }
  else if (modelEM == CSPhysicsOption::EMModel::LivermorePolarized) {
    this->RegisterPhysics( new G4EmLivermorePolarizedPhysics );
  }
  else if (modelEM == CSPhysicsOption::EMModel::CustomizedLivermore) {
    CustomizedEmLivermorePhysics* physicsEM = new CustomizedEmLivermorePhysics;
    physicsEM->setEMOptions(option.EMOptionFluo(),
                            option.EMOptionAuger(),
                            option.EMOptionPIXE());
    physicsEM->setElectronRangeParameters(option.ElectronRangeRatio(),
                                          option.ElectronFinalRange());
    this->RegisterPhysics(physicsEM);
  }
  else if (modelEM == CSPhysicsOption::EMModel::CustomizedLivermorePolarized) {
    CustomizedEmLivermorePolarizedPhysics* physicsEM = new CustomizedEmLivermorePolarizedPhysics;
    physicsEM->setEMOptions(option.EMOptionFluo(),
                            option.EMOptionAuger(),
                            option.EMOptionPIXE());
    physicsEM->setElectronRangeParameters(option.ElectronRangeRatio(),
                                          option.ElectronFinalRange());
    this->RegisterPhysics(physicsEM);
  }
  else {
    std::cout << "CSPhysicsList: unknown EM physics is specified." << std::endl;
    std::cout << "CSPhysicsList: G4EmLivermorePhysics is set." << std::endl;
    this->RegisterPhysics( new G4EmLivermorePhysics );
  }
    
  // Photo-nuclear interaction & synchroton radiation
  this->RegisterPhysics( new G4EmExtraPhysics );
  
  // Decays
  this->RegisterPhysics( new G4DecayPhysics );
  
  // Hadron elastic scattering
  if (option.getHadronPhysicsHighPrecision()) {
    this->RegisterPhysics( new G4HadronElasticPhysicsHP );
  }
  else {
    this->RegisterPhysics( new G4HadronElasticPhysics );
  }
  
  // Hadron inelastic interactions
  const CSPhysicsOption::HadronModel modelHadron = option.getHadronPhysicsModel();
  if (modelHadron == CSPhysicsOption::HadronModel::BERT ) {
    if (option.getHadronPhysicsHighPrecision()) {
      this->RegisterPhysics( new G4HadronPhysicsQGSP_BERT_HP );
    }
    else {
      this->RegisterPhysics( new G4HadronPhysicsQGSP_BERT );
    }
  }
  else if (modelHadron == CSPhysicsOption::HadronModel::BIC) {
    if (option.getHadronPhysicsHighPrecision()) {
      this->RegisterPhysics( new G4HadronPhysicsQGSP_BIC_HP );
    }
    else {
      this->RegisterPhysics( new G4HadronPhysicsQGSP_BIC );
    }
  }
  else if (modelHadron == CSPhysicsOption::HadronModel::INCLXX) {
    this->RegisterPhysics( new G4HadronPhysicsINCLXX );
  }
  else if (modelHadron == CSPhysicsOption::HadronModel::BERT_HP) {
    this->RegisterPhysics( new G4HadronPhysicsQGSP_BERT_HP );
  }
  else if (modelHadron == CSPhysicsOption::HadronModel::BIC_HP) {
    this->RegisterPhysics( new G4HadronPhysicsQGSP_BIC_HP );
  }
  else if (modelHadron == CSPhysicsOption::HadronModel::BIC_AllHP) {
    this->RegisterPhysics( new G4HadronPhysicsQGSP_BIC_AllHP );
  }
  else {
    std::cout << "CSPhysicsList: unknown hadron physics is specified." << std::endl;
    std::cout << "CSPhysicsList: G4HadronPhysicsQGSP_BIC_HP is set." << std::endl;
    this->RegisterPhysics( new G4HadronPhysicsQGSP_BIC_HP );
  }
  
  // Stopping physics
  this->RegisterPhysics( new G4StoppingPhysics );
  
  // Ion physics
  this->RegisterPhysics( new G4IonPhysics );
  
  // Radioactive decay
  if (option.isRadioactiveDecayEnabled()) {
    this->RegisterPhysics( new G4RadioactiveDecayPhysics );
  }
}

void CSPhysicsList::ConstructProcess()
{
  G4VModularPhysicsList::ConstructProcess();
  if (isParallelWorldEnabled()) {
    AddParallelWorldProcess();
  }
}

void CSPhysicsList::AddParallelWorldProcess()
{
  const G4VUserDetectorConstruction* userDetectorConstruction
    = G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  const G4int numParallelWorlds
    = userDetectorConstruction->GetNumberOfParallelWorld();

  for (int i=0; i<numParallelWorlds; ++i) {
    G4String parallelWorldName
      = userDetectorConstruction->GetParallelWorld(i)->GetName();

    // Add parallel world process
    G4ParallelWorldProcess* theParallelWorldProcess
      = new G4ParallelWorldProcess(parallelWorldName+"Process");
    theParallelWorldProcess->SetParallelWorld(parallelWorldName);
    theParallelWorldProcess->SetLayeredMaterialFlag();

    auto theParticleIterator = GetParticleIterator();
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      if(particle!=G4ChargedGeantino::Definition()) {
        G4ProcessManager* pmanager = particle->GetProcessManager();
        pmanager->AddProcess(theParallelWorldProcess);
        if(theParallelWorldProcess->IsAtRestRequired(particle))
        { pmanager->SetProcessOrdering(theParallelWorldProcess, idxAtRest, 9999); }
        pmanager->SetProcessOrdering(theParallelWorldProcess, idxAlongStep, 1);
        pmanager->SetProcessOrdering(theParallelWorldProcess, idxPostStep, 9999);
      }
    }
  }
}

} /* namespace comptonsoft */
