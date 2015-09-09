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

#ifndef ANLGEANT4_PositionSamplerInVolume_H
#define ANLGEANT4_PositionSamplerInVolume_H 1

#include <vector>
#include <string>
#include "G4ThreeVector.hh"

class G4VPhysicalVolume;

namespace anlgeant4
{

/**
 * Sample positions from a volume inside.
 *
 * @author Hirokazu Odaka 
 * @date 2008-09-19 | Hirokazu Odaka
 * @date 2011-06-22 | Hirokazu Odaka
 * @date 2012-08-01 | Yuto Ichinohe & Hirokazu Odaka | bug fixed
 * @date 2015-03-09 | Hirokazu Odaka | rename the class name. new algorithm for G4Sphere.
 */
class PositionSamplerInVolume
{
public:
  enum class VolumeType_t { Box, Sphere, Tube, EllipticalTube, Ellipsoid, Any };
  
public:
  PositionSamplerInVolume();
  ~PositionSamplerInVolume() = default;

  void setVolumeHierarchy(const std::vector<std::string>& v) { volumeID_ = v; }
  void defineVolumeSize();
  G4ThreeVector samplePosition();

private:
  std::vector<std::string> volumeID_;
  VolumeType_t volumeType_;
  const G4VPhysicalVolume* theVolume_;
  
  double boxHSizeX_;
  double boxHSizeY_;
  double boxHSizeZ_;
  double innerRadius_;
  double outerRadius_;
  double startPhi_;
  double deltaPhi_;
  double startTheta_;
  double deltaTheta_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_PositionSamplerInVolume_H */
