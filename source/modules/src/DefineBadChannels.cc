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

#include "DefineBadChannels.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/format.hpp>

#include "ChannelID.hh"
#include "MultiChannelData.hh"
#include "VRealDetectorUnit.hh"
#include "DeviceSimulation.hh"

using namespace anl;

namespace comptonsoft
{

DefineBadChannels::DefineBadChannels()
  : m_Filename("bad_channel.txt")
{
}

DefineBadChannels::~DefineBadChannels() = default;

ANLStatus DefineBadChannels::mod_startup()
{
  register_parameter(&m_Filename, "filename");
  return AS_OK;
}

ANLStatus DefineBadChannels::mod_init()
{
  VCSModule::mod_init();
  
  if (m_Filename[0] == '0') {
    std::cout << "DefineBadChannels: There is no bad channel." << std::endl;
    return AS_OK;
  }

  std::ifstream fin;
  fin.open(m_Filename.c_str());
  if (!fin) {
    std::cout << "DefineBadChannels: file cannot open" << std::endl;
    return AS_QUIT;
  }

  std::cout << "Bad Channel List:" << std::endl;
  std::vector<DetectorChannelID> badChannels;
  while (1) {
    int detector, section, channel;
    char buf[64];
    fin.getline(buf, 64);
    if(fin.eof()) { break; }
    if (buf[0] == '#') { continue; }
    std::istringstream iss(buf);
    iss >> detector >> section >> channel;
    badChannels.push_back(DetectorChannelID(detector, section, channel));
    std::cout << boost::format("%4d %4d %4d")%detector%section%channel << std::endl;
  }
  fin.close();

  DetectorSystem* detectorManager = getDetectorManager();
  if (detectorManager->isMCSimulation()) {
    for (auto channel: badChannels) {
      detectorManager->getDeviceSimulationByID(channel.Detector())
        ->setBrokenChannel(channel.Channel(), 1);
    }
  }
  else {
    for (auto channel: badChannels) {
      detectorManager->getDetectorByID(channel.Detector())
        ->getMultiChannelData(channel.Section())
        ->setBadChannel(channel.Index(), 1);
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
