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

#ifndef ANLGEANT4_VDetectorConstruction_H
#define ANLGEANT4_VDetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#include "SDAssignment.hh"
#include <vector>
#include <utility>
#include <string>

class G4VSensitiveDetector;

namespace anlgeant4 {

/**
 * Virtual detector construction with sensitive detector vector
 * @author Hirokazu Odaka
 */
class VDetectorConstruction : public G4VUserDetectorConstruction, public SDAssignment
{
public:
  VDetectorConstruction();
  ~VDetectorConstruction();

  void ConstructSDandField() override;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VDetectorConstruction_H */
