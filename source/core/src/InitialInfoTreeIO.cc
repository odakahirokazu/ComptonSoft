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
#include "TTree.h"
#include "DetectorHit.hh"

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
    tree_->Branch("ini_time",   &ini_time_,   "ini_time/F");
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

} /* namespace comptonsoft */
