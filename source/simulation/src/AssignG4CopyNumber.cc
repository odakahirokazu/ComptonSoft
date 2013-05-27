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

#include "AssignG4CopyNumber.hh"
#include <fstream>
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"


using namespace anl;
using namespace comptonsoft;


AssignG4CopyNumber::AssignG4CopyNumber()
  : m_FileName("copyno.txt")
{
}


AssignG4CopyNumber::~AssignG4CopyNumber()
{
}


ANLStatus AssignG4CopyNumber::mod_startup()
{
  register_parameter(&m_FileName, "File name");

  return AS_OK;
}


ANLStatus AssignG4CopyNumber::mod_bgnrun()
{
  typedef std::map<std::string, int> volume_map_t;
  typedef volume_map_t::const_iterator volume_map_iter;
  volume_map_t volumeMap;

  std::ifstream fin(m_FileName.c_str());
  if (!fin) {
    std::cout << "AssignG4CopyNumber: file cannot open" << std::endl;
    return AS_QUIT_ERR;
  }

  const size_t BufSize = 256;
  char buf[BufSize];
  while (1) {
    std::string name;
    int copyNo;
    fin.getline(buf, BufSize);
    if (fin.eof()) { break; }
    if (buf[0] == '#') { continue; }
    
    std::istringstream iss(buf);
    iss >> name;
    iss >> copyNo;
    if (iss) {
      volumeMap[name] = copyNo;
    }
  }
  fin.close();
  
  G4PhysicalVolumeStore* store = G4PhysicalVolumeStore::GetInstance();
  for (volume_map_iter it=volumeMap.begin(); it != volumeMap.end(); ++it) {
    G4VPhysicalVolume* volume = store->GetVolume((*it).first);
    if (volume) {
      volume->SetCopyNo((*it).second);
    }
  }

  return AS_OK;
}
