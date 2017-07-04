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

#include "BasicPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "AstroUnits.hh"
#include "BasicPrimaryGen.hh"

namespace anlgeant4
{

BasicPrimaryGeneratorAction::BasicPrimaryGeneratorAction()
  : m_ParticleGun(new G4ParticleGun(1)),
    m_Time(0.0), m_Position(0.0, 0.0, 0.0),
    m_Energy(10.0*unit::keV), m_Direction(1.0, 0.0, 0.0)
{
}

BasicPrimaryGeneratorAction::BasicPrimaryGeneratorAction(G4ParticleDefinition* definition)
  : m_ParticleGun(new G4ParticleGun(1)),
    m_Time(0.0), m_Position(0.0, 0.0, 0.0),
    m_Energy(10.0*unit::keV), m_Direction(1.0, 0.0, 0.0)
{
  m_ParticleGun->SetParticleDefinition(definition);
}

BasicPrimaryGeneratorAction::BasicPrimaryGeneratorAction(G4String particle_name)
  : m_ParticleGun(new G4ParticleGun(1)),
    m_Time(0.0), m_Position(0.0, 0.0, 0.0),
    m_Energy(10.0*unit::keV), m_Direction(1.0, 0.0, 0.0)
{
  G4ParticleDefinition* particle = 
    G4ParticleTable::GetParticleTable()->FindParticle(particle_name);
  m_ParticleGun->SetParticleDefinition(particle);
}

BasicPrimaryGeneratorAction::~BasicPrimaryGeneratorAction() = default;

void BasicPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  if (m_GeneratorSetting) {
    m_GeneratorSetting->makePrimarySetting();
    m_GeneratorSetting->confirmPrimarySetting();
  }

  m_ParticleGun->SetParticleTime(m_Time);
  m_ParticleGun->SetParticlePosition(m_Position);
  m_ParticleGun->SetParticleEnergy(m_Energy);
  m_ParticleGun->SetParticleMomentumDirection(m_Direction);
  if (m_Polarization.x()!=0.0 ||
      m_Polarization.y()!=0.0 ||
      m_Polarization.z()!=0.0) {
    m_ParticleGun->SetParticlePolarization(m_Polarization);
  }
  m_ParticleGun->GeneratePrimaryVertex(anEvent);
}

void BasicPrimaryGeneratorAction::SetDefinition(G4ParticleDefinition* definition)
{
  m_ParticleGun->SetParticleDefinition(definition);
}

} /* namespace anlgeant4 */
