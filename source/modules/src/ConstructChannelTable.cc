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

#include "ConstructChannelTable.hh"

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "RealDetectorUnit.hh"

using namespace comptonsoft;
using namespace anl;


ConstructChannelTable::ConstructChannelTable()
  : m_FileName("channeltable.xml")
{}


ANLStatus ConstructChannelTable::mod_startup()
{
  register_parameter(&m_FileName, "filename");
  set_parameter_description("XML data file describing a channel table.");
  return AS_OK;
}


ANLStatus ConstructChannelTable::mod_init()
{
  using boost::property_tree::ptree;

  VCSModule::mod_init();

  std::map<std::string, int> detPrefixMap;

  ptree pt;
  try {
    read_xml(m_FileName, pt);
  }
  catch (boost::property_tree::xml_parser_error&) {
    std::cout << "cannot parse: " << m_FileName << std::endl;
    return AS_QUIT;
  }

  m_ChannelTableVec.clear();
  
  int detIndex = 0;
  foreach (ptree::value_type& v, pt.get_child("channel_map")) {
    if (v.first == "detector") {
      ptree detNode = v.second;
      ChannelTable* channelTable = new ChannelTable;
      m_ChannelTableVec.push_back( channelTable );
      
      std::string name = detNode.get<std::string>("<xmlattr>.name");
      detPrefixMap.insert( std::make_pair(name, detIndex) );
      
      int totalChip = detNode.get<int>("<xmlattr>.total");
      channelTable->setNumChip(totalChip);
      
      foreach (ptree::value_type& vv, detNode.get_child("")) {
        if (vv.first == "chip") {
          ptree chipNode = vv.second;
          int chipid = chipNode.get<int>("<xmlattr>.id");
          int totalChannel = chipNode.get<int>("<xmlattr>.total");
          channelTable->setNumChannel(chipid, totalChannel);
          
          foreach (ptree::value_type& vvv, chipNode.get_child("")) {
            if (vvv.first == "channel") {
              ptree channelNode = vvv.second;
              int ch = channelNode.get<int>("<xmlattr>.id");
              int px = channelNode.get<int>("x");
              int py = channelNode.get<int>("y");
              channelTable->set(chipid, ch, px, py);
            }
          }
        }
      }
      detIndex++;
    }
  }

  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd;

  itDet = GetDetectorVector().begin();
  itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::string prefix = (*itDet)->getNamePrefix();
    if (detPrefixMap.count(prefix) == 0) {
      std::cout << "ConstructChannelTable: Not found: " << prefix << std::endl;
      return AS_QUIT;
    }
    (*itDet)->registerChannelTable( m_ChannelTableVec[ detPrefixMap[prefix] ] );

    itDet++;
  }
  
  return AS_OK;
}


ANLStatus ConstructChannelTable::mod_exit()
{
  for (size_t i=0; i<m_ChannelTableVec.size(); i++) {
    delete m_ChannelTableVec[i];
    m_ChannelTableVec[i] = 0;
  }
  m_ChannelTableVec.clear();

  return AS_OK;
}
