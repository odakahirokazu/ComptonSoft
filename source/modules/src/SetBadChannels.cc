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

#include "SetBadChannels.hh"

#include <iostream>
#include <functional>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "VRealDetectorUnit.hh"
#include "MultiChannelData.hh"
#include "DeviceSimulation.hh"

using namespace anlnext;

namespace comptonsoft
{

SetBadChannels::SetBadChannels()
  : m_FileName("")
{
}

SetBadChannels::~SetBadChannels() = default;

ANLStatus SetBadChannels::mod_define()
{
  register_parameter(&m_FileName, "filename");
  
  return AS_OK;
}

ANLStatus SetBadChannels::mod_initialize()
{
  VCSModule::mod_initialize();

  bool rval = set_by_file();
  if (!rval) {
    return AS_QUIT;
  }

  return AS_OK;
}

bool SetBadChannels::set_by_file()
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

  DetectorSystem* detectorManager = getDetectorManager();
  const bool isSimulation = detectorManager->isMCSimulation();
  
  for (ptree::value_type& v: pt.get_child("badchannels.data")) {
    if (v.first == "detector") {
      const ptree detectorNode = v.second;
      const int detectorID = detectorNode.get<int>("<xmlattr>.id");
      for (const ptree::value_type& vv: detectorNode.get_child("")) {
        if (vv.first == "section") {
          const ptree sectionNode = vv.second;
          const int section = sectionNode.get<int>("<xmlattr>.id");
          for (const ptree::value_type& vvv: sectionNode.get_child("")) {
            if (vvv.first == "channel") {
              const ptree channelNode = vvv.second;
              const int channel = channelNode.get<int>("<xmlattr>.id");
              const int status = channelNode.get<int>("<xmlattr>.status");
              if (isSimulation) {
                detectorManager->getDeviceSimulationByID(detectorID)
                  ->setChannelDisabled(SectionChannelPair(section, channel), status);
              }
              else {
                detectorManager->getDetectorByID(detectorID)
                  ->getMultiChannelData(section)
                  ->setChannelDisabled(channel, status);
              }
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
          
          for (const ptree::value_type& vvv: sectionNode.get_child("")) {
            if (vvv.first == "channel") {
              const ptree channelNode = vvv.second;
              const int channel = channelNode.get<int>("<xmlattr>.id");
              const int status = channelNode.get<int>("<xmlattr>.status");
              if (isSimulation) {
                detectorManager->getDeviceSimulationByID(detectorID)
                  ->setChannelDisabled(SectionChannelPair(det_section, channel), status);
              }
              else {
                detectorManager->getDetectorByID(detectorID)
                  ->getMultiChannelData(det_section)
                  ->setChannelDisabled(channel, status);
              }
            }
          }
        }
      }
    }
  }
  
  return true;
}

} /*namespace comptonsoft */
