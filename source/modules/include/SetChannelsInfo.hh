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

#ifndef COMPTONSOFT_SetChannelsInfo_H
#define COMPTONSOFT_SetChannelsInfo_H 1

#include "VCSModule.hh"
#include <map>
#include <tuple>
#include <boost/property_tree/ptree.hpp>

namespace comptonsoft {

class DeviceSimulation;


class SetChannelsInfo : public VCSModule
{
  DEFINE_ANL_MODULE(SetChannelsInfo, 3.0);
public:
  SetChannelsInfo();
  ~SetChannelsInfo();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();

private:
  bool set_by_map();
  bool set_by_file();

  void setupChannelInfo(int section,
                        const boost::property_tree::ptree& channelNode,
                        DeviceSimulation* ds);
  
private:
  bool m_ByFile;
  std::string m_Filename;
  
  std::map<std::string,
           std::tuple<int,
                      double, double, double, double,
                      double, double, double, double>> m_Map;
  int m_DetectorType;
  double m_Side1NoiseParam0;
  double m_Side1NoiseParam1;
  double m_Side1NoiseParam2;
  double m_Side1CompensationFactor;
  double m_Side2NoiseParam0;
  double m_Side2NoiseParam1;
  double m_Side2NoiseParam2;
  double m_Side2CompensationFactor;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SetChannelsInfo_H */
