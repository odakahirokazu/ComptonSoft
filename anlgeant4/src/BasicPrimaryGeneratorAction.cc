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

#include <G4ParticleGun.hh>
#include <G4Event.hh>
#include <G4ParticleDefinition.hh>

#include "BasicPrimaryGenerator.hh"

namespace anlgeant4
{

BasicPrimaryGeneratorAction::BasicPrimaryGeneratorAction()
  : particle_gun_(new G4ParticleGun(1))
{
}

BasicPrimaryGeneratorAction::~BasicPrimaryGeneratorAction() = default;

void BasicPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  const int event_id = event->GetEventID();
  const G4ParticleDefinition* particle = sampler_->particle_definition();
  const PrimarySetting primary_info = sampler_->make_primary_setting();

  set_particle_definition(const_cast<G4ParticleDefinition*>(particle));
  set_primary_setting(primary_info);
  sampler_->confirm_primary_setting(event_id, primary_info);
  particle_gun_->GeneratePrimaryVertex(event);
}

void BasicPrimaryGeneratorAction::set_particle_definition(G4ParticleDefinition* definition)
{
  particle_gun_->SetParticleDefinition(definition);
}

void BasicPrimaryGeneratorAction::set_primary_setting(const PrimarySetting& primary)
{
  particle_gun_->SetParticleTime(primary.time);
  particle_gun_->SetParticlePosition(primary.position);
  particle_gun_->SetParticleEnergy(primary.energy);
  particle_gun_->SetParticleMomentumDirection(primary.direction);
  particle_gun_->SetParticlePolarization(primary.polarization);
}

} /* namespace anlgeant4 */
