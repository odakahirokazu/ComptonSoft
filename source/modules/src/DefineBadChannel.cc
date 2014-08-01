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

#include "DefineBadChannel.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "OneASICData.hh"
#include "RealDetectorUnit.hh"
#include "ChannelID.hh"


using namespace comptonsoft;


using namespace anl;

DefineBadChannel::DefineBadChannel()
  : m_FileName("bad_channel.txt")
{
}


ANLStatus DefineBadChannel::mod_startup()
{
  register_parameter(&m_FileName, "filename");

  return AS_OK;
}


ANLStatus DefineBadChannel::mod_init()
{
  VCSModule::mod_init();
  
  if (m_FileName[0] == '0') {
    std::cout << "DefineBadChannel: There is no bad channel." << std::endl;
    return AS_OK;
  }

  std::ifstream fin;
  fin.open(m_FileName.c_str());
  if (!fin) {
    std::cout << "DefineBadChannel: file cannot open" << std::endl;
    return AS_QUIT;
  }

  std::cout << "Bad Channel List:" << std::endl;
  std::vector<ChannelID> badChannel;
  while (1) {
    int detid, chipid, ch;
    char buf[64];
    fin.getline(buf, 64);
    if(fin.eof()) { break; }
    if (buf[0] == '#') { continue; }
    std::istringstream iss(buf);
    iss >> detid >> chipid >> ch;
    badChannel.push_back(ChannelID(detid, chipid, ch));
    std::cout << detid << "    " << chipid << "    " << ch << std::endl;
  }
 
  std::vector<ChannelID>::iterator it;
  it = badChannel.begin();
  while (it != badChannel.end()) {
    GetDetectorManager()->getDetectorByID(it->Module())
      ->getASICData(it->Chip())->setBadChannel(it->Channel(), 1);
    it++;
  }

  fin.close();

  return AS_OK;
}
