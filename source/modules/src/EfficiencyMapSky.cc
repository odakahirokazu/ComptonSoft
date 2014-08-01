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

#include "EfficiencyMapSky.hh"

#include "globals.hh"
#include "InitialInformation.hh"

using namespace anl;
using namespace comptonsoft;


EfficiencyMapSky::EfficiencyMapSky()
  : m_Scale(1.0)
{
}


ANLStatus EfficiencyMapSky::mod_startup()
{
  SetUnit(degree, "degree");
  register_parameter(&m_Scale, "scale");

  return BackProjection::mod_startup();
}


ANLStatus EfficiencyMapSky::mod_init()
{
  GetANLModuleIF("InitialInformation", &m_InitialInfo);
  
  return BackProjection::mod_init();
}


ANLStatus EfficiencyMapSky::mod_ana()
{
  G4ThreeVector dir = -(m_InitialInfo->InitialDirection());
  G4ThreeVector yaxis(0.0, 1.0, 0.0);
  G4ThreeVector zaxis(0.0, 0.0, 1.0);
  double y = 90.0 - dir.angle(yaxis)/degree;
  G4ThreeVector coneXZ(dir.x(), 0.0, dir.z());
  double x = coneXZ.angle(zaxis)/degree;
  if (coneXZ.x()<0.0) {
    x = -x;
  }
  FillImage(x, y, m_Scale);
  // std::cout << x << "  " << y << std::endl;
  
  return AS_OK;
}
