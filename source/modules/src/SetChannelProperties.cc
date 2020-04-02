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

#include "AstroUnits.hh"

#include "VRealDetectorUnit.hh"
#include "MultiChannelData.hh"
#include "FrameData.hh"
#include "GainFunctionCubic.hh"
#include "VChannelMap.hh"
#include "SimDetectorUnit2DPixel.hh"
#include "SimDetectorUnit2DStrip.hh"
#include "SimDetectorUnitScintillator.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

SetChannelProperties::SetChannelProperties()
  : m_Filename("channel_properties.xml")
{
}

SetChannelProperties::~SetChannelProperties() = default;

ANLStatus SetChannelProperties::mod_define()
{
  register_parameter(&m_Filename, "filename");
  return AS_OK;
}

ANLStatus SetChannelProperties::mod_initialize()
{
  VCSModule::mod_initialize();
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
        if (vv.first == "frame") {
          const ptree& frameNode = vv.second;
          DetectorBasedChannelID detectorChannelID(detectorID);
          loadFrameNode(frameNode, detectorChannelID);
        }
      }
    }
    else if (v.first == "readout_module") {
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
          const SectionChannelPair sectionChannelID(section, i);
          if (isMCSimulation()) {
            setupChannelProperties(sectionChannelID, commonProperties, ds);
          }
          else {
            setupChannelProperties(sectionChannelID, commonProperties, detector);
          }
        }
      }
    }
    else if (vvv.first == "channel") {
      const ptree& channelNode = vvv.second;
      DetectorSystem::ChannelNodeContents channelProperties(commonProperties);
      channelProperties.load(channelNode);

      if (channelProperties.id == boost::none) {
        const std::string message
          = "channel node must have 'id' attribute.";
        BOOST_THROW_EXCEPTION(ANLException(message));
      }

      const int channel = *(channelProperties.id);
      const SectionChannelPair sectionChannelID(section, channel);

      if (isMCSimulation()) {
        setupChannelProperties(sectionChannelID, channelProperties, ds);
      }
      else {
        setupChannelProperties(sectionChannelID, channelProperties, detector);
      }
    }
  }
}

void SetChannelProperties::
loadFrameNode(const boost::property_tree::ptree& frameNode,
              DetectorBasedChannelID detectorChannelID)
{
  using boost::property_tree::ptree;
  using boost::optional;
  
  const int detectorID = detectorChannelID.Detector();
  const optional<int> all = frameNode.get_optional<int>("<xmlattr>.all");

  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID);
  DeviceSimulation* ds = nullptr;

  if (!detector->checkType(DetectorType::PixelDetector)) {
    const std::string message
      = (boost::format("frame node must be contained in a pixel detector. Detector ID: %d") % detectorID).str();
    BOOST_THROW_EXCEPTION(ANLException(message));
  }
  
  if (isMCSimulation()) {
    ds = dynamic_cast<DeviceSimulation*>(detector);
    if (ds == nullptr) {
      const std::string message
        = (boost::format("can not get DeviceSimulation. Detector ID: %d") % detectorID).str();
      BOOST_THROW_EXCEPTION(ANLException(message));
    }
  }

  DetectorSystem::ChannelNodeContents commonProperties;
  for (const ptree::value_type& vvv: frameNode.get_child("")) {
    if (vvv.first == "common") {
      const ptree& channelNode = vvv.second;
      commonProperties.load(channelNode);

      if (all && (*all==1)) {
        const int NumPixelsX = detector->getNumPixelX();
        const int NumPixelsY = detector->getNumPixelY();
        for (int ix=0; ix<NumPixelsX; ix++) {
          for (int iy=0; iy<NumPixelsY; iy++) {
            const PixelID pixelID(ix, iy);
            if (isMCSimulation()) {
              setupChannelProperties(pixelID, commonProperties, ds);
            }
            else {
              setupChannelProperties(pixelID, commonProperties, detector);
            }
          }
        }
      }
    }
    else if (vvv.first == "pixel") {
      const ptree& channelNode = vvv.second;
      DetectorSystem::ChannelNodeContents channelProperties(commonProperties);
      channelProperties.load(channelNode);

      if (channelProperties.x == boost::none || channelProperties.y == boost::none) {
        const std::string message
          = "pixel node must have 'x' and 'y' attributes.";
        BOOST_THROW_EXCEPTION(ANLException(message));
      }

      const int ix = *(channelProperties.x);
      const int iy = *(channelProperties.y);
      const PixelID pixelID(ix, iy);

      if (isMCSimulation()) {
        setupChannelProperties(pixelID, channelProperties, ds);
      }
      else {
        setupChannelProperties(pixelID, channelProperties, detector);
      }
    }
  }
}

void SetChannelProperties::setupChannelProperties(SectionChannelPair channelID,
                                                  const DetectorSystem::ChannelNodeContents& properties,
                                                  DeviceSimulation* ds)
{
  const PixelID pixel = ds->ChannelToPixel(channelID);
  setupChannelProperties(pixel, properties, ds);
}

void SetChannelProperties::setupChannelProperties(PixelID pixelID,
                                                  const DetectorSystem::ChannelNodeContents& properties,
                                                  DeviceSimulation* ds)
{
  using boost::optional;
  
  if (optional<int> o = properties.disable_status) {
    ds->setChannelDisabled(pixelID, *o);
  }
  if (optional<double> o = properties.noise_level_param0) {
    ds->setNoiseParam0(pixelID, *o);
  }
  if (optional<double> o = properties.noise_level_param1) {
    ds->setNoiseParam1(pixelID, *o);
  }
  if (optional<double> o = properties.noise_level_param2) {
    ds->setNoiseParam2(pixelID, *o);
  }
  if (optional<double> o = properties.trigger_discrimination_center) {
    const double value = (*o) * unit::keV;
    ds->setTriggerDiscriminationCenter(pixelID, value);
  }
  if (optional<double> o = properties.trigger_discrimination_sigma) {
    const double value = (*o) * unit::keV;
    ds->setTriggerDiscriminationSigma(pixelID, value);
  }
  if (optional<double> o = properties.compensation_factor) {
    ds->setEPICompensationFactor(pixelID, *o);
  }
  if (optional<double> o = properties.threshold_value) {
    const double value = (*o) * unit::keV;
    ds->setThreshold(pixelID, value);
  }
}

void SetChannelProperties::setupChannelProperties(SectionChannelPair channelID,
                                                  const DetectorSystem::ChannelNodeContents& properties,
                                                  VRealDetectorUnit* detector)
{
  using boost::optional;

  const int section = channelID.Section();
  const int channel = channelID.Channel();
  MultiChannelData* mcd = detector->getMultiChannelData(section);

  if (optional<int> status = properties.disable_status) {
    mcd->setChannelDisabled(channel, *status);
  }
  if (optional<double> o = properties.pedestal_value) {
    const double value = (*o);
    mcd->setPedestal(channel, value);
  }
  if (optional<double> o = properties.gain_correction_c0) {
    const double c0 = (*o);
    auto gainFunction = std::make_shared<GainFunctionCubic>();
    double c1 = 0.0;
    double c2 = 0.0;
    double c3 = 0.0;
    if (optional<double> o = properties.gain_correction_c1) {
      c1 = (*o);
    }
    if (optional<double> o = properties.gain_correction_c2) {
      c2 = (*o);
    }
    if (optional<double> o = properties.gain_correction_c3) {
      c3 = (*o);
    }
    gainFunction->set(c0, c1, c2, c3);
    mcd->setGainFunction(channel, gainFunction);
  }
  if (optional<double> o = properties.threshold_value) {
    const double value = (*o) * unit::keV;
    mcd->setThresholdEnergy(channel, value);
  }
}

void SetChannelProperties::setupChannelProperties(PixelID pixelID,
                                                  const DetectorSystem::ChannelNodeContents& properties,
                                                  VRealDetectorUnit* detector)
{
  if (detector->hasFrameData()) {
    setupFramePixelProperties(pixelID, properties, detector);
  }
  else {
    const SectionChannelPair channelID = detector->PixelToChanelID(pixelID);
    setupChannelProperties(channelID, properties, detector);
  }
}

void SetChannelProperties::setupFramePixelProperties(PixelID pixelID,
                                                     const DetectorSystem::ChannelNodeContents& properties,
                                                     VRealDetectorUnit* detector)
{
  using boost::optional;

  const int ix = pixelID.X();
  const int iy = pixelID.Y();
  FrameData* frame = detector->getFrameData();

  if (optional<int> status = properties.disable_status) {
    frame->setDisabledPixel(ix, iy, *status);
  }
  if (optional<double> o = properties.pedestal_value) {
    const double value = (*o);
    frame->setPedestal(ix, iy, value);
  }
  if (optional<double> o = properties.gain_correction_c0) {
    const double c0 = (*o);
    auto gainFunction = std::make_shared<GainFunctionCubic>();
    double c1 = 0.0;
    double c2 = 0.0;
    double c3 = 0.0;
    if (optional<double> o = properties.gain_correction_c1) {
      c1 = (*o);
    }
    if (optional<double> o = properties.gain_correction_c2) {
      c2 = (*o);
    }
    if (optional<double> o = properties.gain_correction_c3) {
      c3 = (*o);
    }
    gainFunction->set(c0, c1, c2, c3);
    frame->setGainFunction(ix, iy, gainFunction);
  }
  if (optional<double> o = properties.threshold_value) {
#if 0
    const double value = (*o) * unit::keV;
    frame->setThresholdEnergy(ix, iy, value);
#else
    const std::string message
      = (boost::format("frame/pixel node can not have a threshold value. Detector %d (%d, %d)") % (detector->getID()) % ix % iy).str();
    BOOST_THROW_EXCEPTION(ANLException(message));
#endif
  }
}

} /*namespace comptonsoft */
