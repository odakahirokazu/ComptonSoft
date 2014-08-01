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

#include "WriteGDML.hh"
#include "G4GDMLParser.hh"

using namespace anl;
using namespace anlgeant4;

WriteGDML::WriteGDML()
  : m_GeometryFileName("detector.gdml")
{
}


ANLStatus WriteGDML::mod_startup()
{
  register_parameter(&m_GeometryFileName, "file");
    
  return AS_OK;
}


ANLStatus WriteGDML::mod_init()
{ 
  G4VPhysicalVolume* world = 
    G4TransportationManager::GetTransportationManager()
    ->GetNavigatorForTracking()->GetWorldVolume();
  G4GDMLParser parser;
  // parser.Write("output.gdml", world, true, "path_to_GDML_schema");
  parser.Write(m_GeometryFileName, world, false);
  
  return AS_OK;
}
