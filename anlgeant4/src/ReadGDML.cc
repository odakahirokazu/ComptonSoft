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

using namespace anl;
using namespace anlgeant4;

ReadGDML::ReadGDML()
  : m_GeometryFileName("detector.gdml"), m_Validate(true)
{
}


ANLStatus ReadGDML::mod_startup()
{
  register_parameter(&m_GeometryFileName, "Detector geometry file");
  register_parameter(&m_Validate, "Validate GDML?");
    
  return AS_OK;
}


G4VUserDetectorConstruction* ReadGDML::create()
{  
  return new GDMLDetectorConstruction(m_GeometryFileName, m_Validate);
}
