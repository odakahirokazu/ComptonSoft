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

#include "RadioactivationTreeIO.hh"
#include "AstroUnits.hh"
#include "TTree.h"
#include "CSException.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

RadioactivationTreeIO::RadioactivationTreeIO() = default;

RadioactivationTreeIO::~RadioactivationTreeIO() = default;

void RadioactivationTreeIO::defineBranches()
{
  tree_->Branch("Z", &Z_, "Z/I");
  tree_->Branch("A", &A_, "A/I");
  tree_->Branch("E", &E_, "E/D");
  tree_->Branch("floating_level", &floating_level_, "floating_level/I");
  tree_->Branch("volume", &volume_, "volume/I");
  tree_->Branch("posx", &posx_, "posx/F");
  tree_->Branch("posy", &posy_, "posy/F");
  tree_->Branch("posz", &posz_, "posz/F");
}

void RadioactivationTreeIO::setBranchAddresses()
{
  tree_->SetBranchAddress("Z", &Z_);
  tree_->SetBranchAddress("A", &A_);
  tree_->SetBranchAddress("E", &E_);
  tree_->SetBranchAddress("floating_level", &floating_level_);
  tree_->SetBranchAddress("volume", &volume_);
  tree_->SetBranchAddress("posx", &posx_);
  tree_->SetBranchAddress("posy", &posy_);
  tree_->SetBranchAddress("posz", &posz_);
}

void RadioactivationTreeIO::fill(const IsotopeInfo& isotope, int volume, const G4ThreeVector& position)
{
  Z_ = isotope.Z();
  A_ = isotope.A();
  E_ = isotope.Energy() / unit::keV;
  floating_level_ = isotope.FloatingLevel();
  volume_ = volume;
  posx_ = position.x() / unit::cm;
  posy_ = position.y() / unit::cm;
  posz_ = position.z() / unit::cm;

  tree_->Fill();
}

std::tuple<IsotopeInfo, int, G4ThreeVector> RadioactivationTreeIO::retrieve() const
{
  IsotopeInfo isotope(Z_, A_, E_*unit::keV, floating_level_);
  const int volume = volume_;
  const G4ThreeVector position(posx_*unit::cm, posy_*unit::cm, posz_*unit::cm);

  return std::make_tuple(isotope, volume, position);
}

} /* namespace comptonsoft */
