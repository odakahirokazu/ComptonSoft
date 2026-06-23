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

#include "G4VUserDetectorConstruction.hh"

#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

VANLGeometry::VANLGeometry()
  : length_unit_(unit::cm),
    length_unit_name_("cm"),
    surface_check_(true)
{
  add_alias("VANLGeometry");
}

ANLStatus VANLGeometry::mod_define()
{
  define_parameter("surface_check", &mod_class::surface_check_);
  return AS_OK;
}

} /* namespace anlgeant4 */
