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

#include "ScatteringPickUpData.hh"
#include "globals.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "TDirectory.h"
#include "SaveData.hh"

using namespace anl;
using namespace comptonsoft;


ScatteringPickUpData::ScatteringPickUpData()
  : m_Tree(0), m_DirX(0.0), m_DirY(0.0), m_DirZ(0.0), m_Energy(0.0),
    m_FirstInteraction(false), m_ProcessName("compton")
{
  SetStepActOn(true);
}


ANLStatus ScatteringPickUpData::mod_startup()
{
  register_parameter(&m_ProcessName, "process_name");
  return AS_OK;
}


ANLStatus ScatteringPickUpData::mod_his()
{
  if (ModuleExist("SaveData")) {
    SaveData* saveModule;
    GetANLModuleNC("SaveData", &saveModule);
    saveModule->GetDirectory()->cd();
  }
  
  m_Tree = new TTree("scattering_tree", "scattering_tree");
  m_Tree->Branch("dirx", &m_DirX, "dirx/D");
  m_Tree->Branch("diry", &m_DirY, "diry/D");
  m_Tree->Branch("dirz", &m_DirZ, "dirz/D");
  m_Tree->Branch("energy", &m_Energy, "energy/D");
  
  return AS_OK;
}


void ScatteringPickUpData::EventAct_begin(const G4Event*)
{
  m_FirstInteraction = true;
}


void ScatteringPickUpData::StepAct(const G4Step* aStep, G4Track* aTrack)
{
  if (!m_FirstInteraction) return;

  if (aTrack->GetTrackID()==1) {
    G4StepPoint* sp = aStep->GetPostStepPoint();
    G4String processName = sp->GetProcessDefinedStep()->GetProcessName();
    if (processName==m_ProcessName.c_str()) {
      G4ThreeVector direction = sp->GetMomentumDirection();
      m_DirX = direction.x();
      m_DirY = direction.y();
      m_DirZ = direction.z();
      m_Energy = sp->GetKineticEnergy();
      m_Tree->Fill();
      m_FirstInteraction = false;
    }
  }
  else {
    m_FirstInteraction = false;
  }
}
