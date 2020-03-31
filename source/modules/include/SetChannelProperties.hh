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

#ifndef COMPTONSOFT_SetChannelProperties_H
#define COMPTONSOFT_SetChannelProperties_H 1

#include "VCSModule.hh"
#include <map>
#include <tuple>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace comptonsoft {

class DeviceSimulation;


class SetChannelProperties : public VCSModule
{
  DEFINE_ANL_MODULE(SetChannelProperties, 4.0);
public:
  SetChannelProperties();
  ~SetChannelProperties();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;

private:
  void readFile();
  void loadRootNode(const boost::property_tree::ptree& rootNode);
  void loadSectionNode(const boost::property_tree::ptree& sectionNode,
                       DetectorBasedChannelID detectorChannelID);
  void loadFrameNode(const boost::property_tree::ptree& frameNode,
                     DetectorBasedChannelID detectorChannelID);

  void setupChannelProperties(SectionChannelPair channelID,
                              const DetectorSystem::ChannelNodeContents& properties,
                              DeviceSimulation* ds);
  void setupChannelProperties(PixelID pixelID,
                              const DetectorSystem::ChannelNodeContents& properties,
                              DeviceSimulation* ds);
  void setupChannelProperties(SectionChannelPair channelID,
                              const DetectorSystem::ChannelNodeContents& properties,
                              VRealDetectorUnit* detector);
  void setupChannelProperties(PixelID pixelID,
                              const DetectorSystem::ChannelNodeContents& properties,
                              VRealDetectorUnit* detector);
  void setupFramePixelProperties(PixelID pixelID,
                                 const DetectorSystem::ChannelNodeContents& properties,
                                 VRealDetectorUnit* detector);
  
private:
  std::string m_Filename;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SetChannelProperties_H */
