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

#ifndef ANLGEANT4_GDMLDetectorConstruction_H
#define ANLGEANT4_GDMLDetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"

namespace anlgeant4 {

/**
 * Detector construction using a GDML input.
 * @author Hirokazu Odaka
 */
class GDMLDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  GDMLDetectorConstruction(const std::string& geom_file_name, bool validate);

  G4VPhysicalVolume* Construct();

private:
  std::string m_GeometryFileName;
  bool m_Validate;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_GDMLDetectorConstruction_H */
