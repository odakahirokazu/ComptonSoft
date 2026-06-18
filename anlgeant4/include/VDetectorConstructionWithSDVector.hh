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

#ifndef ANLGEANT4_VDetectorConstructionWithSDVector_H
#define ANLGEANT4_VDetectorConstructionWithSDVector_H 1

#include "G4VUserDetectorConstruction.hh"
#include <vector>
#include <utility>
#include <string>

class G4VSensitiveDetector;

namespace anlgeant4 {

/**
 * Virtual detector construction with sensitive detector vector
 * @author Hirokazu Odaka
 */
class VDetectorConstructionWithSDVector : public G4VUserDetectorConstruction
{
public:
  using SDVector = std::vector<std::pair<std::string, G4VSensitiveDetector*>>;

public:
  VDetectorConstructionWithSDVector();
  ~VDetectorConstructionWithSDVector();

  void ConstructSDandField() override;

  void setSDs(const SDVector& v) { SDs_ = v; }

private:
  SDVector SDs_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VDetectorConstructionWithSDVector_H */
