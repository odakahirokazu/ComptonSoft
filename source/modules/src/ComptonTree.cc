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

#include "ComptonTree.hh"

#include "TDirectory.h"
#include "TTree.h"

#include "EventReconstruction.hh"
#include "InitialInformation.hh"

using namespace comptonsoft;
using namespace anl;


ComptonTree::ComptonTree()
  : EventReconstruction_ptr(0), initial_info(0), comptree(0),
    detector_detail(false),
    polarization_enable(false),
    eventid(0),
    h1time(0.0), h1process(0), h1detid(0), h1edep(0.0), h1posx(0.0), h1posy(0.0), h1posz(0.0),
    h2time(0.0), h2process(0), h2detid(0), h2edep(0.0), h2posx(0.0), h2posy(0.0), h2posz(0.0),
    costheta_kin(0.0), dtheta(0.0), hitpattern(0), flag(0),
    ini_energy(0.0), ini_dirx(0.0),  ini_diry(0.0), ini_dirz(0.0), ini_time(0.0), ini_posx(0.0),  ini_posy(0.0), ini_posz(0.0),
    ini_polarx(0.0), ini_polary(0.0), ini_polarz(0.0),
    weight(1.0)
{
}


ANLStatus ComptonTree::mod_startup()
{
  register_parameter(&detector_detail, "Save detector detail?");

  return AS_OK;
}


ANLStatus ComptonTree::mod_his()
{
  VCSModule::mod_his();

  TTree::SetMaxTreeSize(4000000000);
  comptree = new TTree("cetree", "cetree");
  comptree->Branch("eventid",      &eventid,      "eventid/I");
  comptree->Branch("h1time",       &h1time,       "h1time/D");
  comptree->Branch("h1process",    &h1process,    "h1process/I");
  comptree->Branch("h1detid",      &h1detid,      "h1detid/I");
  comptree->Branch("h1edep",       &h1edep,       "h1edep/D");
  comptree->Branch("h1posx",       &h1posx,       "h1posx/D");
  comptree->Branch("h1posy",       &h1posy,       "h1posy/D");
  comptree->Branch("h1posz",       &h1posz,       "h1posz/D");
  comptree->Branch("h2time",       &h2time,       "h2time2/D");
  comptree->Branch("h2process",    &h2process,    "h2process2/I");
  comptree->Branch("h2detid",      &h2detid,      "h2detid2/I");
  comptree->Branch("h2edep",       &h2edep,       "h2edep2/D");
  comptree->Branch("h2posx",       &h2posx,       "h2posx2/D");
  comptree->Branch("h2posy",       &h2posy,       "h2posy2/D");
  comptree->Branch("h2posz",       &h2posz,       "h2posz2/D");
  comptree->Branch("costheta_kin", &costheta_kin, "costheta_kin/D");
  comptree->Branch("dtheta",       &dtheta,       "dtheta/D");
  comptree->Branch("hitpattern",   &hitpattern,   "hitpattern/i");
  comptree->Branch("flag",         &flag,         "flag/i");
  
  comptree->Branch("ini_energy",   &ini_energy,   "ini_energy/D");
  comptree->Branch("ini_dirx",     &ini_dirx,     "ini_dirx/D");
  comptree->Branch("ini_diry",     &ini_diry,     "ini_diry/D");
  comptree->Branch("ini_dirz",     &ini_dirz,     "ini_dirz/D");
  comptree->Branch("ini_time",     &ini_time,     "ini_time/D");
  comptree->Branch("ini_posx",     &ini_posx,     "ini_posx/D");
  comptree->Branch("ini_posy",     &ini_posy,     "ini_posy/D");
  comptree->Branch("ini_posz",     &ini_posz,     "ini_posz/D");

  if (Evs("Polarization enable")) {
    polarization_enable = true;
    comptree->Branch("ini_polarx", &ini_polarx,  "ini_polarx/D");
    comptree->Branch("ini_polary", &ini_polary,  "ini_polary/D");
    comptree->Branch("ini_polarz", &ini_polarz,  "ini_polarz/D");
  }

  comptree->Branch("weight",     &weight,     "weight/D");

  if (detector_detail) {
    comptree->Branch("h1stripx",     &h1stripx,     "h1stripx/I");
    comptree->Branch("h1stripy",     &h1stripy,     "h1stripy/I");
    comptree->Branch("h1chip",       &h1chip,       "h1chip/I");
    comptree->Branch("h1channel",    &h1channel,    "h1channel/I");
    comptree->Branch("h2stripx",     &h2stripx,     "h2stripx/I");
    comptree->Branch("h2stripy",     &h2stripy,     "h2stripy/I");
    comptree->Branch("h2chip",       &h2chip,       "h2chip/I");
    comptree->Branch("h2channel",    &h2channel,    "h2channel/I");
  }

  return AS_OK;
}


ANLStatus ComptonTree::mod_init()
{
  GetANLModuleNC("EventReconstruction", &EventReconstruction_ptr);
  GetANLModuleIF("InitialInformation", &initial_info);
  
  return AS_OK;
}


ANLStatus ComptonTree::mod_ana()
{
  if (!Evs("EventReconst:Stored")) {
    return AS_OK;
  }

  ini_energy = initial_info->InitialEnergy();
  G4ThreeVector iniDir = initial_info->InitialDirection();
  ini_dirx = iniDir.x();
  ini_diry = iniDir.y();
  ini_dirz = iniDir.z();
  ini_time = initial_info->InitialTime();
  G4ThreeVector iniPos = initial_info->InitialPosition();
  ini_posx = iniPos.x();
  ini_posy = iniPos.y();
  ini_posz = iniPos.z();
  
  if (polarization_enable) {
    G4ThreeVector iniPolar = initial_info->InitialPolarization();
    ini_polarx = iniPolar.x();
    ini_polary = iniPolar.y();
    ini_polarz = iniPolar.z();
  }

  weight = initial_info->Weight();

  eventid = initial_info->EventID();
  //  BnkGet("EventReconst:Flag", sizeof(unsigned int), &usedsize, &flag);

  const TwoHitComptonEvent& compton_event = EventReconstruction_ptr->GetTwoHitData();

  h1time = compton_event.getH1Time();
  h1detid = compton_event.getH1DetId();
  h1process = compton_event.getH1Process();  
  h1edep = compton_event.getH1Energy();
  h1posx = compton_event.getH1PosX();
  h1posy = compton_event.getH1PosY();
  h1posz = compton_event.getH1PosZ();
  h2time = compton_event.getH2Time();
  h2detid = compton_event.getH2DetId();
  h2process = compton_event.getH2Process();  
  h2edep = compton_event.getH2Energy();
  h2posx = compton_event.getH2PosX();
  h2posy = compton_event.getH2PosY();
  h2posz = compton_event.getH2PosZ();
  costheta_kin = compton_event.CosThetaE();
  dtheta = compton_event.DeltaTheta();
  flag = compton_event.getFlag();

  if (detector_detail) {
    h1stripx = compton_event.getH1StripX();
    h1stripy = compton_event.getH1StripY();
    h1chip = compton_event.getH1Chip();
    h1channel = compton_event.getH1Channel();
    h2stripx = compton_event.getH2StripX();
    h2stripy = compton_event.getH2StripY();
    h2chip = compton_event.getH2Chip();
    h2channel = compton_event.getH2Channel();
  }
  
  comptree->Fill();
  
  return AS_OK;
}
