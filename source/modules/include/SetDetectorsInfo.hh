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

#ifndef COMPTONSOFT_SetDetectorsInfo_H
#define COMPTONSOFT_SetDetectorsInfo_H 1

#include "VCSModule.hh"
#include <map>
#include <tuple>
#include <boost/property_tree/ptree.hpp>

namespace comptonsoft {

class DeviceSimulation;


class SetDetectorsInfo : public VCSModule
{
  DEFINE_ANL_MODULE(SetDetectorsInfo, 3.0);

  struct ChannelNodeProperties {
  public:
    int id = 0;
    int disabled = 0;
    double noise_param0 = 0.0;
    double noise_param1 = 0.0;
    double noise_param2 = 0.0;
    double trigger_discrimination_center = 0.0;
    double trigger_discrimination_sigma = 0.0;
    double compensation_factor = 1.0;
    double threshold = 0.0;
  };

public:
  SetDetectorsInfo();
  ~SetDetectorsInfo();

  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_init() override;

private:
  void readFile();
  void loadRootNode(const boost::property_tree::ptree& rootNode);
  void loadSectionNode(const boost::property_tree::ptree& sectionNode,
                       DetectorBasedChannelID detectorChannelID);
  void extractChannelProperties(const boost::property_tree::ptree& channelNode,
                                ChannelNodeProperties& channel_properties);
  void setupChannelProperties(int section,
                              const ChannelNodeProperties& channel_properties,
                              DeviceSimulation* ds);
  void setupChannelProperties(int section,
                              const ChannelNodeProperties& channel_properties,
                              VRealDetectorUnit* detector);
  
private:
  std::string m_Filename;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SetDetectorsInfo_H */
