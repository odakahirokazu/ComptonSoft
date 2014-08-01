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

#include "EventSelection.hh"

#include <fstream>
#include <cmath>
#include <algorithm>
#include "globals.hh"
#include "DetectorHit.hh"
#include "NextCLI.hh"


using namespace anl;
using namespace comptonsoft;

EventSelection::EventSelection()
  : hit_collection(0),
    filename("detector_group.txt"),
    remove_veto_event(true)
{
}


ANLStatus EventSelection::mod_startup()
{
  fluor_cdte[0] = 0.0229571 * MeV;
  fluor_cdte[1] = 0.0231526 * MeV;
  fluor_cdte[2] = 0.0260402 * MeV;
  fluor_cdte[3] = 0.0260754 * MeV;
  fluor_cdte[4] = 0.0266158 * MeV;
  fluor_cdte[5] = 0.0271868 * MeV;
  fluor_cdte[6] = 0.0274653 * MeV;
  fluor_cdte[7] = 0.0309379 * MeV;
  fluor_cdte[8] = 0.0309901 * MeV;
  fluor_cdte[9] = 0.0316841 * MeV;
  fluor_range = 0.5 * keV;

  register_parameter(&filename, "detector_group",
                     "", "Detector group definition file (0 for no group)");
  register_parameter(&remove_veto_event, "apply_veto");
  register_parameter(&fluor_range, "tolerance_for_fluorescence_lines", keV, "keV"); 

  return AS_OK;
}


ANLStatus EventSelection::mod_init()
{
  //  VCSModule::mod_init();

  GetANLModuleNC("CSHitCollection", &hit_collection);

  if (filename[0] == '0') {
    std::cout << "EventSelection: There is no detector group." << std::endl;
    return AS_OK;
  }

  std::ifstream fin;
  fin.open(filename.c_str());
  if (!fin) {
    std::cout << "EventSelection: file cannot open" << std::endl;
    return AS_QUIT;
  }

  DetectorGroup* dg = 0;

  while (1) {
    int detid;
    std::string str;
    char buf[256];
    fin.getline(buf, 256);
    if(fin.eof()) { break; }
    if (buf[0] == '#') { continue; }

    std::istringstream iss(buf);
    iss >> str;
    
    if (str == "GROUP") {
      iss >> str;
      if (str == "Si") {
        dg = &si_detector_group;
        dg->setName(str);
      }
      else if (str == "CdTe") {
        dg = &cdte_detector_group;
        dg->setName(str);
      }
      else if (str == "Anti") {
        dg = &anti_detector_group;
        dg->setName(str);
      }
      else {
        dg = 0;
      }
    }
    else if (str == "ID") {
      while (iss >> detid) {
        if (dg) { dg->add(detid); }
      }
    }
  }
  
  si_detector_group.print();
  cdte_detector_group.print();
  anti_detector_group.print();

  EvsDef("EventSelection:Veto");
  
  return AS_OK;
}


ANLStatus EventSelection::mod_ana()
{
  int numTimeGroup = hit_collection->NumTimeGroup();
  for (int tgroup=comptonsoft::NOTIMEGROUP; tgroup<numTimeGroup; tgroup++) {
    std::vector<DetectorHit_sptr>& hitVec
      = hit_collection->GetHits(tgroup);

    bool veto = false;
    bool si_trig = false;
    bool cdte_trig = false;

    std::vector<DetectorHit_sptr>::iterator it;

    for (it=hitVec.begin(); it!=hitVec.end(); it++) {
      (*it)->clearFlag(comptonsoft::BGO_HIT|comptonsoft::SI_HIT|comptonsoft::CDTE_HIT);
      int detid = (*it)->getDetectorID();
      if (anti_detector_group.isMember(detid)) {
        veto = true;
        (*it)->addFlag(comptonsoft::BGO_HIT);
        EvsSet("EventSelection:Veto");
      }
      else if (si_detector_group.isMember(detid)) {
        si_trig = true;
        (*it)->addFlag(comptonsoft::SI_HIT);
      }
      else if (cdte_detector_group.isMember(detid)) {
        cdte_trig = true;
        (*it)->addFlag(comptonsoft::CDTE_HIT);
      }

      (*it)->clearFlag(comptonsoft::CDTE_FLUOR);
      double e_pi = (*it)->getPI();
      int fl_index;
      double fl_energy;
      double fl_delta_energy = check_fluor_cdte(e_pi, fl_index, fl_energy);
      if (fl_delta_energy < fluor_range) {
        (*it)->addFlag(comptonsoft::CDTE_FLUOR);
      }
    }

    if (remove_veto_event && veto) {
      hit_collection->ClearHits(tgroup);
    }
    else {
      for (it=hitVec.begin(); it!=hitVec.end(); it++) {
        (*it)->clearFlag(comptonsoft::BGO_VETO|comptonsoft::SI_TRIGGER|comptonsoft::CDTE_TRIGGER);
        if (veto) (*it)->addFlag(comptonsoft::BGO_VETO);
        if (si_trig) (*it)->addFlag(comptonsoft::SI_TRIGGER);
        if (cdte_trig) (*it)->addFlag(comptonsoft::CDTE_TRIGGER);
      }
    }
  }

  return AS_OK;
}


// from simHXISGD by S. Watanabe
// modifed by H. Odaka (2012-07-01)
double EventSelection::check_fluor_cdte(double ene, int& index, double& fluorene)
{
  double delta_e;
  // index = TMath::BinarySearch((Long64_t)NUM_OF_FLUOR_CDTE, fluor_cdte, ene);
  double* p = std::lower_bound(fluor_cdte, fluor_cdte+NUM_OF_FLUOR_CDTE, ene);
  index = p-1-fluor_cdte;
  
  if(index == -1) {
    index = 0;
    fluorene = fluor_cdte[index];
    delta_e = std::abs(ene - fluorene);
  }
  else if(index == NUM_OF_FLUOR_CDTE-1) {
    fluorene = fluor_cdte[index];
    delta_e = std::abs(ene - fluorene);   
  }
  else {
    fluorene = fluor_cdte[index];
    delta_e = std::abs(ene - fluorene);
    if( delta_e > std::abs(ene - fluor_cdte[index+1]) ) {
      index = index + 1;
      fluorene = fluor_cdte[index];
      delta_e = std::abs(ene - fluorene);   
    }
  }
  
  return delta_e;
}
