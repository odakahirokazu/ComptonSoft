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

#include "ReadRawHitTree.hh"

#include "DetectorHit.hh"
#include "DeviceSimulation.hh"
#include "NextCLI.hh"

using namespace comptonsoft;
using namespace anl;

ReadRawHitTree::ReadRawHitTree()
  : InitialInformation(false)
{
  add_alias("InitialInformation");
}


ANLStatus ReadRawHitTree::mod_startup()
{
  register_parameter(&inputfilename, "Hit tree file", "seq", "hittree.root");
  return AS_OK;
}


ANLStatus ReadRawHitTree::mod_his()
{
  hittree = new TChain("hittree");
  
  for(size_t i=0; i<inputfilename.size(); i++) {
    hittree->Add(inputfilename[i].c_str());
  }
  
  nevent = hittree->GetEntries();
  std::cout << "Total event: " << nevent << std::endl;

  hittree->SetBranchAddress("eventid",    &eventid);
  hittree->SetBranchAddress("seqnum",     &seqnum);
  hittree->SetBranchAddress("totalhit",   &totalhit);
  if (hittree->GetBranch("ini_energy")) hittree->SetBranchAddress("ini_energy", &ini_energy);
  if (hittree->GetBranch("ini_dirx")) {
    setInitialInformationStored();
    hittree->SetBranchAddress("ini_dirx", &ini_dirx);
  }
  if (hittree->GetBranch("ini_diry")) hittree->SetBranchAddress("ini_diry", &ini_diry);
  if (hittree->GetBranch("ini_dirz")) hittree->SetBranchAddress("ini_dirz", &ini_dirz);
  if (hittree->GetBranch("ini_time")) hittree->SetBranchAddress("ini_time", &ini_time);
  if (hittree->GetBranch("ini_posx")) hittree->SetBranchAddress("ini_posx", &ini_posx);
  if (hittree->GetBranch("ini_posy")) hittree->SetBranchAddress("ini_posy", &ini_posy);
  if (hittree->GetBranch("ini_posz")) hittree->SetBranchAddress("ini_posz", &ini_posz);
  if (hittree->GetBranch("ini_polarx")) hittree->SetBranchAddress("ini_polarx", &ini_polarx);
  if (hittree->GetBranch("ini_polary")) hittree->SetBranchAddress("ini_polary", &ini_polary);
  if (hittree->GetBranch("ini_polarz")) hittree->SetBranchAddress("ini_polarz", &ini_polarz);
  if (hittree->GetBranch("weight")) {
    setWeightStored();
    hittree->SetBranchAddress("weight", &weight);
  }
  hittree->SetBranchAddress("realposx",   &realposx);
  hittree->SetBranchAddress("realposy",   &realposy);
  hittree->SetBranchAddress("realposz",   &realposz);
  hittree->SetBranchAddress("localposx",  &localposx);
  hittree->SetBranchAddress("localposy",  &localposy);
  hittree->SetBranchAddress("localposz",  &localposz);
  hittree->SetBranchAddress("posx",       &posx);
  hittree->SetBranchAddress("posy",       &posy);
  hittree->SetBranchAddress("posz",       &posz);
  hittree->SetBranchAddress("edep",       &edep);
  hittree->SetBranchAddress("e_pha",      &e_pha);
  hittree->SetBranchAddress("e_pi",       &e_pi);
  hittree->SetBranchAddress("time",       &time);
  hittree->SetBranchAddress("process",    &process);
  hittree->SetBranchAddress("grade",      &grade);
  hittree->SetBranchAddress("detid",      &detid);
  if (hittree->GetBranch("stripx")) {
    hittree->SetBranchAddress("stripx",     &stripx);
    hittree->SetBranchAddress("stripy",     &stripy);
  }
  else {
    hittree->SetBranchAddress("xstrip",     &stripx);
    hittree->SetBranchAddress("ystrip",     &stripy);
  }
  hittree->SetBranchAddress("time_group", &time_group);
  hittree->SetBranchAddress("flag",       &flag);
  if (hittree->GetBranch("chip")) hittree->SetBranchAddress("chip", &chip);
  if (hittree->GetBranch("channel")) hittree->SetBranchAddress("channel", &channel);

  return AS_OK;
}


ANLStatus ReadRawHitTree::mod_ana()
{
  if (id == nevent) {
    return AS_QUIT;
  }

  hittree->GetEntry(id);
  int num = totalhit;
  setEventID(eventid);
  if (InitialInformationStored()) {
    setInitialEnergy(ini_energy);
    setInitialDirection(ini_dirx, ini_diry, ini_dirz);
    setInitialTime(ini_time);
    setInitialPosition(ini_posx, ini_posy, ini_posz);
    setInitialPolarization(ini_polarx, ini_polary, ini_polarz);
  }
  
  if (WeightStored()) {
    setWeight(weight);
  }
  
  for(int i=0; i<num; i++) {
    if (i != 0) { hittree->GetEntry(id+i); }
    
    DetectorHit_sptr hit(new DetectorHit);
  
    hit->setRealPosX(realposx);
    hit->setRealPosY(realposy);
    hit->setRealPosZ(realposz);
    hit->setLocalPosX(localposx);
    hit->setLocalPosY(localposy);
    hit->setLocalPosZ(localposz);
    hit->setPosX(posx);
    hit->setPosY(posy);
    hit->setPosZ(posz);
    hit->setEdep(edep);
    hit->setPHA(e_pha);
    hit->setPI(e_pi);
    hit->setTime(time);
    hit->setProcess(process);
    hit->setGrade(grade);
    hit->setDetectorID(detid);
    hit->setStripX(stripx);
    hit->setStripY(stripy);
    hit->setTimeGroup(time_group);
    hit->setFlag(flag);
    
    insertHit(detid, hit);
  }
  
  id += totalhit;

  return AS_OK;
}


void ReadRawHitTree::insertHit(int detid, DetectorHit_sptr hit)
{
  VDeviceSimulation* ds
    = GetDetectorManager()->getDeviceSimulationByID(detid);
  ds->insertRawHit(hit);
}
