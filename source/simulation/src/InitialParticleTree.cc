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

#include "InitialParticleTree.hh"
#include "TTree.h"
#include "InitialInformation.hh"

using namespace comptonsoft;
using namespace anl;

InitialParticleTree::InitialParticleTree()
  : eventid(0),
    ini_energy(0.0), ini_dirx(0.0), ini_diry(0.0), ini_dirz(0.0),
    ini_time(0.0), ini_posx(0.0), ini_posy(0.0), ini_posz(0.0),
    ini_polarx(0.0), ini_polary(0.0), ini_polarz(0.0),
    weight(1.0),
    initial_info(0), polarization_enable(false)
{
}


ANLStatus InitialParticleTree::mod_init()
{
  VCSModule::mod_init();
  GetANLModuleIF("InitialInformation", &initial_info);
  return AS_OK;
}


ANLStatus InitialParticleTree::mod_his()
{
  VCSModule::mod_his();

  tree = new TTree("ini_tree", "ini_tree");

  tree->Branch("eventid",  &eventid,   "eventid/I");
  tree->Branch("ini_energy", &ini_energy,  "ini_energy/D");
  tree->Branch("ini_dirx", &ini_dirx,  "ini_dirx/D");
  tree->Branch("ini_diry", &ini_diry,  "ini_diry/D");
  tree->Branch("ini_dirz", &ini_dirz,  "ini_dirz/D");
  tree->Branch("ini_time", &ini_time,  "ini_time/D");
  tree->Branch("ini_posx", &ini_posx,  "ini_posx/D");
  tree->Branch("ini_posy", &ini_posy,  "ini_posy/D");
  tree->Branch("ini_posz", &ini_posz,  "ini_posz/D");
  tree->Branch("weight",   &weight,    "weight/D");

  if (Evs("Polarization enable")) {
    polarization_enable = true;
    tree->Branch("ini_polarx", &ini_polarx,  "ini_polarx/D");
    tree->Branch("ini_polary", &ini_polary,  "ini_polary/D");
    tree->Branch("ini_polarz", &ini_polarz,  "ini_polarz/D");
  }
  
  return AS_OK;
}


ANLStatus InitialParticleTree::mod_ana()
{
  ini_energy = initial_info->InitialEnergy();
  G4ThreeVector iniDir = initial_info->InitialDirection();
  ini_dirx = iniDir.x();
  ini_diry = iniDir.y();
  ini_dirz = iniDir.z();
  ini_time = initial_info->InitialTime();
  G4ThreeVector iniPos = initial_info->InitialPosition();
  ini_posx = iniPos.x();
  ini_posy = iniPos.y();
  ini_posz = iniPos.z();
  
  if (polarization_enable) {
    G4ThreeVector iniPolar = initial_info->InitialPolarization();
    ini_polarx = iniPolar.x();
    ini_polary = iniPolar.y();
    ini_polarz = iniPolar.z();
  }

  weight = initial_info->Weight();
  eventid = initial_info->EventID();
  
  tree->Fill();
  
  return AS_OK;
}
