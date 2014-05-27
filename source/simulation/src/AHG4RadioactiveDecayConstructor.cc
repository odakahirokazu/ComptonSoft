/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato, Hirokazu Odaka                       *
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

#include "AHG4RadioactiveDecayConstructor.hh"

#include "G4ProcessManager.hh"
#include "G4RadioactiveDecay.hh"


AHG4RadioactiveDecayConstructor::AHG4RadioactiveDecayConstructor(G4int )
  : G4VPhysicsConstructor("Decay")
{
}


AHG4RadioactiveDecayConstructor::~AHG4RadioactiveDecayConstructor()
{
}


void AHG4RadioactiveDecayConstructor::ConstructParticle()
{
}


void AHG4RadioactiveDecayConstructor::ConstructProcess()
{
  G4RadioactiveDecay* RadioactiveDecay = new G4RadioactiveDecay();
  
  aParticleIterator->reset();
  while( (*aParticleIterator)() ){
    G4ParticleDefinition* Particle = aParticleIterator->value();
    G4ProcessManager* Manager = Particle->GetProcessManager();
    G4String ParticleName = Particle->GetParticleName();

    if (ParticleName == "GenericIon") {
      Manager->AddProcess(RadioactiveDecay, 0, -1, 3);
    } 
    /*else if (Decay->IsApplicable(*Particle)) { 
      Manager->AddProcess(Decay);
      Manager->SetProcessOrdering(Decay, idxPostStep);
      Manager->SetProcessOrdering(Decay, idxAtRest);
    }*/
  }
}
