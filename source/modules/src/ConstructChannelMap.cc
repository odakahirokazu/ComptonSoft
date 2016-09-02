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

#include "ConstructChannelMap.hh"

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "ChannelMap.hh"
#include "ChannelMapDSD.hh"
#include "VRealDetectorUnit.hh"
#include "DetectorSystem.hh"

using namespace anl;

namespace comptonsoft
{

ConstructChannelMap::ConstructChannelMap()
  : filename_("channel_map.xml")
{
}

ConstructChannelMap::~ConstructChannelMap() = default;

ANLStatus ConstructChannelMap::mod_startup()
{
  register_parameter(&filename_, "filename");
  set_parameter_description("XML data file describing a channel map.");
  return AS_OK;
}

ANLStatus ConstructChannelMap::mod_init()
{
  using boost::property_tree::ptree;
  using boost::optional;

  VCSModule::mod_init();

  ptree pt;
  try {
    read_xml(filename_, pt);
    loadChannelMap(pt);
  }
  catch (boost::property_tree::xml_parser_error& ex) {
    std::cout << "Cannot parse: " << filename_ << std::endl;
    std::cout << "xml_parser_error ===> \n"
              << "message: " << ex.message() << "\n"
              << "filename: " << ex.filename() << "\n"
              << "line: " << ex.line() << std::endl;
    return AS_QUIT_ERR;
  }

  return AS_OK;
}

void ConstructChannelMap::loadChannelMap(const boost::property_tree::ptree& pt)
{
  using boost::property_tree::ptree;
  
  for (const ptree::value_type& v: pt.get_child("channel_map")) {
    if (v.first == "detector") {
      const ptree& detectorNode = v.second;
      const std::string type = detectorNode.get<std::string>("<xmlattr>.type");
      const std::string prefix = detectorNode.get<std::string>("<xmlattr>.prefix");
      const int numSections = detectorNode.get<int>("<xmlattr>.num_sections");
      const int numChannels = detectorNode.get<int>("<xmlattr>.num_channels");
      const int numX = detectorNode.get<int>("<xmlattr>.num_x");
      const int numY = detectorNode.get<int>("<xmlattr>.num_y");
      const bool isDSD = (type == "2DStrip");

      std::shared_ptr<VChannelMap> channelMap;
      if (isDSD) {
        channelMap.reset(new ChannelMapDSD(numSections, numChannels, numX, numY));
      }
      else {
        channelMap.reset(new ChannelMap(numSections, numChannels, numX, numY));
      }

      for (const ptree::value_type& vv: detectorNode.get_child("")) {
        if (vv.first == "channel") {
          const ptree& channelNode = vv.second;
          const int section = channelNode.get<int>("<xmlattr>.section");
          const int channel = channelNode.get<int>("<xmlattr>.index");
          const int x = channelNode.get<int>("<xmlattr>.x");
          const int y = channelNode.get<int>("<xmlattr>.y");
          channelMap->set(section, channel, x, y);
        }
      }

      channelMaps_[prefix] = channelMap;
    }
  }
  
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    const std::string prefix = detector->getNamePrefix();
    if (channelMaps_.count(prefix) == 0) {
      std::ostringstream message;
      message << "Channel map is not found for Detector prefix: " << prefix << std::endl;
      BOOST_THROW_EXCEPTION(ANLException(this, message.str()));
    }
    detector->setChannelMap(channelMaps_[prefix]);
  }
}

} /* namespace comptonsoft */
