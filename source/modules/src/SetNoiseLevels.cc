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

#include "SetNoiseLevels.hh"

#include <iostream>
#include <functional>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "NextCLI.hh"
#include "SimDetectorUnit2DPad.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"

using namespace anl;

namespace comptonsoft
{

SetNoiseLevels::SetNoiseLevels()
  : m_ByFile(true),
    m_FileName(""),
    m_DetectorType(1),
    m_Noise00(0.0), m_Noise01(0.0), m_Noise02(0.0),
    m_Noise10(0.0), m_Noise11(0.0), m_Noise12(0.0)
{
}

SetNoiseLevels::~SetNoiseLevels() = default;

ANLStatus SetNoiseLevels::mod_startup()
{
  register_parameter(&m_FileName, "filename");
  register_parameter_map(&m_NoiseLevelMap, "noise_level_map",
                         "detector_name_prefix", "Si");
  add_map_value(&m_DetectorType, "detector_type", "",
                "Detector type (1: single, 2: double)");
  add_map_value(&m_Noise00, "noise_coefficient_00");
  add_map_value(&m_Noise01, "noise_coefficient_01");
  add_map_value(&m_Noise02, "noise_coefficient_02");
  add_map_value(&m_Noise10, "noise_coefficient_10");
  add_map_value(&m_Noise11, "noise_coefficient_11");
  add_map_value(&m_Noise12, "noise_coefficient_12");
  
  return AS_OK;
}

ANLStatus SetNoiseLevels::mod_com()
{
  hide_parameter("noise_level_map");

  CLread("Set by file (1:yes, 0:no)", &m_ByFile);

  if (m_ByFile) {
    m_FileName = "noiselevels.xml";
    CLread("Filename", &m_FileName);
  }
  else {
    std::string name("Si");    
    int type = 1;
    
    std::cout << "Definition of noise levels: " << std::endl;
    
    while (1) {
      CLread("Detector name prefix (OK for exit)", &name);
      if (name == "OK" or name == "ok") { break; }
      
      CLread("Detector type (1:single-side, 2:double-side)", &type);
      
      if (type == 1) {
        double noiseParam0 = 1.0; // keV
        CLread("Noise level Param 0", &noiseParam0, 1.0, "keV");
        double noiseParam1 = 0.044;
        CLread("Noise level Param 1", &noiseParam1);
        double noiseParam2 = 0.0;
        CLread("Noise level Param 2", &noiseParam2);
        
        m_NoiseLevelMap.insert(std::make_pair(name, std::make_tuple(type, noiseParam0, noiseParam1, noiseParam2, 0., 0., 0.)));
      }
      else if (type == 2) {
        double noiseParam00 = 1.0; // keV
        CLread("Noise level Param 0 (cathode)", &noiseParam00, 1.0, "keV");
        double noiseParam01 = 0.044;
        CLread("Noise level Param 1 (cathode)", &noiseParam01);
        double noiseParam02 = 0.0;
        CLread("Noise level Param 2 (cathode)", &noiseParam02);
        
        double noiseParam10 = 1.0; // keV
        CLread("Noise level Param 0 (anode)", &noiseParam10, 1.0, "keV");
        double noiseParam11 = 0.044;
        CLread("Noise level Param 1 (anode)", &noiseParam11);
        double noiseParam12 = 0.0;
        CLread("Noise level Param 2 (anode)", &noiseParam12);
        
        m_NoiseLevelMap.insert(std::make_pair(name, std::make_tuple(type, noiseParam00, noiseParam01, noiseParam02, noiseParam10, noiseParam11, noiseParam12)));
      }
      else {
        std::cout << "Detector type is invalid." << std::endl;
      }
      
      name = "OK";
    }
  }

  return AS_OK;
}

ANLStatus SetNoiseLevels::mod_init()
{
  VCSModule::mod_init();

  if (m_FileName == "") { m_ByFile = false; }

  bool rval = false;
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

bool SetNoiseLevels::set_by_map()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    const std::string prefix = detector->getNamePrefix();
    if (m_NoiseLevelMap.count(prefix) == 0) {
      std::cout << "No detector name prefix is found in Noise level map. "
                << detector->getName() << std::endl;
      continue;
    }
    
    const double noise00 = std::get<1>(m_NoiseLevelMap[prefix]);
    const double noise01 = std::get<2>(m_NoiseLevelMap[prefix]);
    const double noise02 = std::get<3>(m_NoiseLevelMap[prefix]);
    const double noise10 = std::get<4>(m_NoiseLevelMap[prefix]);
    const double noise11 = std::get<5>(m_NoiseLevelMap[prefix]);
    const double noise12 = std::get<6>(m_NoiseLevelMap[prefix]);
    
    const int detid = detector->getID();
    if (detector->Type().find("SimDetector2DStrip") == 0) {
      SimDetectorUnit2DStrip* ds
        = dynamic_cast<SimDetectorUnit2DStrip*>(detector.get());
      if (ds == 0) {
        std::cout << "Detector type is not 2D strip. Detector ID: " << detid << std::endl;
        return false;
      }
      auto xStripSelector = std::mem_fn(&PixelID::isXStrip);
      auto yStripSelector = std::mem_fn(&PixelID::isYStrip);
      if (ds->isXStripSideCathode()) {
        // cathode
        ds->setNoiseParamToSelected(NoiseParam_t(noise00, noise01, noise02),
                                    xStripSelector);
        // anode
        ds->setNoiseParamToSelected(NoiseParam_t(noise10, noise11, noise12),
                                    yStripSelector);
      }
      else if (ds->isXStripSideAnode()){
        // anode
        ds->setNoiseParamToSelected(NoiseParam_t(noise10, noise11, noise12),
                                    xStripSelector);
        // cathode
        ds->setNoiseParamToSelected(NoiseParam_t(noise00, noise01, noise02),
                                    yStripSelector);
      }
    }
    else {
      VDeviceSimulation* ds
        = dynamic_cast<VDeviceSimulation*>(detector.get());
      if (ds == 0) {
        std::cout << "Detector type is not DeviceSimulation. Detector ID: " << detid << std::endl;
        return false;
      }
      ds->resetNoiseParamVector(NoiseParam_t(noise00, noise01, noise02));
    }
  }
  
  return true;
}

bool SetNoiseLevels::set_by_file()
{
  using boost::property_tree::ptree;
  using boost::optional;

  ptree pt;
  try {
    read_xml(m_FileName, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    std::cout << "cannot parse: " << m_FileName << std::endl;
    return false;
  }

  for (ptree::value_type& v: pt.get_child("noiselevels.data")) {
    if (v.first == "detector") {
      const ptree detectorNode = v.second;
      const int detectorID = detectorNode.get<int>("<xmlattr>.id");
      VDeviceSimulation* const ds = getDetectorManager()->getDeviceSimulationByID(detectorID);
      for (const ptree::value_type& vv: detectorNode.get_child("")) {
        if (vv.first == "section") {
          const ptree sectionNode = vv.second;
          const int section = sectionNode.get<int>("<xmlattr>.id");
          for (const ptree::value_type& vvv: sectionNode.get_child("")) {
            if (vvv.first == "channel") {
              const ptree channelNode = vvv.second;
              const int channel = channelNode.get<int>("<xmlattr>.id");
              const double param0 = channelNode.get<double>("param0");
              const double param1 = channelNode.get<double>("param1");
              const double param2 = channelNode.get<double>("param2");
              ds->setNoiseParam(ChannelID(section, channel), NoiseParam_t(param0, param1, param2));
            }
          }
        }
      }
    }
    else if (v.first == "readout_module") {
      const ptree moduleNode = v.second;
      const int moduleID = moduleNode.get<int>("<xmlattr>.id");
      const DetectorReadoutModule* readoutModule
        = getDetectorManager()->getReadoutModuleByID(moduleID);

      for (const ptree::value_type& vv: moduleNode.get_child("")) {
        if (vv.first == "section") {
          const ptree sectionNode = vv.second;
          const int mod_section = sectionNode.get<int>("<xmlattr>.id");
          DetectorChannelID channelID
            = readoutModule->ReadoutSection(mod_section);
          const int detectorID = channelID.Detector();
          const int det_section = channelID.Section();
          
          VDeviceSimulation* const ds
            = getDetectorManager()->getDeviceSimulationByID(detectorID);
          
          for (const ptree::value_type& vvv: sectionNode.get_child("")) {
            if (vvv.first == "channel") {
              const ptree channelNode = vvv.second;
              const int channel = channelNode.get<int>("<xmlattr>.id");
              const double param0 = channelNode.get<double>("param0");
              const double param1 = channelNode.get<double>("param1");
              const double param2 = channelNode.get<double>("param2");
              ds->setNoiseParam(ChannelID(det_section, channel), NoiseParam_t(param0, param1, param2));
            }
          }
        }
      }
    }
  }
  
  return true;
}

} /*namespace comptonsoft */
