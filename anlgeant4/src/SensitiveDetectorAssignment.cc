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

#include "SensitiveDetectorAssignment.hh"

#include "G4VSensitiveDetector.hh"
#include "G4VUserDetectorConstruction.hh"

namespace anlgeant4
{

SensitiveDetectorAssignment::SensitiveDetectorAssignment() = default;

void SensitiveDetectorAssignment::register_sensitive_detector(const std::string& logical_volume_name, G4VSensitiveDetector* sd)
{
  SD_vector_.push_back(std::make_pair(logical_volume_name, sd))  ;
}

} /* namespace anlgeant4 */
