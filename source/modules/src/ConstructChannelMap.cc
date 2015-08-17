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
{}

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

  std::map<std::string, int> detPrefixMap;

  ptree pt;
  try {
    read_xml(filename_, pt);
  }
  catch (boost::property_tree::xml_parser_error& ex) {
    std::cout << "Cannot parse: " << filename_ << std::endl;
    std::cout << "xml_parser_error ===> \n"
              << "message: " << ex.message() << "\n"
              << "filename: " << ex.filename() << "\n"
              << "line: " << ex.line() << std::endl;
    return AS_QUIT_ERR;
  }

  int detIndex = 0;
  for (ptree::value_type& v: pt.get_child("channel_map")) {
    if (v.first == "detector" || v.first=="detector_2dstrip") {
      const ptree detNode = v.second;
      const optional<std::string> type_opt = detNode.get_optional<std::string>("<xmlattr>.type");
      const std::string type = type_opt ? (*type_opt) : "";
      const std::string name = detNode.get<std::string>("<xmlattr>.name");
      detPrefixMap.insert( std::make_pair(name, detIndex) );
      const int numSections = detNode.get<int>("<xmlattr>.num_sections");
      const int numChannels = detNode.get<int>("<xmlattr>.num_channels");
      const int numX = detNode.get<int>("<xmlattr>.num_x");
      const int numY = detNode.get<int>("<xmlattr>.num_y");

      std::shared_ptr<VChannelMap> channelMap;
      const bool isDSD = (type == "2DStrip");
      if (isDSD) {
        channelMap.reset(new ChannelMapDSD(numSections, numChannels, numX, numY));
      }
      else {
        channelMap.reset(new ChannelMap(numSections, numChannels, numX, numY));
      }
      channelMaps_.push_back(channelMap);

      for (const ptree::value_type& vv: detNode.get_child("")) {
        if (vv.first == "section") {
          const ptree sectionNode = vv.second;
          const int section = sectionNode.get<int>("<xmlattr>.id");
          for (const ptree::value_type& vvv: sectionNode.get_child("")) {
            if (vvv.first == "channel") {
              const ptree channelNode = vvv.second;
              const int channel = channelNode.get<int>("<xmlattr>.id");
              const int x = channelNode.get<int>("x");
              const int y = channelNode.get<int>("y");
              channelMap->set(section, channel, x, y);
            }
          }
        }
      }
      detIndex++;
    }
  }
  
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    std::string prefix = detector->getNamePrefix();
    if (detPrefixMap.count(prefix) == 0) {
      std::cout << "ConstructChannelMap: Not found: " << prefix << std::endl;
      return AS_QUIT;
    }
    detector->setChannelMap(channelMaps_[detPrefixMap[prefix]]);
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
