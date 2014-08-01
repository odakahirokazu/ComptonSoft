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

// 2008-12-15 Hirokazu Odaka

#include "ReadComptonTree.hh"
#include "NextCLI.hh"


using namespace comptonsoft;
using namespace anl;

ReadComptonTree::ReadComptonTree()
  : InitialInformation(false, this),
    cetree(0), NEvent(0),
    eventid(0),
    h1time(0.0), h1process(0), h1detid(0), h1edep(0.0), h1posx(0.0), h1posy(0.0), h1posz(0.0),
    h2time(0.0), h2process(0), h2detid(0), h2edep(0.0), h2posx(0.0), h2posy(0.0), h2posz(0.0),
    costheta_kin(0.0),
    dtheta(0.0), hitpattern(0), flag(0),
    ini_energy(0.0), ini_dirx(0.0),  ini_diry(0.0), ini_dirz(0.0), ini_time(0.0), ini_posx(0.0),  ini_posy(0.0), ini_posz(0.0), ini_polarx(0.0), ini_polary(0.0), ini_polarz(0.0), weight(1.0)
{
}


ANLStatus ReadComptonTree::mod_startup()
{
  EventReconstruction::mod_startup();
  register_parameter(&filename, "file_list", "seq", "ce.root");
  
  return AS_OK;
}


ANLStatus ReadComptonTree::mod_his()
{
  cetree = new TChain("cetree");
  for(int i=0; i<(int)filename.size(); i++) {
    cetree->Add(filename[i].c_str());
  }
  
  NEvent = cetree->GetEntries();
  std::cout << "Total Event : " << NEvent << std::endl;

  cetree->SetBranchAddress("eventid",      &eventid);
  cetree->SetBranchAddress("h1time",       &h1time);
  cetree->SetBranchAddress("h1process",    &h1process);
  cetree->SetBranchAddress("h1detid",      &h1detid);
  if (cetree->GetBranch("h1stripx"))  cetree->SetBranchAddress("h1stripx",   &h1stripx);
  if (cetree->GetBranch("h1stripy"))  cetree->SetBranchAddress("h1stripy",   &h1stripy);
  if (cetree->GetBranch("h1chip"))    cetree->SetBranchAddress("h1chip",   &h1chip);
  if (cetree->GetBranch("h1channel")) cetree->SetBranchAddress("h1channel",   &h1channel);
  cetree->SetBranchAddress("h1edep",       &h1edep);
  cetree->SetBranchAddress("h1posx",       &h1posx);
  cetree->SetBranchAddress("h1posy",       &h1posy);
  cetree->SetBranchAddress("h1posz",       &h1posz);
  cetree->SetBranchAddress("h2time",       &h2time);
  cetree->SetBranchAddress("h2process",    &h2process);
  cetree->SetBranchAddress("h2detid",      &h2detid);
  if (cetree->GetBranch("h2stripx"))  cetree->SetBranchAddress("h2stripx",   &h2stripx);
  if (cetree->GetBranch("h2stripy"))  cetree->SetBranchAddress("h2stripy",   &h2stripy);
  if (cetree->GetBranch("h2chip"))    cetree->SetBranchAddress("h2chip",   &h2chip);
  if (cetree->GetBranch("h2channel")) cetree->SetBranchAddress("h2channel",   &h2channel);
  cetree->SetBranchAddress("h2edep",       &h2edep);
  cetree->SetBranchAddress("h2posx",       &h2posx);
  cetree->SetBranchAddress("h2posy",       &h2posy);
  cetree->SetBranchAddress("h2posz",       &h2posz);
  cetree->SetBranchAddress("costheta_kin", &costheta_kin);
  cetree->SetBranchAddress("dtheta",       &dtheta);
  cetree->SetBranchAddress("hitpattern",   &hitpattern);
  cetree->SetBranchAddress("flag",         &flag);

  if (cetree->GetBranch("ini_energy")) cetree->SetBranchAddress("ini_energy", &ini_energy);
  if (cetree->GetBranch("ini_dirx")) {
    setInitialInformationStored();
    cetree->SetBranchAddress("ini_dirx", &ini_dirx);
  }
  if (cetree->GetBranch("ini_diry")) cetree->SetBranchAddress("ini_diry", &ini_diry);
  if (cetree->GetBranch("ini_dirz")) cetree->SetBranchAddress("ini_dirz", &ini_dirz);
  if (cetree->GetBranch("ini_posx")) cetree->SetBranchAddress("ini_posx", &ini_posx);
  if (cetree->GetBranch("ini_posy")) cetree->SetBranchAddress("ini_posy", &ini_posy);
  if (cetree->GetBranch("ini_posz")) cetree->SetBranchAddress("ini_posz", &ini_posz);
  if (cetree->GetBranch("ini_polarx")) cetree->SetBranchAddress("ini_polarx", &ini_polarx);
  if (cetree->GetBranch("ini_polary")) cetree->SetBranchAddress("ini_polary", &ini_polary);
  if (cetree->GetBranch("ini_polarz")) cetree->SetBranchAddress("ini_polarz", &ini_polarz);
  if (cetree->GetBranch("weight")) {
    setWeightStored();
    cetree->SetBranchAddress("weight", &weight);
  }
  
  return AS_OK;
}


ANLStatus ReadComptonTree::mod_ana()
{
  static unsigned int i = 0;
  if(i == NEvent) {
    return AS_QUIT;
  }
  
  cetree->GetEntry(i);
  
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

  TwoHitComptonEvent reconstructedEvent;
  reconstructedEvent.setH1Time(h1time);
  reconstructedEvent.setH1DetId(h1detid);
  reconstructedEvent.setH1StripX(h1stripx);
  reconstructedEvent.setH1StripY(h1stripy);
  reconstructedEvent.setH1Chip(h1chip);
  reconstructedEvent.setH1Channel(h1channel);
  reconstructedEvent.setH1Process(h1process);
  reconstructedEvent.setH1Energy(h1edep);
  reconstructedEvent.setH1Pos(h1posx,h1posy,h1posz);
  reconstructedEvent.setH2Time(h2time);
  reconstructedEvent.setH2DetId(h2detid);
  reconstructedEvent.setH2StripX(h2stripx);
  reconstructedEvent.setH2StripY(h2stripy);
  reconstructedEvent.setH2Chip(h2chip);
  reconstructedEvent.setH2Channel(h2channel);
  reconstructedEvent.setH2Process(h2process);
  reconstructedEvent.setH2Energy(h2edep);
  reconstructedEvent.setH2Pos(h2posx,h2posy,h2posz);
  reconstructedEvent.setFlag(flag);
  compton_event.setReconstructedEvent(reconstructedEvent);

  assign_source_information();
  
  if (flag & comptonsoft::CDTE_FLUOR) {
    EvsSet("EventReconst:CdTeFluor");
  }

  if (flag & comptonsoft::HI_ENERGY) {
    EvsSet("EventReconst:HighE");
  }
  
  if (flag & comptonsoft::BAD_TIME_ORDER) {
    EvsSet("EventReconst:BadTimeOrder");
  }

  std::vector<int> id_vec(2);
  id_vec[0] = h1detid;
  id_vec[1] = h2detid;
  determine_hit_pattern(id_vec);
   
  // BnkPut("EventReconst:Flag", sizeof(unsigned int), &flag);
  
  EvsSet("EventReconst:Stored");

  i++;
  return AS_OK;
}
