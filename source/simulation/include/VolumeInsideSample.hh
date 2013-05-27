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

#ifndef COMPTONSOFT_VolumeInsideSample_H
#define COMPTONSOFT_VolumeInsideSample_H 1

#include <vector>
#include <string>
#include "G4ThreeVector.hh"

class G4VPhysicalVolume;

namespace comptonsoft {

/**
 * Sample positions from a volume inside.
 *
 * @author Hirokazu Odaka 
 * @date 2008-09-19 | Hirokazu Odaka
 * @date 2011-06-22 | Hirokazu Odaka
 * @date 2012-08-01 | Yuto Ichinohe & Hirokazu Odaka | bug fixed
 */
class VolumeInsideSample
{
public: 
  VolumeInsideSample();
  ~VolumeInsideSample(){}

  void setVolumeHierarchy(const std::vector<std::string>& v) { VolumeId = v; }
  void defineVolumeSize();
  G4ThreeVector samplePosition();

private:
  std::vector<std::string> VolumeId;
  
  G4bool VolumeBox;
  G4double BoxHSizeX;
  G4double BoxHSizeY;
  G4double BoxHSizeZ;

  const G4VPhysicalVolume* TargetVolume;
};

}

#endif /* COMPTONSOFT_VolumeInsideSample_H */
