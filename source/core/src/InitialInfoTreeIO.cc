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

#include "InitialInfoTreeIO.hh"
#include "AstroUnits.hh"
#include "TTree.h"
#include "DetectorHit.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

InitialInfoTreeIO::InitialInfoTreeIO()
  : tree_(nullptr), enabled_(true)
{
}

InitialInfoTreeIO::~InitialInfoTreeIO() = default;

void InitialInfoTreeIO::defineBranches()
{
  if (enabled_) {
    tree_->Branch("ini_energy", &ini_energy_, "ini_energy/F");
    tree_->Branch("ini_dirx",   &ini_dirx_,   "ini_dirx/F");
    tree_->Branch("ini_diry",   &ini_diry_,   "ini_diry/F");
    tree_->Branch("ini_dirz",   &ini_dirz_,   "ini_dirz/F");
    tree_->Branch("ini_time",   &ini_time_,   "ini_time/D");
    tree_->Branch("ini_posx",   &ini_posx_,   "ini_posx/F");
    tree_->Branch("ini_posy",   &ini_posy_,   "ini_posy/F");
    tree_->Branch("ini_posz",   &ini_posz_,   "ini_posz/F");
    tree_->Branch("ini_polarx", &ini_polarx_, "ini_polarx/F");
    tree_->Branch("ini_polary", &ini_polary_, "ini_polary/F");
    tree_->Branch("ini_polarz", &ini_polarz_, "ini_polarz/F");
    tree_->Branch("weight",     &weight_,     "weight/F");
  }
}

void InitialInfoTreeIO::setBranchAddresses()
{
  if (enabled_) {
    tree_->SetBranchAddress("ini_energy", &ini_energy_);
    tree_->SetBranchAddress("ini_dirx",   &ini_dirx_);
    tree_->SetBranchAddress("ini_diry",   &ini_diry_);
    tree_->SetBranchAddress("ini_dirz",   &ini_dirz_);
    tree_->SetBranchAddress("ini_time",   &ini_time_);
    tree_->SetBranchAddress("ini_posx",   &ini_posx_);
    tree_->SetBranchAddress("ini_posy",   &ini_posy_);
    tree_->SetBranchAddress("ini_posz",   &ini_posz_);
    tree_->SetBranchAddress("ini_polarx", &ini_polarx_);
    tree_->SetBranchAddress("ini_polary", &ini_polary_);
    tree_->SetBranchAddress("ini_polarz", &ini_polarz_);
    tree_->SetBranchAddress("weight",     &weight_);
  }
}

double InitialInfoTreeIO::getInitialEnergy() const
{
  return ini_energy_ * unit::keV;
}

vector3_t InitialInfoTreeIO::getInitialDirection() const
{
  return vector3_t(ini_dirx_, ini_diry_, ini_dirz_);
}

double InitialInfoTreeIO::getInitialTime() const
{
  return ini_time_ * unit::second;
}

vector3_t InitialInfoTreeIO::getInitialPosition() const
{
  return vector3_t(ini_posx_ * unit::cm, ini_posy_ * unit::cm, ini_posz_ * unit::cm);
}
  
vector3_t InitialInfoTreeIO::getInitialPolarization() const
{
  return vector3_t(ini_polarx_, ini_polary_, ini_polarz_);
}

double InitialInfoTreeIO::getWeight() const
{
  return weight_;
}

void InitialInfoTreeIO::setInitialEnergy(double v)
{
  ini_energy_ = v / unit::keV;
}

void InitialInfoTreeIO::setInitialDirection(double x, double y, double z)
{
  ini_dirx_ = x;
  ini_diry_ = y;
  ini_dirz_ = z;
}

void InitialInfoTreeIO::setInitialTime(double v)
{
  ini_time_ = v / unit::second;
}

void InitialInfoTreeIO::setInitialPosition(double x, double y, double z)
{
  ini_posx_ = x / unit::cm;
  ini_posy_ = y / unit::cm;
  ini_posz_ = z / unit::cm;
}
  
void InitialInfoTreeIO::setInitialPolarization(double x, double y, double z)
{
  ini_polarx_ = x;
  ini_polary_ = y;
  ini_polarz_ = z;
}

void InitialInfoTreeIO::setWeight(double v)
{
  weight_ = v;
}

} /* namespace comptonsoft */
