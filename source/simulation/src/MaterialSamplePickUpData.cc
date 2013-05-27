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

#include "MaterialSamplePickUpData.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"

#include "G4TwoVector.hh"
#include "G4ProcessTable.hh"
#include "G4ParticleTable.hh"
#include "G4PhotoElectricEffect.hh"

#include "SaveData.hh"
#include "BasicPrimaryGen.hh"
#include "InitialInformation.hh"

using namespace anl;
using namespace comptonsoft;


MaterialSamplePickUpData::MaterialSamplePickUpData()
  : masstree(0),
    ini_energy(0.0),
    ini_dirx(0.0), ini_diry(0.0), ini_dirz(0.0), 
    ini_posx(0.0), ini_posy(0.0), ini_posz(0.0), 
    primary(0), mass(0.0),
    tau1(0.0), tau2(0.0), tau3(0.0), tau4(0.0), tau5(0.0),
    m_PhAbsProcess(0)
{
  SetStepActOn(true);
}


ANLStatus MaterialSamplePickUpData::mod_startup()
{
  return AS_OK;
}


ANLStatus MaterialSamplePickUpData::mod_init()
{
  GetANLModuleIF("InitialInformation", &primary);

#if 0
   massfile = new TFile(filename.c_str(), "recreate");
  
  if ( !massfile ) {
    std::cout << "cannot create ROOT file" << std::endl;
    return AS_QUIT;
  }
#endif
  
  SaveData* save_data = GetANLModuleNC<SaveData>("SaveData");
  save_data->GetDirectory()->cd();
  masstree = new TTree("masstree", "masstree");
  
  masstree->Branch("ini_energy", &ini_energy,  "ini_energy/D");
  masstree->Branch("ini_dirx", &ini_dirx,  "ini_dirx/D");
  masstree->Branch("ini_diry", &ini_diry,  "ini_diry/D");
  masstree->Branch("ini_dirz", &ini_dirz,  "ini_dirz/D");
  masstree->Branch("ini_posx", &ini_posx,  "ini_posx/D");
  masstree->Branch("ini_posy", &ini_posy,  "ini_posy/D");
  masstree->Branch("ini_posz", &ini_posz,  "ini_posz/D");
  masstree->Branch("mass", &mass, "mass/D");
  masstree->Branch("tau_5keV", &tau1, "tau_5keV/D");
  masstree->Branch("tau_10keV", &tau2, "tau_10keV/D");
  masstree->Branch("tau_20keV", &tau3, "tau_20keV/D");
  masstree->Branch("tau_40keV", &tau4, "tau_40keV/D");
  masstree->Branch("tau_80keV", &tau5, "tau_80keV/D");

  G4VProcess* process = G4ProcessTable::GetProcessTable()->FindProcess("phot", "gamma");
  m_PhAbsProcess = static_cast<G4PhotoElectricEffect*>(process);
  
  return AS_OK;
}


ANLStatus MaterialSamplePickUpData::mod_ana()
{
  ini_energy = primary->InitialEnergy();
  G4ThreeVector iniDir = primary->InitialDirection();
  ini_dirx = iniDir.x();
  ini_diry = iniDir.y();
  ini_dirz = iniDir.z();
  G4ThreeVector iniPos = primary->InitialPosition();
  ini_posx = iniPos.x();
  ini_posy = iniPos.y();
  ini_posz = iniPos.z();

  masstree->Fill();
  
  mass = 0.;
  tau1 = 0.;
  tau2 = 0.;
  tau3 = 0.;
  tau4 = 0.;
  tau5 = 0.;
  
  return AS_OK;
}


void MaterialSamplePickUpData::StepAct(const G4Step* aStep, G4Track* aTrack)
{
  //G4String VolumeName = aTrack->GetVolume()->GetName();
  G4double steplength = aStep->GetStepLength();
  
  G4Material* mat = aTrack->GetMaterial();
  G4double density = mat->GetDensity();
  
  mass += (steplength/cm) * (density/(g/cm3));
  
  const G4MaterialCutsCouple* mcc = aTrack->GetMaterialCutsCouple();
  G4double ene = 5.0*keV;
  tau1 += steplength * m_PhAbsProcess->GetLambda(ene, mcc);
  ene = 10.0*keV;
  tau2 += steplength * m_PhAbsProcess->GetLambda(ene, mcc);
  ene = 20.0*keV;
  tau3 += steplength * m_PhAbsProcess->GetLambda(ene, mcc);
  ene = 40.0*keV;
  tau4 += steplength * m_PhAbsProcess->GetLambda(ene, mcc);
  ene = 80.0*keV;
  tau5 += steplength * m_PhAbsProcess->GetLambda(ene, mcc);
}
