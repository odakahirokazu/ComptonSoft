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

#include "SetChannelProperties.hh"

#include <iostream>
#include <functional>
#include <boost/format.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "G4SystemOfUnits.hh"

#include "ANLException.hh"
#include "VRealDetectorUnit.hh"
#include "MultiChannelData.hh"
#include "VChannelMap.hh"
#include "SimDetectorUnit2DPixel.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"

using namespace anl;

namespace comptonsoft
{

SetChannelProperties::SetChannelProperties()
  : m_Filename("channel_properties.xml")
{
}

SetChannelProperties::~SetChannelProperties() = default;

ANLStatus SetChannelProperties::mod_startup()
{
  register_parameter(&m_Filename, "filename");
  return AS_OK;
}

ANLStatus SetChannelProperties::mod_init()
{
  VCSModule::mod_init();
  readFile();
  return AS_OK;
}

void SetChannelProperties::readFile()
{
  using boost::property_tree::ptree;
  using boost::optional;

  ptree pt;
  try {
    read_xml(m_Filename, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    const std::string message
      = (boost::format("cannot parse: %s") % m_Filename).str();
    BOOST_THROW_EXCEPTION(ANLException(message));
  }

  loadRootNode(pt);
}

void SetChannelProperties::loadRootNode(const boost::property_tree::ptree& rootNode)
{
  using boost::property_tree::ptree;
  
  for (const ptree::value_type& v: rootNode.get_child("channel_properties.data")) {
    if (v.first == "detector") {
      const ptree& detectorNode = v.second;
      const int detectorID = detectorNode.get<int>("<xmlattr>.id");
      for (const ptree::value_type& vv: detectorNode.get_child("")) {
        if (vv.first == "section") {
          const ptree& sectionNode = vv.second;
          const int section = sectionNode.get<int>("<xmlattr>.id");
          DetectorBasedChannelID detectorChannelID(detectorID, section);
          loadSectionNode(sectionNode, detectorChannelID);
        }
      }
    }
    else if (v.first == "detector" || v.first == "readout_module") {
      const ptree& moduleNode = v.second;
      const int moduleID = moduleNode.get<int>("<xmlattr>.id");
      for (const ptree::value_type& vv: moduleNode.get_child("")) {
        if (vv.first == "section") {
          const ptree& sectionNode = vv.second;
          const int section = sectionNode.get<int>("<xmlattr>.id");
          ReadoutBasedChannelID readoutChannelID(moduleID, section);
          DetectorBasedChannelID detectorChannelID
            = getDetectorManager()->convertToDetectorBasedChannelID(readoutChannelID);
          loadSectionNode(sectionNode, detectorChannelID);
        }
      }
    }
  }
}  

void SetChannelProperties::
loadSectionNode(const boost::property_tree::ptree& sectionNode,
                DetectorBasedChannelID detectorChannelID)
{
  using boost::property_tree::ptree;
  using boost::optional;
  
  const int detectorID = detectorChannelID.Detector();
  const int section = detectorChannelID.Section();
  const optional<int> all = sectionNode.get_optional<int>("<xmlattr>.all");

  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID);
  DeviceSimulation* ds = nullptr;
  
  if (isMCSimulation()) {
    ds = dynamic_cast<DeviceSimulation*>(detector);
    if (ds == nullptr) {
      const std::string message
        = (boost::format("can not get DeviceSimulation. Detector ID: %d") % detectorID).str();
      BOOST_THROW_EXCEPTION(ANLException(message));
    }
  }

  DetectorSystem::ChannelNodeContents commonProperties;
  for (const ptree::value_type& vvv: sectionNode.get_child("")) {
    if (vvv.first == "common") {
      const ptree& channelNode = vvv.second;
      commonProperties.load(channelNode);

      if (all && (*all==1)) {
        const int NumChannels = detector->getChannelMap()->NumChannels(section);
        for (int i=0; i<NumChannels; i++) {
          DetectorSystem::ChannelNodeContents channelProperties(commonProperties);
          channelProperties.id = i;
          if (isMCSimulation()) {
            setupChannelProperties(section, channelProperties, ds);
          }
          else {
            setupChannelProperties(section, channelProperties, detector);
          }
        }
      }
    }
    else if (vvv.first == "channel") {
      const ptree& channelNode = vvv.second;
      DetectorSystem::ChannelNodeContents channelProperties(commonProperties);
      channelProperties.load(channelNode);
      if (isMCSimulation()) {
        setupChannelProperties(section, channelProperties, ds);
      }
      else {
        setupChannelProperties(section, channelProperties, detector);
      }
    }
  }
}

void SetChannelProperties::setupChannelProperties(int section,
                                                  const DetectorSystem::ChannelNodeContents& properties,
                                                  DeviceSimulation* ds)
{
  using boost::optional;

  if (properties.id == boost::none) {
    return;
  }

  const int channel = *(properties.id);
  const SectionChannelPair channelID(section, channel);
  
  if (optional<int> o = properties.disable_status) {
    ds->setChannelDisabled(channelID, *o);
  }
  if (optional<double> o = properties.noise_level_param0) {
    ds->setNoiseParam0(channelID, *o);
  }
  if (optional<double> o = properties.noise_level_param1) {
    ds->setNoiseParam1(channelID, *o);
  }
  if (optional<double> o = properties.noise_level_param2) {
    ds->setNoiseParam2(channelID, *o);
  }
  if (optional<double> o = properties.trigger_discrimination_center) {
    const double value = (*o) * keV;
    ds->setTriggerDiscriminationCenter(channelID, value);
  }
  if (optional<double> o = properties.trigger_discrimination_sigma) {
    const double value = (*o) * keV;
    ds->setTriggerDiscriminationSigma(channelID, value);
  }
  if (optional<double> o = properties.compensation_factor) {
    ds->setEPICompensationFactor(channelID, *o);
  }
  if (optional<double> o = properties.threshold_value) {
    const double value = (*o) * keV;
    ds->setThreshold(channelID, value);
  }
}

void SetChannelProperties::setupChannelProperties(int section,
                                                  const DetectorSystem::ChannelNodeContents& properties,
                                                  VRealDetectorUnit* detector)
{
  using boost::optional;

  if (properties.id == boost::none) {
    return;
  }

  const int channel = *(properties.id);
  MultiChannelData* mcd = detector->getMultiChannelData(section);

  if (optional<int> status = properties.disable_status) {
    mcd->setChannelDisabled(channel, *status);
  }
  if (optional<double> value = properties.threshold_value) {
    mcd->setThresholdEnergy(channel, *value);
  }
}

} /*namespace comptonsoft */
