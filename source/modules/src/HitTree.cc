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

#include "HitTree.hh"
#include "DetectorHit.hh"
#include "ReadDataFile.hh"
#include "CSHitCollection.hh"

using namespace comptonsoft;
using namespace anl;

HitTree::HitTree()
  : hit_collection(0), m_ReadDataModule(0), hittree(0),
    eventid(0), seqnum(0), totalhit(0),
    ini_energy(0.0),
    realposx(0.0), realposy(0.0), realposz(0.0),
    localposx(0.0), localposy(0.0), localposz(0.0),
    posx(0.0), posy(0.0), posz(0.0),
    edep(0.0), e_pha(0.0), e_pi(0.0),
    time1(0.0),
    process(0), grade(0),
    detid(-1), stripx(-1), stripy(-1),
    chip(-1), channel(-1),
    time_group(0), flag(0)
{
}


ANLStatus HitTree::mod_his()
{
  VCSModule::mod_his();

  hittree = new TTree("hittree", "hittree");
  
  hittree->Branch("eventid",    &eventid,     "eventid/I");
  hittree->Branch("seqnum",     &seqnum,      "seqnum/I");
  hittree->Branch("totalhit",   &totalhit,    "totalhit/I");

  hittree->Branch("realposx",   &realposx,    "realposx/D");
  hittree->Branch("realposy",   &realposy,    "realposy/D");
  hittree->Branch("realposz",   &realposz,    "realposz/D");
  hittree->Branch("localposx",  &localposx,   "localposx/D");
  hittree->Branch("localposy",  &localposy,   "localposy/D");
  hittree->Branch("localposz",  &localposz,   "localposz/D");
  hittree->Branch("posx",       &posx,        "posx/D");
  hittree->Branch("posy",       &posy,        "posy/D");
  hittree->Branch("posz",       &posz,        "posz/D");

  hittree->Branch("edep",       &edep,        "edep/D");
  hittree->Branch("e_pha",      &e_pha,       "e_pha/D");
  hittree->Branch("e_pi",       &e_pi,        "e_pi/D");
  
  hittree->Branch("time",       &time1,       "time/D");
  hittree->Branch("process",    &process,     "process/i");
  hittree->Branch("grade",      &grade,       "grade/I");

  hittree->Branch("detid",      &detid,       "detid/I");
  hittree->Branch("stripx",     &stripx,      "stripx/I");
  hittree->Branch("stripy",     &stripy,      "stripy/I");
  hittree->Branch("chip",       &chip,        "chip/I");
  hittree->Branch("channel",    &channel,     "channel/I");
  
  hittree->Branch("time_group", &time_group,  "time_group/I");

  hittree->Branch("flag",       &flag,        "flag/i");

  hittree->Branch("ini_energy", &ini_energy,  "ini_ene/D");

  return AS_OK;
}


ANLStatus HitTree::mod_init()
{
  VCSModule::mod_init();

  GetANLModuleNC("CSHitCollection", &hit_collection);
  GetANLModuleNC("ReadDataFile", &m_ReadDataModule);
  EvsDef("HitTree:Fill");
  
  return AS_OK;
}


ANLStatus HitTree::mod_ana()
{
  const std::vector<DetectorHit_sptr>& hitVec 
    = hit_collection->GetHits();
  int num = hitVec.size();
  seqnum = 0;
  totalhit = num;

  time1 = m_ReadDataModule->Time();
  flag = 0;

  if (num>0) EvsSet("HitTree:Fill");

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

  eventid++;

  return AS_OK;
}
