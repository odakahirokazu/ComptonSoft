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

#include "SetDetectorsInfo.hh"

#include <iostream>
#include <functional>
#include <boost/format.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "ANLException.hh"
#include "VRealDetectorUnit.hh"
#include "MultiChannelData.hh"
#include "SimDetectorUnit2DPixel.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"

using namespace anl;

namespace comptonsoft
{

SetDetectorsInfo::SetDetectorsInfo()
  : m_Filename("channels_info.xml")
{
}

SetDetectorsInfo::~SetDetectorsInfo() = default;

ANLStatus SetDetectorsInfo::mod_define()
{
  register_parameter(&m_Filename, "filename");
  return AS_OK;
}

ANLStatus SetDetectorsInfo::mod_initialize()
{
  VCSModule::mod_initialize();
  readFile();
  return AS_OK;
}

void SetDetectorsInfo::readFile()
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

void SetDetectorsInfo::loadRootNode(const boost::property_tree::ptree& rootNode)
{
  using boost::property_tree::ptree;
  
  for (const ptree::value_type& v: rootNode.get_child("channels_info.data")) {
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

void SetDetectorsInfo::
loadSectionNode(const boost::property_tree::ptree& sectionNode,
                DetectorBasedChannelID detectorChannelID)
{
  using boost::property_tree::ptree;
  
  const int detectorID = detectorChannelID.Detector();
  const int section = detectorChannelID.Section();
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

  ChannelNodeProperties commonProperties;
  for (const ptree::value_type& vvv: sectionNode.get_child("")) {
    if (vvv.first == "common") {
      const ptree& channelNode = vvv.second;
      extractChannelProperties(channelNode, commonProperties);
    }
    if (vvv.first == "channel") {
      const ptree& channelNode = vvv.second;
      ChannelNodeProperties channelProperties(commonProperties);
      extractChannelProperties(channelNode, channelProperties);
      if (isMCSimulation()) {
        setupChannelProperties(section, channelProperties, ds);
      }
      else {
        setupChannelProperties(section, channelProperties, detector);
      }
    }
  }
}

void SetDetectorsInfo::
extractChannelProperties(const boost::property_tree::ptree& channelNode,
                         ChannelNodeProperties& channel_properties)
{
  using boost::property_tree::ptree;
  using boost::optional;
  
  const optional<int> channel = channelNode.get_optional<int>("<xmlattr>.id");
  if (channel) { channel_properties.id = *channel; }

  for (const ptree::value_type& v: channelNode.get_child("")) {
    if (v.first == "disabled") {
      const int disabled = v.second.get_value<int>();
      channel_properties.disabled = disabled;
    }
    else if (v.first == "noise") {
      const optional<double> noiseParam0 = v.second.get_optional<double>("param0");
      const optional<double> noiseParam1 = v.second.get_optional<double>("param1");
      const optional<double> noiseParam2 = v.second.get_optional<double>("param2");
      if (noiseParam0) { channel_properties.noise_param0 = *noiseParam0; }
      if (noiseParam1) { channel_properties.noise_param0 = *noiseParam1; }
      if (noiseParam2) { channel_properties.noise_param0 = *noiseParam2; }
    }
    else if (v.first == "threshold") {
      const double threshold = v.second.get_value<double>();
      channel_properties.threshold = threshold;
    }
    else if (v.first == "trigger_discrimination") {
      const optional<double> triggerDiscriminationCenter = v.second.get_optional<double>("center");
      const optional<double> triggerDiscriminationSigma = v.second.get_optional<double>("sigma");
      if (triggerDiscriminationCenter) {
        channel_properties.trigger_discrimination_center = *triggerDiscriminationCenter;
      }
      if (triggerDiscriminationSigma) {
        channel_properties.trigger_discrimination_sigma = *triggerDiscriminationSigma;
      }
    }
    else if (v.first == "compensation_factor") {
      const double compensationFactor = v.second.get_value<double>();
      channel_properties.compensation_factor = compensationFactor;
    }
    else if (v.first == "threshold") {
      const double threshold = v.second.get_value<double>();
      channel_properties.threshold = threshold;
    }
  }
}

void SetDetectorsInfo::setupChannelProperties(int section,
                                             const ChannelNodeProperties& channel_properties,
                                             DeviceSimulation* ds)
{
  const SectionChannelPair channelID(section, channel_properties.id);
  ds->setChannelDisabled(channelID, channel_properties.disabled);
  ds->setNoiseParam(channelID,
                    NoiseParam_t(channel_properties.noise_param0,
                                 channel_properties.noise_param1,
                                 channel_properties.noise_param2));
  ds->setThreshold(channelID, channel_properties.threshold);
  ds->setTriggerDiscriminationCenter(channelID, channel_properties.trigger_discrimination_center);
  ds->setTriggerDiscriminationSigma(channelID, channel_properties.trigger_discrimination_sigma);
  ds->setEPICompensationFactor(channelID, channel_properties.compensation_factor);
}

void SetDetectorsInfo::setupChannelProperties(int section,
                                             const ChannelNodeProperties& channel_properties,
                                             VRealDetectorUnit* detector)
{
  const int channel = channel_properties.id;
  MultiChannelData* mcd = detector->getMultiChannelData(section);
  mcd->setChannelDisabled(channel, channel_properties.disabled);
  mcd->setThresholdEnergy(channel, channel_properties.threshold);
}

} /*namespace comptonsoft */
