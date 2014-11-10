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

#include "VANLGeometry.hh"
#include "G4SystemOfUnits.hh"

using namespace anl;
using namespace anlgeant4;


VANLGeometry::VANLGeometry()
  : m_LengthUnit(mm), m_LengthUnitName("mm"),
    m_SurfaceCheck(true)
{
  add_alias("VANLGeometry");
}


void VANLGeometry::SetLengthUnit(double unit, const std::string& name)
{
  m_LengthUnit = unit;
  m_LengthUnitName = name;
}


void VANLGeometry::SetLengthUnit(const std::string& name)
{
  if (name=="cm") {
    SetLengthUnit(cm, name);
  }
  else if (name=="pc") {
    SetLengthUnit(pc, name);
  }
  else {
    SetLengthUnit(1.0, "");
  }
}


ANLStatus VANLGeometry::mod_startup()
{
  register_parameter(&m_SurfaceCheck, "surface_check");
  return AS_OK;
}
