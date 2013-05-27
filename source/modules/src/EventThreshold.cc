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

#include "EventThreshold.hh"

#include <fstream>
#include "TMath.h"
#include "globals.hh"

#include "NextCLI.hh"
#include "DetectorHit.hh"


using namespace comptonsoft;
using namespace anl;

EventThreshold::EventThreshold()
  : hit_collection(0), dg_filename("detector_group.txt")
{
}


ANLStatus EventThreshold::mod_startup()
{
  register_parameter(&dg_filename, "Detector group file");
 
  return AS_OK;
}


ANLStatus EventThreshold::mod_com()
{
  std::string name("DSSD");
  std::cout << "Definition of event threshold : " << std::endl;

  while (1) {
    CLread("detector group name (OK for exit)", &name);
    if (name == "OK" or name == "ok") { break; }

    double threshold = 10.0*keV;
    CLread("Threshold energy", &threshold, keV, "keV");
    threshold_vector.push_back(std::make_pair(name, threshold));

    name = "OK";
  }
  
  return AS_OK;
}


ANLStatus EventThreshold::mod_init()
{
  GetANLModuleNC("CSHitCollection", &hit_collection);
  
  if (dg_filename[0] == '0') {
    std::cout << "EventThreshold: There is no detector group." << std::endl;
    return AS_OK;
  }

  if (!load_detector_groups()) {
    return AS_QUIT;
  }

  return AS_OK;
}


ANLStatus EventThreshold::mod_ana()
{
  int numTimeGroup = hit_collection->NumTimeGroup();
  for (int tGroup=comptonsoft::NOTIMEGROUP; tGroup<numTimeGroup; tGroup++) {
    std::vector<DetectorHit_sptr>& hitVec
      = hit_collection->GetHits(tGroup);
    
    std::vector<DetectorHit_sptr>::iterator hit = hitVec.begin();
    while (hit!=hitVec.end()) {
      double energy = (*hit)->getPI();
      double detid = (*hit)->getDetectorID();
      
      bool detected = true;
      for (size_t i=0; i<threshold_vector.size(); ++i) {
        const DetectorGroup* dg = get_detector_group(threshold_vector[i].first);
        double threshold = threshold_vector[i].second;
        if (dg->isMember(detid) && energy < threshold) {
          detected = false;
          // std::cout << detid << " " << energy/keV << " keV" << std::endl;
          break;
        }
      }
      
      if (detected) {
        ++hit;
      }
      else {
        hit = hitVec.erase(hit);
      }
    }
  }
  
  return AS_OK;
}


ANLStatus EventThreshold::mod_exit()
{
  return AS_OK;
}


const DetectorGroup* EventThreshold::get_detector_group(const std::string& name)
{
  for (unsigned int i=0; i<detector_groups.size(); i++) {
    if (name == detector_groups[i].Name()) {
      return &(detector_groups[i]);
    }
  }

  return 0;
}


bool EventThreshold::load_detector_groups()
{
  if (dg_filename[0] == '0') {
    std::cout << "EventThreshold: There is no detector group." << std::endl;
    return true;
  }

  std::ifstream fin;
  fin.open(dg_filename.c_str());
  if (!fin) {
    std::cout << "EventThreshold: file cannot open" << std::endl;
    return false;
  }

  DetectorGroup* dg = 0;

  bool match = false;

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

      match = false;
      size_t n = threshold_vector.size();
      for (size_t i=0; i<n; ++i) {
        if (threshold_vector[i].first == str) {
          match = true;
          break;
        }
      }

      if (match) {
        int vecsize = detector_groups.size();
        detector_groups.resize(vecsize+1);
        dg = &detector_groups[vecsize];
        dg->setName(str);
      }
    }
    else if (str == "ID") {
      while (iss >> detid) {
        if (match) {
          if (dg) { dg->add(detid); }
        }
      }
    }
  }
  
  for (size_t i=0; i<detector_groups.size(); i++) {
    detector_groups[i].print();
  }
  
  return true;
}
