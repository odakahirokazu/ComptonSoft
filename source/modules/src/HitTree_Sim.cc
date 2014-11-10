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

#include "HitTree_Sim.hh"
#include <iostream>
#include "DetectorHit.hh"
#include "InitialInformation.hh"
#include "CSHitCollection.hh"

using namespace comptonsoft;
using namespace anl;


HitTree_Sim::HitTree_Sim()
  : ini_dirx(0.0), ini_diry(0.0), ini_dirz(0.0),
    ini_time(0.0), ini_posx(0.0), ini_posy(0.0), ini_posz(0.0),
    ini_polarx(0.0), ini_polary(0.0), ini_polarz(0.0),
    weight(1.0),
    initial_info(0), polarization_enable(false)
{
}


ANLStatus HitTree_Sim::mod_init()
{
  VCSModule::mod_init();

  GetANLModuleNC("CSHitCollection", &hit_collection);
  GetANLModuleIF("InitialInformation", &initial_info);

  EvsDef("HitTree:Fill");
  
  return AS_OK;
}


ANLStatus HitTree_Sim::mod_his()
{
  HitTree::mod_his();
  hittree->Branch("ini_dirx", &ini_dirx,  "ini_dirx/D");
  hittree->Branch("ini_diry", &ini_diry,  "ini_diry/D");
  hittree->Branch("ini_dirz", &ini_dirz,  "ini_dirz/D");
  hittree->Branch("ini_time", &ini_time,  "ini_time/D");
  hittree->Branch("ini_posx", &ini_posx,  "ini_posx/D");
  hittree->Branch("ini_posy", &ini_posy,  "ini_posy/D");
  hittree->Branch("ini_posz", &ini_posz,  "ini_posz/D");
  hittree->Branch("weight", &weight,  "weight/D");

  if (Evs("Polarization enable")) {
    polarization_enable = true;
    hittree->Branch("ini_polarx", &ini_polarx,  "ini_polarx/D");
    hittree->Branch("ini_polary", &ini_polary,  "ini_polary/D");
    hittree->Branch("ini_polarz", &ini_polarz,  "ini_polarz/D");
  }
  
  return AS_OK;
}


ANLStatus HitTree_Sim::mod_ana()
{
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
  flag = 0;

  int numTimeGroup = hit_collection->NumTimeGroup();
  for (int tGroup=comptonsoft::NOTIMEGROUP; tGroup<numTimeGroup; tGroup++) {
    const std::vector<DetectorHit_sptr>& hitVec
      = hit_collection->GetHits(tGroup);
    
    int num = hitVec.size();

    if (num>0) { EvsSet("HitTree:Fill"); }

    seqnum = 0;
    totalhit = num;

    flag = 0;

    for (int i=0; i<num; i++) {
      flag = 0;
      
      realposx = hitVec[i]->getRealPosX();
      realposy = hitVec[i]->getRealPosY();
      realposz = hitVec[i]->getRealPosZ();
      localposx = hitVec[i]->getLocalPosX();
      localposy = hitVec[i]->getLocalPosY();
      localposz = hitVec[i]->getLocalPosZ();
      posx = hitVec[i]->getPosX();
      posy = hitVec[i]->getPosY();
      posz = hitVec[i]->getPosZ();
      
      edep  = hitVec[i]->getEdep();
      e_pha = hitVec[i]->getPHA();
      e_pi  = hitVec[i]->getPI();
      
      time1 = hitVec[i]->getTime();
      
      process = hitVec[i]->getProcess();
      grade = hitVec[i]->getGrade();
      
      detid = hitVec[i]->getDetectorID();
      stripx = hitVec[i]->getStripX();
      stripy = hitVec[i]->getStripY();
      chip = hitVec[i]->getChipID();
      channel = hitVec[i]->getChannel();
      time_group = hitVec[i]->getTimeGroup();
      
      flag |= hitVec[i]->getFlag();
      
      hittree->Fill();
      
      seqnum++;
    }
  }

  return AS_OK;
}
