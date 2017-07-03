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
#include "G4VProcess.hh"
#include "TTree.h"
#include "TDirectory.h"
#include "SaveData.hh"

using namespace anl;

namespace comptonsoft
{

ScatteringPickUpData::ScatteringPickUpData()
  : processName_("compt")
{
}

ANLStatus ScatteringPickUpData::mod_startup()
{
  register_parameter(&processName_, "process_name");
  return AS_OK;
}

ANLStatus ScatteringPickUpData::mod_init()
{
  if (ModuleExist("SaveData")) {
    SaveData* saveModule;
    GetModuleNC("SaveData", &saveModule);
    saveModule->GetDirectory()->cd();
  }
  
  tree_ = new TTree("stree", "Scattering tree");
  tree_->Branch("dirx", &dirx_, "dirx/D");
  tree_->Branch("diry", &diry_, "diry/D");
  tree_->Branch("dirz", &dirz_, "dirz/D");
  tree_->Branch("energy", &energy_, "energy/D");
  
  return AS_OK;
}

void ScatteringPickUpData::EventActionAtBeginning(const G4Event*)
{
  firstInteraction_ = true;
}

void ScatteringPickUpData::SteppingAction(const G4Step* aStep)
{
  if (!firstInteraction_) return;

  const G4Track* aTrack = aStep->GetTrack();
  if (aTrack->GetTrackID()==1) {
    const G4StepPoint* sp = aStep->GetPostStepPoint();
    const std::string processName = sp->GetProcessDefinedStep()->GetProcessName();
    if (processName == processName_) {
      const G4ThreeVector direction = sp->GetMomentumDirection();
      dirx_ = direction.x();
      diry_ = direction.y();
      dirz_ = direction.z();
      energy_ = sp->GetKineticEnergy();
      tree_->Fill();
      firstInteraction_ = false;
    }
  }
  else {
    firstInteraction_ = false;
  }
}

} /* namespace comptonsoft */
