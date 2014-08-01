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


#include "KillStepPickUpData.hh"
#include "globals.hh"

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

using namespace anl;
using namespace comptonsoft;


KillStepPickUpData::KillStepPickUpData()
  : m_ParticleName("gamma"),
    stopping_volume_name("Tele"),
    m_ParticleType(0)
    //,enable(0)
{
  SetStepActOn(true);
}


ANLStatus KillStepPickUpData::mod_startup()
{
  //register_parameter(&enable,"Enabel Kill");
  register_parameter(&m_ParticleName, "particle");
  register_parameter(&stopping_volume_name, "volume");
  
  return AS_OK;
}

ANLStatus KillStepPickUpData::mod_init()
{
  if( m_ParticleName=="gamma" ){
    
    m_ParticleType = G4Gamma::GammaDefinition();
    
  }else if( m_ParticleName=="proton" ){
    
    m_ParticleType = G4Proton::ProtonDefinition();
    
  }else if( m_ParticleName=="electron" ){
    
    m_ParticleType = G4Electron::ElectronDefinition();
    
  }else{
    
    std::cout << " ** ERROR : Do not Exist Particle >> " << m_ParticleName << std::endl;
    return AS_QUIT_ERR;
  }
  
  
  return AS_OK;
}


void KillStepPickUpData::StepAct(const G4Step* , G4Track* aTrack)
{
  
  G4String VolumeName = aTrack->GetVolume()->GetName();
  
  //if( enable == 1 ){
  
  if( m_ParticleType == aTrack->GetDefinition() ){
    if( stopping_volume_name == VolumeName ) {
      aTrack->SetTrackStatus(fKillTrackAndSecondaries);
    }
  }
  //}
  
}

