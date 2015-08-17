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

#include "ComptonEventTreeWithInitialInfo.hh"
#include "TTree.h"
#include "InitialInformation.hh"

using namespace anl;

namespace comptonsoft
{

ComptonEventTreeWithInitialInfo::ComptonEventTreeWithInitialInfo()
  : initialInfo_(nullptr), polarizationEnable_(false)
{
}

ANLStatus ComptonEventTreeWithInitialInfo::mod_init()
{
  ComptonEventTree::mod_init();
  GetANLModuleIF("InitialInformation", &initialInfo_);
  
  return AS_OK;
}

ANLStatus ComptonEventTreeWithInitialInfo::mod_his()
{
  ComptonEventTree::mod_his();

  cetree_->Branch("ini_energy", &ini_energy_, "ini_energy/F");
  cetree_->Branch("ini_dirx",   &ini_dirx_,   "ini_dirx/F");
  cetree_->Branch("ini_diry",   &ini_diry_,   "ini_diry/F");
  cetree_->Branch("ini_dirz",   &ini_dirz_,   "ini_dirz/F");
  cetree_->Branch("ini_time",   &ini_time_,   "ini_time/F");
  cetree_->Branch("ini_posx",   &ini_posx_,   "ini_posx/F");
  cetree_->Branch("ini_posy",   &ini_posy_,   "ini_posy/F");
  cetree_->Branch("ini_posz",   &ini_posz_,   "ini_posz/F");
  cetree_->Branch("weight",     &weight_,     "weight/F");

  if (Evs("Polarization enable")) {
    polarizationEnable_ = true;
    cetree_->Branch("ini_polarx", &ini_polarx_, "ini_polarx/F");
    cetree_->Branch("ini_polary", &ini_polary_, "ini_polary/F");
    cetree_->Branch("ini_polarz", &ini_polarz_, "ini_polarz/F");
  }
  
  return AS_OK;
}

ANLStatus ComptonEventTreeWithInitialInfo::mod_ana()
{
  ini_energy_ = initialInfo_->InitialEnergy();
  G4ThreeVector iniDir = initialInfo_->InitialDirection();
  ini_dirx_ = iniDir.x();
  ini_diry_ = iniDir.y();
  ini_dirz_ = iniDir.z();

  ini_time_ = initialInfo_->InitialTime();
  G4ThreeVector iniPos = initialInfo_->InitialPosition();
  ini_posx_ = iniPos.x();
  ini_posy_ = iniPos.y();
  ini_posz_ = iniPos.z();
  
  if (polarizationEnable_) {
    G4ThreeVector iniPolar = initialInfo_->InitialPolarization();
    ini_polarx_ = iniPolar.x();
    ini_polary_ = iniPolar.y();
    ini_polarz_ = iniPolar.z();
  }

  weight_ = initialInfo_->Weight();

  return ComptonEventTree::mod_ana();
}

uint64_t ComptonEventTreeWithInitialInfo::EventID() const
{
  return initialInfo_->EventID();
}

} /* namespace comptonsoft */
