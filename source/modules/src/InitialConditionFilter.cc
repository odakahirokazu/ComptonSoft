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
#include "AstroUnits.hh"
#include "InitialInformation.hh"

using namespace anl;

namespace comptonsoft
{

InitialConditionFilter::InitialConditionFilter()
  : energy0_(-100.0*MeV), energy1_(100.0*MeV), initialInfo_(nullptr)
{
}

ANLStatus InitialConditionFilter::mod_startup()
{
  register_parameter(&energy0_, "energy_min", keV, "keV");
  register_parameter(&energy1_, "energy_max", keV, "keV");

  GetANLModuleIF("InitialInformation", &initialInfo_);

  return AS_OK;
}

ANLStatus InitialConditionFilter::mod_ana()
{
  const double initialEnergy = initialInfo_->InitialEnergy();
  
  if (initialEnergy < energy0_ || energy1_ < initialEnergy) {
    return AS_SKIP;
  }

  return AS_OK;
}

} /* namespace comptonsoft */
