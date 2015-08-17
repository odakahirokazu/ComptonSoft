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

#include "SetChannelsInfo.hh"

#include <iostream>
#include <functional>
#include <boost/property_tree/xml_parser.hpp>

#include "SimDetectorUnit2DPad.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"

using namespace anl;

namespace comptonsoft
{

SetChannelsInfo::SetChannelsInfo()
  : m_ByFile(true),
    m_Filename(""),
    m_DetectorType(1),
    m_Side1NoiseParam0(0.0),
    m_Side1NoiseParam1(0.0),
    m_Side1NoiseParam2(0.0),
    m_Side1CompensationFactor(1.0),
    m_Side2NoiseParam0(0.0),
    m_Side2NoiseParam1(0.0),
    m_Side2NoiseParam2(0.0),
    m_Side2CompensationFactor(1.0)
{
}

SetChannelsInfo::~SetChannelsInfo() = default;

ANLStatus SetChannelsInfo::mod_startup()
{
  register_parameter(&m_Filename, "filename");
  register_parameter_map(&m_Map, "channels_info_map",
                         "detector_name_prefix", "Si");
  add_map_value(&m_DetectorType, "detector_type", "",
                "Detector type (1: single, 2: double)");
  add_map_value(&m_Side1NoiseParam0, "side1_noise_param_0");
  add_map_value(&m_Side1NoiseParam1, "side1_noise_param_1");
  add_map_value(&m_Side1NoiseParam2, "side1_noise_param_2");
  add_map_value(&m_Side1CompensationFactor, "side1_compensation_factor");
  add_map_value(&m_Side2NoiseParam0, "side2_noise_param_0");
  add_map_value(&m_Side2NoiseParam1, "side2_noise_param_1");
  add_map_value(&m_Side2NoiseParam2, "side2_noise_param_2");
  add_map_value(&m_Side2CompensationFactor, "side2_compensation_factor");
  
  return AS_OK;
}

ANLStatus SetChannelsInfo::mod_init()
{
  VCSModule::mod_init();

  if (m_Filename == "") {
    m_ByFile = false;
  }

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

bool SetChannelsInfo::set_by_map()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    const std::string prefix = detector->getNamePrefix();
    if (m_Map.count(prefix) == 0) {
      std::cout << "No detector name prefix is found in Noise level map. "
                << detector->getName() << std::endl;
      continue;
    }

    const auto parameters = m_Map[prefix];
    const double side1NoiseParam0 = std::get<1>(parameters);
    const double side1NoiseParam1 = std::get<2>(parameters);
    const double side1NoiseParam2 = std::get<3>(parameters);
    const double side1CompensationFactor = std::get<4>(parameters);
    const double side2NoiseParam0 = std::get<5>(parameters);
    const double side2NoiseParam1 = std::get<6>(parameters);
    const double side2NoiseParam2 = std::get<7>(parameters);
    const double side2CompensationFactor = std::get<8>(parameters);
    
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
        ds->setNoiseParamToSelected(NoiseParam_t(side1NoiseParam0,
                                                 side1NoiseParam1,
                                                 side1NoiseParam2),
                                    xStripSelector);
        ds->setEPICompensationFactorToSelected(side1CompensationFactor,
                                               xStripSelector);
        // anode
        ds->setNoiseParamToSelected(NoiseParam_t(side2NoiseParam0,
                                                 side2NoiseParam1,
                                                 side2NoiseParam2),
                                    yStripSelector);
        ds->setEPICompensationFactorToSelected(side2CompensationFactor,
                                               yStripSelector);
      }
      else if (ds->isXStripSideAnode()){
        // anode
        ds->setNoiseParamToSelected(NoiseParam_t(side2NoiseParam0,
                                                 side2NoiseParam1,
                                                 side2NoiseParam2),
                                    xStripSelector);
        ds->setEPICompensationFactorToSelected(side2CompensationFactor,
                                               xStripSelector);
        // cathode
        ds->setNoiseParamToSelected(NoiseParam_t(side1NoiseParam0,
                                                 side1NoiseParam1,
                                                 side1NoiseParam2),
                                    yStripSelector);
        ds->setEPICompensationFactorToSelected(side1CompensationFactor,
                                               yStripSelector);
      }
    }
    else {
      DeviceSimulation* ds
        = dynamic_cast<DeviceSimulation*>(detector.get());
      if (ds == 0) {
        std::cout << "Detector type is not DeviceSimulation. Detector ID: " << detid << std::endl;
        return false;
      }
      ds->resetNoiseParamVector(NoiseParam_t(side1NoiseParam0,
                                             side1NoiseParam1,
                                             side1NoiseParam2));
      ds->resetEPICompensationFactorVector(side1CompensationFactor);
    }
  }
  
  return true;
}

bool SetChannelsInfo::set_by_file()
{
  using boost::property_tree::ptree;
  using boost::optional;

  ptree pt;
  try {
    read_xml(m_Filename, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    std::cout << "cannot parse: " << m_Filename << std::endl;
    return false;
  }

  for (ptree::value_type& v: pt.get_child("channels_info.data")) {
    if (v.first == "detector") {
      const ptree& detectorNode = v.second;
      const int detectorID = detectorNode.get<int>("<xmlattr>.id");
      DeviceSimulation* ds = dynamic_cast<DeviceSimulation*>(getDetectorManager()->getDeviceSimulationByID(detectorID));
      if (ds == 0) {
        std::cout << "Could not get DeviceSimulation. Detector ID: " << detectorID << std::endl;
        return false;
      }

      
      for (const ptree::value_type& vv: detectorNode.get_child("")) {
        if (vv.first == "section") {
          const ptree& sectionNode = vv.second;
          const int section = sectionNode.get<int>("<xmlattr>.id");
          for (const ptree::value_type& vvv: sectionNode.get_child("")) {
            if (vvv.first == "channel") {
              const ptree& channelNode = vvv.second;
              setupChannelInfo(section, channelNode, ds);
            }
          }
        }
      }
    }
    else if (v.first == "readout_module") {
      const ptree& moduleNode = v.second;
      const int moduleID = moduleNode.get<int>("<xmlattr>.id");
      const DetectorReadoutModule* readoutModule
        = getDetectorManager()->getReadoutModuleByID(moduleID);

      for (const ptree::value_type& vv: moduleNode.get_child("")) {
        if (vv.first == "section") {
          const ptree& sectionNode = vv.second;
          const int mod_section = sectionNode.get<int>("<xmlattr>.id");
          DetectorChannelID channelID
            = readoutModule->ReadoutSection(mod_section);
          const int detectorID = channelID.Detector();
          const int det_section = channelID.Section();
          
          DeviceSimulation* ds = dynamic_cast<DeviceSimulation*>(getDetectorManager()->getDeviceSimulationByID(detectorID));
          if (ds == 0) {
            std::cout << "Could not get DeviceSimulation. Detector ID: " << detectorID << std::endl;
            return false;
          }
          
          for (const ptree::value_type& vvv: sectionNode.get_child("")) {
            if (vvv.first == "channel") {
              const ptree& channelNode = vvv.second;
              setupChannelInfo(det_section, channelNode, ds);
            }
          }
        }
      }
    }
  }
  
  return true;
}

void SetChannelsInfo::setupChannelInfo(int section,
                                       const boost::property_tree::ptree& channelNode,
                                       DeviceSimulation* ds)
{
  using boost::property_tree::ptree;
  
  const int channel = channelNode.get<int>("<xmlattr>.id");
  const ChannelID channelID(section, channel);

  for (const ptree::value_type& v: channelNode.get_child("")) {
    if (v.first == "noise") {
      const double noiseParam0 = v.second.get<double>("param0");
      const double noiseParam1 = v.second.get<double>("param1");
      const double noiseParam2 = v.second.get<double>("param2");
      ds->setNoiseParam(channelID,
                        NoiseParam_t(noiseParam0, noiseParam1, noiseParam2));
    }
    else if (v.first == "compensation_factor") {
      const double compensationFactor = v.second.get_value<double>();
      ds->setEPICompensationFactor(channelID, compensationFactor);
    }
  }
}

} /*namespace comptonsoft */
