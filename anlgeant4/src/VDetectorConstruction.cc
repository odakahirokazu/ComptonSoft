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

#include "VDetectorConstruction.hh"
#include "G4VSensitivedetector.hh"
#include "G4SDManager.hh"


namespace anlgeant4
{

VDetectorConstruction::VDetectorConstruction() = default;

VDetectorConstruction::~VDetectorConstruction() = default;

void VDetectorConstruction::ConstructSDandField()
{
  auto* sdManager = G4SDManager::GetSDMpointer();

  const auto& SDs = getSDs();
  for (const auto& pair: SDs) {
    const std::string& logical_volume_name = pair.first;
    const G4VSensitiveDetector* sd = pair.second;
    G4VSensitiveDetector* sd1 = sd->Clone();
    sdManager->AddNewDetector(sd1);
    SetSensitiveDetector(logical_volume_name, sd1);
  }
}

} /* namespace anlgeant4 */
