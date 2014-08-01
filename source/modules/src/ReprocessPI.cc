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

#include "ReprocessPI.hh"

#include <fstream>
#include "TMath.h"
#include "TRandom3.h"
#include "globals.hh"

#include "NextCLI.hh"
#include "DetectorHit.hh"


using namespace comptonsoft;
using namespace anl;

ReprocessPI::ReprocessPI()
  : hit_collection(0), dg_filename("detector_group.txt"),
    m_Factor0(1.0), m_Noise00(0.0), m_Noise01(0.0), m_Noise02(0.0)
{
}  


ANLStatus ReprocessPI::mod_startup()
{
  register_parameter(&dg_filename, "detector_group");
  
  register_parameter_map(&param_map, "parameter_map",
                         "detector_group", "Si");
  add_map_value(&m_Factor0, "gain_correction_factor");
  add_map_value(&m_Noise00, "noise_coefficient_0");
  add_map_value(&m_Noise01, "noise_coefficient_1");
  add_map_value(&m_Noise02, "noise_coefficient_2");
  
  return AS_OK;
}


#if 0
ANLStatus ReprocessPI::mod_com()
{
  ask_parameters();

  std::string name("DSSD");
  std::cout << "Definition of noise level (FWHM) : " << std::endl;

  while (1) {
    CLread("detector group name (OK for exit) :", &name);
    if (name == "OK" or name == "ok") { break; }

    double noiseParam0 = 1.0; // keV
    CLread("Noise level Param 0 [keV]", &noiseParam0);
    double noiseParam1 = 0.044;
    CLread("Noise level Param 1 :", &noiseParam1);
    double noiseParam2 = 0.0;
    CLread("Noise level Param 2 :", &noiseParam2);
    noiselevel_vector.push_back(boost::make_tuple(name, noiseParam0, noiseParam1, noiseParam2, 0., 0., 0.));
    
    name = "OK";
  }
  
  return AS_OK;
}
#endif


ANLStatus ReprocessPI::mod_init()
{
  GetANLModuleNC("CSHitCollection", &hit_collection);

  if (dg_filename[0] == '0') {
    std::cout << "ReprocessPI: There is no detector group." << std::endl;
    return AS_OK;
  }

  if (!load_detector_groups()) {
    return AS_QUIT;
  }

  return AS_OK;
}


ANLStatus ReprocessPI::mod_ana()
{
  int numTimeGroup = hit_collection->NumTimeGroup();
  for (int tGroup=comptonsoft::NOTIMEGROUP; tGroup<numTimeGroup; tGroup++) {
    std::vector<DetectorHit_sptr>& hitVec
      = hit_collection->GetHits(tGroup);
    
    std::vector<DetectorHit_sptr>::iterator hit;
    for(hit=hitVec.begin(); hit!=hitVec.end(); ++hit) {
      int detid = (*hit)->getDetectorID();

      /*
      for (size_t i=0; i<noiselevel_vector.size(); ++i) {
        const DetectorGroup* dg = get_detector_group(noiselevel_vector[i].get<0>());
      */
      map_type::iterator mit;
      for (mit=param_map.begin(); mit!=param_map.end(); ++mit) {
        const DetectorGroup* dg = get_detector_group(mit->first);
        if (dg->isMember(detid)) {
#if 0
          double noiseParam0 = noiselevel_vector[i].get<1>();
          double noiseParam1 = noiselevel_vector[i].get<2>();
          double noiseParam2 = noiselevel_vector[i].get<3>();

          double pha = (*hit)->getPHA();
#else
          double factor = (mit->second).get<0>();
          double noiseParam0 = (mit->second).get<1>();
          double noiseParam1 = (mit->second).get<2>();
          double noiseParam2 = (mit->second).get<3>();

          double pha = (*hit)->getPHA() * factor;
#endif

          double ene = pha/keV; // keV
          double noiseA1 = noiseParam0; // keV
          double noiseB2 = noiseParam1 * noiseParam1 * ene; // keV2
          double noiseC1 = noiseParam2 * ene; // keV
          
          double fwhm2 = noiseA1*noiseA1 + noiseB2 + noiseC1*noiseC1; // keV
          double sigma = TMath::Sqrt(fwhm2)/2.3548 * keV;
          
          double ePI = gRandom->Gaus(pha, sigma);
          (*hit)->setPI(ePI);
          
          break;
        }
      }
    }
  }
  
  return AS_OK;
}


const DetectorGroup* ReprocessPI::get_detector_group(const std::string& name)
{
  for (unsigned int i=0; i<detector_groups.size(); i++) {
    if (name == detector_groups[i].Name()) {
      return &(detector_groups[i]);
    }
  }

  return 0;
}


bool ReprocessPI::load_detector_groups()
{
  if (dg_filename[0] == '0') {
    std::cout << "ReprocessPI: There is no detector group." << std::endl;
    return true;
  }

  std::ifstream fin;
  fin.open(dg_filename.c_str());
  if (!fin) {
    std::cout << "ReprocessPI: file cannot open" << std::endl;
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
#if 0
      size_t n = noiselevel_vector.size();
      for (size_t i=0; i<n; ++i) {
        if (noiselevel_vector[i].get<0>() == str) {
          match = true;
          break;
        }
      }
#else
      map_type::iterator mit;
      for (mit=param_map.begin(); mit!=param_map.end(); ++mit) {
        if (mit->first == str) {
          match = true;
          break;
        }
      }
#endif
      
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
