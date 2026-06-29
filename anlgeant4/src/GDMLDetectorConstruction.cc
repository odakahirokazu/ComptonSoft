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

#include "GDMLDetectorConstruction.hh"
#include "G4GDMLParser.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"

namespace anlgeant4
{

GDMLDetectorConstruction::
GDMLDetectorConstruction(const std::string& filename, bool validate)
  : filename_(filename), validate_(validate)
{
}

G4VPhysicalVolume* GDMLDetectorConstruction::Construct()
{
  G4GDMLParser parser;
  parser.Read(filename_, validate_);
  G4VPhysicalVolume* world = parser.GetWorldVolume();
  world->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::GetInvisible());
  return world;
}

} /* namespace anlgeant4 */
