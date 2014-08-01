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

#include "SetNoiseLevel.hh"

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "globals.hh"

#include "DetectorUnitDef.hh"

#include "NextCLI.hh"

using namespace comptonsoft;
using namespace anl;


SetNoiseLevel::SetNoiseLevel()
  : m_ByFile(true),
    m_FileName(""),
    m_DetectorType(1),
    m_Noise00(0.0), m_Noise01(0.0), m_Noise02(0.0),
    m_Noise10(0.0), m_Noise11(0.0), m_Noise12(0.0)
{
}


ANLStatus SetNoiseLevel::mod_startup()
{
  register_parameter(&m_FileName, "filename");
  register_parameter_map(&m_NoiseLevelMap, "noise_level_map",
                         "detector_name_prefix", "Si");
  add_map_value(&m_DetectorType, "detector_type", "",
                "Detector type (1: pad, 2: DSD, 3: scintillator)");
  add_map_value(&m_Noise00, "noise_coefficient_00");
  add_map_value(&m_Noise01, "noise_coefficient_01");
  add_map_value(&m_Noise02, "noise_coefficient_02");
  add_map_value(&m_Noise10, "noise_coefficient_10");
  add_map_value(&m_Noise11, "noise_coefficient_11");
  add_map_value(&m_Noise12, "noise_coefficient_12");
  
  return AS_OK;
}


ANLStatus SetNoiseLevel::mod_com()
{
  hide_parameter("noise_level_map");

  CLread("Set by file (1:yes 0:no)", &m_ByFile);

  if (m_ByFile) {
    m_FileName = "noiselevel.xml";
    CLread("File name", &m_FileName);
  }
  else {
    std::string name("Si");
    
    int type = 1;
    
    std::cout << "Definition of noise level (FWHM) : " << std::endl;
    
    while (1) {
      CLread("Detector name prefix (OK for exit)", &name);
      if (name == "OK" or name == "ok") { break; }
      
      CLread("Detector type (1:pad, 2:double-sided strip, 3:scintillator, 4:all-param[single-side], 5:all-param[double-side])", &type);
      
      if (type == 1) {
        double noiseConstant = 1.0; // keV
        CLread("Noise level constant (FWHM)", &noiseConstant, 1.0, "keV");
        double noiseRatio = 0.001;
        CLread("Noise level ratio (FWHM)", &noiseRatio);
        m_NoiseLevelMap.insert(std::make_pair(name, boost::make_tuple(type, noiseConstant, 0.0, noiseRatio, 0., 0., 0.)));
      }
      else if (type == 2) {
        double noiseConstantPside = 1.0; // keV
        CLread("Noise level constant of P-side (FWHM)", &noiseConstantPside, 1.0, "keV");
        double noiseRatioPside = 0.001;
        CLread("Noise level ratio of P-side", &noiseRatioPside);
        
        double noiseConstantNside = 1.0; // keV
        CLread("Noise level constant of N-side (FWHM)", &noiseConstantNside, 1.0, "keV");
        double noiseRatioNside = 0.001;
        CLread("Noise level ratio of N-side (FWHM)", &noiseRatioNside);
        
        m_NoiseLevelMap.insert(std::make_pair(name, boost::make_tuple(type, noiseConstantPside, 0.0, noiseRatioPside, noiseConstantNside, 0.0, noiseRatioNside)));
      }
      else if (type == 3) {
        double noiseConstant = 1.0; // keV
        CLread("Noise level constant (FWHM)", &noiseConstant, 1.0, "keV");
        double noiseRatioAt662keV = 0.1;
        CLread("Noise level ratio at 662 keV (FWHM)", &noiseRatioAt662keV);
        double noiseParam1 = noiseRatioAt662keV * std::sqrt(662.0);
        
        m_NoiseLevelMap.insert(std::make_pair(name, boost::make_tuple(type, noiseConstant, noiseParam1, 0., 0., 0., 0.)));
      }
      else if (type == 4) {
        double noiseParam0 = 1.0; // keV
        CLread("Noise level Param 0", &noiseParam0, 1.0, "keV");
        double noiseParam1 = 0.044;
        CLread("Noise level Param 1", &noiseParam1);
        double noiseParam2 = 0.0;
        CLread("Noise level Param 2", &noiseParam2);
        
        m_NoiseLevelMap.insert(std::make_pair(name, boost::make_tuple(type, noiseParam0, noiseParam1, noiseParam2, 0., 0., 0.)));
      }
      else if (type == 5) {
        double noiseParam0Pside = 1.0; // keV
        CLread("Noise level Param 0 of P-side", &noiseParam0Pside, 1.0, "keV");
        double noiseParam1Pside = 0.044;
        CLread("Noise level Param 1 of P-side", &noiseParam1Pside);
        double noiseParam2Pside = 0.0;
        CLread("Noise level Param 2 of P-side", &noiseParam2Pside);
        
        double noiseParam0Nside = 1.0; // keV
        CLread("Noise level Param 0 of N-side", &noiseParam0Nside, 1.0, "keV");
        double noiseParam1Nside = 0.044;
        CLread("Noise level Param 1 of N-side", &noiseParam1Nside);
        double noiseParam2Nside = 0.0;
        CLread("Noise level Param 2 of N-side", &noiseParam2Nside);
        
        m_NoiseLevelMap.insert(std::make_pair(name, boost::make_tuple(type, noiseParam0Pside, noiseParam1Pside, noiseParam2Pside, noiseParam0Nside, noiseParam1Nside, noiseParam2Nside)));
      }
      else {
        std::cout << "Detector type is invalid." << std::endl;
      }
      
      name = "OK";
    }
  }

  return AS_OK;
}


ANLStatus SetNoiseLevel::mod_init()
{
  VCSModule::mod_init();

  if (m_FileName == "") m_ByFile = false;

  bool rval;
  if (m_ByFile) {
    rval = set_by_file();
  }
  else {
    rval = set_by_map();
  }

  if (!rval) {
    return AS_QUIT;
  }

  return AS_OK;
}


bool SetNoiseLevel::set_by_map()
{
  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  
  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    std::string prefix = (*itDet)->getNamePrefix();
    if (m_NoiseLevelMap.count(prefix) == 0) {
      std::cout << "No detector name prefix is found in Noise level map. "
		<< (*itDet)->getName() << std::endl;
      continue;
    }
    
    double noise00 = m_NoiseLevelMap[prefix].get<1>();
    double noise01 = m_NoiseLevelMap[prefix].get<2>();
    double noise02 = m_NoiseLevelMap[prefix].get<3>();
    double noise10 = m_NoiseLevelMap[prefix].get<4>();
    double noise11 = m_NoiseLevelMap[prefix].get<5>();
    double noise12 = m_NoiseLevelMap[prefix].get<6>();
    
    int detid = (*itDet)->getID();
    if ((*itDet)->Type().find("SimDetectorLite") == 0) {
      SimDetectorUnitLite* ds
	= dynamic_cast<SimDetectorUnitLite*>((*itDet));
      if (ds == 0) {
        if (ds == 0) {
          std::cout << "Detector type is not LITE. (" << detid << std::endl;
          return false;
        }
     
        ds->setNoiseFWHMAll(noise00, noise01, noise02);
        double dummy = 0.0;
        dummy = noise10, dummy = noise11, dummy = noise12;
      }
    }
#if USE_ROOT
    else if ((*itDet)->Type().find("SimDetector2DPad") == 0) {
      SimDetectorUnit2DPad* ds
	= dynamic_cast<SimDetectorUnit2DPad*>((*itDet));
      if (ds == 0) {
	std::cout << "Detector type is not 2D pad. (" << detid << std::endl;
	return false;
      }

      ds->setNoiseFWHMAll(noise00, noise01, noise02);
    }
    else if ((*itDet)->Type().find("SimDetector2DStrip") == 0) {
      SimDetectorUnit_Strip_t* ds
	= dynamic_cast<SimDetectorUnit2DStrip*>((*itDet));
      if (ds == 0) {
	std::cout << "Detector type is not 2D strip. (" << detid << std::endl;
        return false;
      }
      
      if (ds->UpsideXStrip() == ds->UpsideAnode()) {
	ds->setNoiseFWHMXStripSideAll(noise10, noise11, noise12); // N-side
	ds->setNoiseFWHMYStripSideAll(noise00, noise01, noise02); // P-side
      }
      else {
	ds->setNoiseFWHMXStripSideAll(noise00, noise01, noise02); // P-side
	ds->setNoiseFWHMYStripSideAll(noise10, noise11, noise12); // N-side
      }
    }
    else if ((*itDet)->Type().find("SimDetectorScintillator") == 0) {
      SimDetectorUnitScintillator* ds
	= dynamic_cast<SimDetectorUnitScintillator*>((*itDet));
      if (ds == 0) {
	std::cout << "Detector type is not Scintillator. (" << detid << std::endl;
	return false;
      }

      ds->setNoiseFWHMAll(noise00, noise01, noise02);
    }
#endif // USE_ROOT
  }
  
  return true;
}


bool SetNoiseLevel::set_by_file()
{
  using boost::property_tree::ptree;

  ptree pt;
  try {
    read_xml(m_FileName, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    std::cout << "cannot parse: " << m_FileName << std::endl;
    return AS_QUIT;
  }

  foreach (ptree::value_type& v, pt.get_child("noiselevel.detectors")) {
    if (v.first == "detector") {
      ptree detNode = v.second;
      int detid = detNode.get<int>("<xmlattr>.id");
      VDeviceSimulation* const ds = GetDetectorManager()->getDeviceSimulationByID(detid);
      foreach (ptree::value_type& vv, detNode.get_child("")) {
        if (vv.first == "chip") {
          ptree chipNode = vv.second;
          int chipid = chipNode.get<int>("<xmlattr>.id");
          foreach (ptree::value_type& vvv, chipNode.get_child("")) {
            if (vvv.first == "channel") {
              ptree channelNode = vvv.second;
              int channel = channelNode.get<int>("<xmlattr>.id");
              double param0 = channelNode.get<double>("param0");
              double param1 = channelNode.get<double>("param1");
              double param2 = channelNode.get<double>("param2");
              ds->setNoiseFWHM(chipid, channel, param0, param1, param2);
            }
          }
        }
      }
    }
  }

  return true;
}
