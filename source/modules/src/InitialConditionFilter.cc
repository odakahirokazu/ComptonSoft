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

#include "InitialConditionFilter.hh"
#include "ReadHitTree.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


using namespace anl;
using namespace comptonsoft;

InitialConditionFilter::InitialConditionFilter()
  : energy0(-100.0*MeV), energy1(100.0*MeV), read_hittree(0)
{
}


ANLStatus InitialConditionFilter::mod_startup()
{
  register_parameter(&energy0, "Initail energy minimum", keV, "keV");
  register_parameter(&energy1, "Initail energy maximum", keV, "keV");
  GetANLModule("ReadHitTree", &read_hittree);
  return AS_OK;
}


ANLStatus InitialConditionFilter::mod_ana()
{
  double ini_energy = read_hittree->InitialEnergy();
  
  if (ini_energy < energy0 || energy1 < ini_energy) {
    return AS_SKIP;
  }

  return AS_OK;
}
