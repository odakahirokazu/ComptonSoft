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

#include "DumpMass.hh"
#include <fstream>
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"

using namespace anl;
using namespace comptonsoft;


DumpMass::DumpMass()
  : m_FileName("mass_volumes.txt")
{
}


DumpMass::~DumpMass()
{
}


ANLStatus DumpMass::mod_startup()
{
  register_parameter(&m_FileName, "filename");
  return AS_OK;
}


ANLStatus DumpMass::mod_bgnrun()
{
  typedef G4LogicalVolumeStore::iterator LVIter;

  std::ofstream fout(m_FileName.c_str());
  G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  for (LVIter it=store->begin(); it!=store->end(); ++it) {
    G4LogicalVolume* lv = *it;
    fout << lv->GetName() << "    " << lv->GetMass()/g << std::endl;
  }
  fout.close();

  return AS_OK;
}
