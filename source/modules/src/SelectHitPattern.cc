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

#include "SelectHitPattern.hh"

#include <fstream>
#include "TMath.h"
#include "globals.hh"

#include "DetectorHit.hh"
#include "NextCLI.hh"

using namespace comptonsoft;
using namespace anl;


SelectHitPattern::SelectHitPattern()
  : hit_collection(0),
    dg_filename("detector_group.txt")
{
  add_alias("SelectHitPattern");
}


ANLStatus SelectHitPattern::mod_startup()
{
  register_parameter(&dg_filename, "Detector group file");
  
  return AS_OK;
}


ANLStatus SelectHitPattern::mod_prepare()
{
  if (dg_filename[0] == '0') {
    std::cout << "SelectHitPattern: There is no detector group." << std::endl;
    return AS_OK;
  }
  
  if (!load_detector_groups()) {
    return AS_QUIT;
  }
  
  return AS_OK;
}


ANLStatus SelectHitPattern::mod_init()
{
  GetANLModuleNC("CSHitCollection", &hit_collection);
  
  return AS_OK;
}


ANLStatus SelectHitPattern::mod_ana()
{
  const int numTimeGroup = hit_collection->NumTimeGroup();
  const int timeGroup = numTimeGroup - 1;
  const std::vector<DetectorHit_sptr>& hitVec
    = hit_collection->GetHits(timeGroup);
  determine_hit_pattern(hitVec);
  
  return AS_OK;
}


ANLStatus SelectHitPattern::mod_exit()
{
  std::cout << std::endl;
  std::cout << "Hit Pattern (SelectHitPattern)" << std::endl;
  for (size_t i=0; i<hit_pattern_vec.size(); i++) {
    std::cout << hit_pattern_vec[i].Name() << "   "
	      << hit_pattern_counts[i] << std::endl;
  }

  return AS_OK;
}


const DetectorGroup* SelectHitPattern::get_detector_group(const std::string& name)
{
  for (size_t i=0; i<detector_groups.size(); i++) {
    if (name == detector_groups[i].Name()) {
      return &(detector_groups[i]);
    }
  }

  return 0;
}


bool SelectHitPattern::load_detector_groups()
{
  if (dg_filename[0] == '0') {
    std::cout << "SelectHitPattern: There is no detector group." << std::endl;
    return true;
  }

  std::ifstream fin;
  fin.open(dg_filename.c_str());
  if (!fin) {
    std::cout << "SelectHitPattern: file cannot open" << std::endl;
    return false;
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
 
      int vecsize = detector_groups.size();
      detector_groups.resize(vecsize+1);
      dg = &detector_groups[vecsize];
      dg->setName(str);
    }
    else if (str == "ID") {
      while (iss >> detid) {
	if (dg) { dg->add(detid); }
      }
    }
    else if (str == "HITPATTERN") {
      HitPattern hitpat;
      iss >> str;
      hitpat.setName(str);
      iss >> str;
      hitpat.setShortName(str);
      while (iss >> str) {
	hitpat.add(*get_detector_group(str));
      }
      hit_pattern_vec.push_back(hitpat);
      hit_pattern_flags.push_back(0);
      hit_pattern_counts.push_back(0);
    }
  }
  
  for (size_t i=0; i<detector_groups.size(); i++) {
    detector_groups[i].print();
  }
  
  return true;
}


void SelectHitPattern::determine_hit_pattern(const std::vector<DetectorHit_sptr>& hitvec)
{
  for (unsigned int i=0; i<hit_pattern_vec.size(); i++) {
    if (hit_pattern_vec[i].match(hitvec)) {
      hit_pattern_flags[i] = 1;
      hit_pattern_counts[i]++;
    }
    else {
      hit_pattern_flags[i] = 0;
    }
  }
}


void SelectHitPattern::determine_hit_pattern(const std::vector<DetectorHit*>& hitvec)
{
  for (unsigned int i=0; i<hit_pattern_vec.size(); i++) {
    if (hit_pattern_vec[i].match(hitvec)) {
      hit_pattern_flags[i] = 1;
      hit_pattern_counts[i]++;
    }
    else {
      hit_pattern_flags[i] = 0;
    }
  }
}


void SelectHitPattern::determine_hit_pattern(const std::vector<int>& idvec)
{
  for (unsigned int i=0; i<hit_pattern_vec.size(); i++) {
    if (hit_pattern_vec[i].match(idvec)) {
      hit_pattern_flags[i] = 1;
      hit_pattern_counts[i]++;
    }
    else {
      hit_pattern_flags[i] = 0;
    }
  }
}
