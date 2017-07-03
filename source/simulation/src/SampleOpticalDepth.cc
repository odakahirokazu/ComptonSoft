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

#include "SampleOpticalDepth.hh"

#include "TTree.h"
#include "TDirectory.h"

#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "G4ProcessTable.hh"
#include "G4VEmProcess.hh"

#include "SaveData.hh"
#include "InitialInformation.hh"

using namespace anl;

namespace comptonsoft
{

SampleOpticalDepth::SampleOpticalDepth()
  : energy_(60.0*keV), processName_("phot"), particleName_("gamma")
{
}

ANLStatus SampleOpticalDepth::mod_startup()
{
  register_parameter(&energy_, "energy", keV, "keV");
  register_parameter(&processName_, "process");
  register_parameter(&particleName_, "particle");
  return AS_OK;
}

ANLStatus SampleOpticalDepth::mod_init()
{
  GetModuleIF("InitialInformation", &initialInfo_);

  if (ModuleExist("SaveData")) {
    SaveData* saveModule;
    GetModuleNC("SaveData", &saveModule);
    saveModule->GetDirectory()->cd();
  }

  tree_ = new TTree("odtree", "Optical depth tree");
  tree_->Branch("ini_posx", &ini_posx_, "ini_posx/D");
  tree_->Branch("ini_posy", &ini_posy_, "ini_posy/D");
  tree_->Branch("ini_posz", &ini_posz_, "ini_posz/D");
  tree_->Branch("ini_dirx", &ini_dirx_, "ini_dirx/D");
  tree_->Branch("ini_diry", &ini_diry_, "ini_diry/D");
  tree_->Branch("ini_dirz", &ini_dirz_, "ini_dirz/D");
  tree_->Branch("length",   &length_,   "length/D");
  tree_->Branch("tau",      &tau_,      "tau/D");

  G4VProcess* process_base = G4ProcessTable::GetProcessTable()->FindProcess(processName_, particleName_);
  if (process_base == nullptr) {
    std::cout << "Process " << processName_ << " for " << particleName_ << " is not found." << std::endl;
    return AS_QUIT_ERR;
  }

  process_ = dynamic_cast<G4VEmProcess*>(process_base);
  if (process_ == nullptr) {
    std::cout << "Process " << processName_ << " is not an EM process." << std::endl;
    return AS_QUIT_ERR;
  }
  
  return AS_OK;
}

void SampleOpticalDepth::EventActionAtBeginning(const G4Event*)
{
  ini_posx_ = 0.0;
  ini_posy_ = 0.0;
  ini_posz_ = 0.0;
  ini_dirx_ = 0.0;
  ini_diry_ = 0.0;
  ini_dirz_ = 0.0;
  length_ = 0.0;
  tau_ = 0.0;
}

void SampleOpticalDepth::EventActionAtEnd(const G4Event*)
{
  const G4ThreeVector iniPos = initialInfo_->InitialPosition();
  ini_posx_ = iniPos.x();
  ini_posy_ = iniPos.y();
  ini_posz_ = iniPos.z();
  const G4ThreeVector iniDir = initialInfo_->InitialDirection();
  ini_dirx_ = iniDir.x();
  ini_diry_ = iniDir.y();
  ini_dirz_ = iniDir.z();

  tree_->Fill();
}

void SampleOpticalDepth::SteppingAction(const G4Step* aStep)
{
  const G4Track* aTrack = aStep->GetTrack();
  const G4MaterialCutsCouple* mcc = aTrack->GetMaterialCutsCouple();
  const double stepLength = aStep->GetStepLength();
  const double deltaTau = stepLength * process_->GetLambda(energy_, mcc);

  length_ += stepLength;
  tau_ += deltaTau;
}

} /* namespace comptonsoft */
