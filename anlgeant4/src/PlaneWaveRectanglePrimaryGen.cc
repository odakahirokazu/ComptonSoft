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

#include "PlaneWaveRectanglePrimaryGen.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

PlaneWaveRectanglePrimaryGen::PlaneWaveRectanglePrimaryGen()
  : m_SizeX(100.0*unit::cm), m_SizeY(100.0*unit::cm), m_RollAngle(0.0)
{
  add_alias("PlaneWaveRectanglePrimaryGen");
}

PlaneWaveRectanglePrimaryGen::~PlaneWaveRectanglePrimaryGen() = default;

ANLStatus PlaneWaveRectanglePrimaryGen::mod_define()
{
  PlaneWavePrimaryGen::mod_define();

  unregister_parameter("radius");
  
  register_parameter(&m_SizeX, "size_x", LengthUnit(), LengthUnitName());
  set_parameter_description("Size x of the rectangle where parimary particles are generated.");  
  register_parameter(&m_SizeY, "size_y", LengthUnit(), LengthUnitName());
  set_parameter_description("Size y of the rectangle where parimary particles are generated.");
  register_parameter(&m_RollAngle, "roll_angle", 1.0, "radian");
  set_parameter_description("Roll angle of the rectangle where parimary particles are generated.");
  
  return AS_OK;
}

G4ThreeVector PlaneWaveRectanglePrimaryGen::samplePosition()
{
  // set position
  const double theta = m_RollAngle;
  const double cosTheta = std::cos(theta);
  const double sinTheta = std::sin(theta);
  const G4ThreeVector xAxis(cosTheta, sinTheta, 0.0);
  const G4ThreeVector yAxis(-sinTheta, cosTheta, 0.0);
  const double x = (-0.5+G4UniformRand())*m_SizeX;
  const double y = (-0.5+G4UniformRand())*m_SizeY;
  const G4ThreeVector position = CenterPosition() + x*xAxis + y*yAxis;
  return position;
}

double PlaneWaveRectanglePrimaryGen::GenerationArea()
{
  const double cosTheta = Direction().cosTheta();
  const double area = m_SizeX * m_SizeY * std::abs(cosTheta);
  return area;
}

} /* namespace anlgeant4 */
