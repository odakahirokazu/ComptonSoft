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

#include "ReadGDML.hh"
#include "GDMLDetectorConstruction.hh"

using namespace anlnext;

namespace anlgeant4
{

ReadGDML::ReadGDML()
  : filename_("detector.gdml"), validate_(true)
{
}

ANLStatus ReadGDML::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  define_parameter("validate", &mod_class::validate_);

  return AS_OK;
}

G4VUserDetectorConstruction* ReadGDML::create()
{
  VDetectorConstruction* udc = new GDMLDetectorConstruction(filename_, validate_);
  udc->set_sensitive_detectors(get_sensitive_detectors());
  return udc;
}

} /* namespace anlgeant4 */
