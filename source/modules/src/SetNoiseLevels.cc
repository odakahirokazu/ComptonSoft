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

#include "SimDetectorUnit2DPixel.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"

using namespace anl;

namespace comptonsoft
{

SetNoiseLevels::SetNoiseLevels()
  : m_ByFile(true),
    m_FileName(""),
    m_DetectorType(1),
    m_Noise0(0.0), m_Noise1(0.0), m_Noise2(0.0),
    m_CathodeNoise0(0.0), m_CathodeNoise1(0.0), m_CathodeNoise2(0.0),
    m_AnodeNoise0(0.0), m_AnodeNoise1(0.0), m_AnodeNoise2(0.0)
{
}

SetNoiseLevels::~SetNoiseLevels() = default;

ANLStatus SetNoiseLevels::mod_define()
{
  register_parameter(&m_FileName, "filename");
  register_parameter_map(&m_NoiseLevelMap, "noise_level_map",
                         "detector_name_prefix", "Si");
  add_value_element(&m_DetectorType, "detector_type", "",
                    "Detector type (1: single, 2: double)");
  add_value_element(&m_Noise0, "noise_coefficient0");
  add_value_element(&m_Noise1, "noise_coefficient1");
  add_value_element(&m_Noise2, "noise_coefficient2");
  add_value_element(&m_CathodeNoise0, "cathode_noise_coefficient0");
  add_value_element(&m_CathodeNoise1, "cathode_noise_coefficient1");
  add_value_element(&m_CathodeNoise2, "cathode_noise_coefficient2");
  add_value_element(&m_AnodeNoise0, "anode_noise_coefficient0");
  add_value_element(&m_AnodeNoise1, "anode_noise_coefficient1");
  add_value_element(&m_AnodeNoise2, "anode_noise_coefficient2");

  enable_value_elements(1, {1, 2, 3});
  enable_value_elements(2, {4, 5, 6, 7, 8, 9});
  enable_value_elements(3, {1, 2, 3});
  
  return AS_OK;
}

ANLStatus SetNoiseLevels::mod_initialize()
{
  VCSModule::mod_initialize();

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

    const auto parameters = m_NoiseLevelMap[prefix];
    const int type = std::get<0>(parameters);
    if (!detector->checkType(type)) {
      std::cout << "Detector type given in the analysis parameters is inconsistent.\n"
                << "  prefix: " << prefix << "\n"
                << "  type: " << type << " => should be " << static_cast<int>(detector->Type())
                << std::endl;
      return false;
    }
    
    const int detid = detector->getID();
    if (detector->checkType(DetectorType::DoubleSidedStripDetector)) {
      const double cathode_noise0 = std::get<4>(parameters);
      const double cathode_noise1 = std::get<5>(parameters);
      const double cathode_noise2 = std::get<6>(parameters);
      const double anode_noise0 = std::get<7>(parameters);
      const double anode_noise1 = std::get<8>(parameters);
      const double anode_noise2 = std::get<9>(parameters);
      
      SimDetectorUnit2DStrip* ds
        = dynamic_cast<SimDetectorUnit2DStrip*>(detector.get());
      if (ds == nullptr) {
        std::cout << "The detector object can not be converted into SimDetector2DStrip. Detector ID: " << detid << std::endl;
        return false;
      }
      auto xStripSelector = std::mem_fn(&PixelID::isXStrip);
      auto yStripSelector = std::mem_fn(&PixelID::isYStrip);
      if (ds->isXStripSideCathode()) {
        // cathode
        ds->setNoiseParam0ToSelected(cathode_noise0, xStripSelector);
        ds->setNoiseParam1ToSelected(cathode_noise1, xStripSelector);
        ds->setNoiseParam2ToSelected(cathode_noise2, xStripSelector);
        // anode
        ds->setNoiseParam0ToSelected(anode_noise0, yStripSelector);
        ds->setNoiseParam1ToSelected(anode_noise1, yStripSelector);
        ds->setNoiseParam2ToSelected(anode_noise2, yStripSelector);
      }
      else if (ds->isXStripSideAnode()){
        // anode
        ds->setNoiseParam0ToSelected(anode_noise0, xStripSelector);
        ds->setNoiseParam1ToSelected(anode_noise1, xStripSelector);
        ds->setNoiseParam2ToSelected(anode_noise2, xStripSelector);
        // cathode
        ds->setNoiseParam0ToSelected(cathode_noise0, yStripSelector);
        ds->setNoiseParam1ToSelected(cathode_noise1, yStripSelector);
        ds->setNoiseParam2ToSelected(cathode_noise2, yStripSelector);
      }
    }
    else {
      VDeviceSimulation* ds
        = dynamic_cast<VDeviceSimulation*>(detector.get());
      if (ds == nullptr) {
        std::cout << "The detector object can not be converted into VDeviceSimulation. Detector ID: " << detid << std::endl;
        return false;
      }

      const double noise0 = std::get<1>(parameters);
      const double noise1 = std::get<2>(parameters);
      const double noise2 = std::get<3>(parameters);
      ds->resetNoiseParam0Vector(noise0);
      ds->resetNoiseParam1Vector(noise1);
      ds->resetNoiseParam2Vector(noise2);
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
              ds->setNoiseParam0(SectionChannelPair(section, channel), param0);
              ds->setNoiseParam1(SectionChannelPair(section, channel), param1);
              ds->setNoiseParam2(SectionChannelPair(section, channel), param2);
            }
          }
        }
      }
    }
    else if (v.first == "readout_module") {
      const ptree moduleNode = v.second;
      const int moduleID = moduleNode.get<int>("<xmlattr>.id");
      const ReadoutModule* readoutModule
        = getDetectorManager()->getReadoutModuleByID(moduleID);

      for (const ptree::value_type& vv: moduleNode.get_child("")) {
        if (vv.first == "section") {
          const ptree sectionNode = vv.second;
          const int mod_section = sectionNode.get<int>("<xmlattr>.id");
          DetectorBasedChannelID channelID
            = readoutModule->getSection(mod_section);
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
              ds->setNoiseParam0(SectionChannelPair(det_section, channel), param0);
              ds->setNoiseParam1(SectionChannelPair(det_section, channel), param1);
              ds->setNoiseParam2(SectionChannelPair(det_section, channel), param2);
            }
          }
        }
      }
    }
  }
  
  return true;
}

} /*namespace comptonsoft */
