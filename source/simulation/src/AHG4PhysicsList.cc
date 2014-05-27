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

#include "AHG4PhysicsList.hh"

#include "globals.hh"

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
#include "G4HadronPhysicsINCLXX.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonPhysics.hh"
// #include "G4AdjointPhysicsList.hh"

#include "AHG4EmLivermorePhysics.hh"
#include "AHG4RadioactiveDecayConstructor.hh"

#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserParallelWorld.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ChargedGeantino.hh"

#include "G4ParallelWorldProcess.hh"
// #include "G4XrayBoundaryProcess.hh"

AHG4PhysicsList::AHG4PhysicsList(G4String option)
{
  G4int ver = 1;
  
  // EM Physics
  if (option.find("PC") != std::string::npos) {
    this->RegisterPhysics( new G4EmLivermorePolarizedPhysics(ver) );
  }
  else {
    if (option.find("resp") != std::string::npos) {
      AHG4EmLivermorePhysics* physEM = new AHG4EmLivermorePhysics(ver);
      physEM->SetElectronFinalRange(0.001*mm);
      physEM->ActivateAuger(true);
      physEM->ActivatePIXE(true);
      this->RegisterPhysics(physEM);
    }
    else {
      this->RegisterPhysics( new G4EmLivermorePhysics(ver) );
    }
  }
    
  // Synchroton Radiation & GN Physics
  this->RegisterPhysics( new G4EmExtraPhysics(ver) );
  
  // Decays
  this->RegisterPhysics( new G4DecayPhysics(ver) );
  
  // Hadron Elastic scattering
  if (option.find("HP") != std::string::npos) {
    this->RegisterPhysics( new G4HadronElasticPhysicsHP(ver) );
  }
  else {
    this->RegisterPhysics( new G4HadronElasticPhysics(ver) );
  }
  
  // Hadron Physics
  if (option.find("INCLXX") != std::string::npos) {
    this->RegisterPhysics( new G4HadronPhysicsINCLXX(ver) );
  }
  else if (option.find("BIC") != std::string::npos) {
    if (option.find("HP") != std::string::npos) {
      this->RegisterPhysics( new G4HadronPhysicsQGSP_BIC_HP(ver) );
    }
    else {
      this->RegisterPhysics( new G4HadronPhysicsQGSP_BIC(ver) );
    }
  }
  else {
    if (option.find("HP") != std::string::npos) {
      this->RegisterPhysics( new G4HadronPhysicsQGSP_BERT_HP(ver) );
    }
    else {
      this->RegisterPhysics( new G4HadronPhysicsQGSP_BERT(ver) );
    }
  }
  
  // Stopping Physics
  this->RegisterPhysics( new G4StoppingPhysics(ver) );
  
  // Ion Physics
  this->RegisterPhysics( new G4IonPhysics(ver) );
  
  // RadioActive Decay
  if (option.find("RD") != std::string::npos) {
    this->RegisterPhysics( new AHG4RadioactiveDecayConstructor(ver) );
  }

  // Reverse/Adjoint MC PhysicsList
  // this->RegisterPhysics( new G4AdjointPhysicsList(ver));	
}


void AHG4PhysicsList::SetCuts()
{
  SetCutsWithDefault();
}


#if 0
void AHG4PhysicsList::ConstructProcess()
{
  AddTransportation();
  AddParallelWorldProcess();
  // AddXrayBoundary();
  
  G4PhysConstVector::iterator itr;
  for (itr = physicsVector->begin(); itr!= physicsVector->end(); ++itr) {
    (*itr)->ConstructProcess();
  }
}
#endif

#if 0
void AHG4PhysicsList::AddParallelWorldProcess()
{
  const G4VUserDetectorConstruction* userDetectorConstruction
    = G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  G4int numParallelWorld = userDetectorConstruction->GetNumberOfParallelWorld();
  for (int i=0; i<numParallelWorld; ++i) {
    G4String parallelWorldName
      = userDetectorConstruction->GetParallelWorld(i)->GetName();
    
    // Add parallel world process
    G4ParallelWorldProcess* theParallelWorldProcess
      = new G4ParallelWorldProcess(parallelWorldName+"Proc");
    theParallelWorldProcess->SetParallelWorld(parallelWorldName);
    theParallelWorldProcess->SetLayeredMaterialFlag();
    
    aParticleIterator->reset();
    while( (*aParticleIterator)() ){
      G4ParticleDefinition* particle = aParticleIterator->value();
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
#endif


#if 0
void AHG4PhysicsList::AddXrayBoundary()
{
  aParticleIterator->reset();
  while( (*aParticleIterator)() ) {
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (particleName == "gamma") {
      pmanager->AddDiscreteProcess(new G4XrayBoundaryProcess());
    }
  }
}
#endif
