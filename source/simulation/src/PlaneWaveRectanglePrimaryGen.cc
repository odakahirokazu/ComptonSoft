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

using namespace anl;
using namespace comptonsoft;


PlaneWaveRectanglePrimaryGen::
PlaneWaveRectanglePrimaryGen()
  : m_SizeX(100.0*cm), m_SizeY(100.0*cm)
{
  add_alias("PlaneWaveRectanglePrimaryGen");
}


ANLStatus PlaneWaveRectanglePrimaryGen::mod_startup()
{
  PlaneWavePrimaryGen::mod_startup();

  unregister_parameter("radius");
  
  register_parameter(&m_SizeX, "size_x", cm, "cm");
  set_parameter_description("Size x of the rectangle where parimary particles are generated.");  
  register_parameter(&m_SizeY, "size_y", cm, "cm");
  set_parameter_description("Size y of the rectangle where parimary particles are generated.");   
  
  return AS_OK;
}


G4ThreeVector PlaneWaveRectanglePrimaryGen::samplePosition()
{
  // set position
  G4ThreeVector xAxis(1.0, 0.0, 0.0);
  G4ThreeVector yAxis(0.0, 1.0, 0.0);
  G4double x = (-0.5+G4UniformRand())*m_SizeX;
  G4double y = (-0.5+G4UniformRand())*m_SizeY;
  G4ThreeVector position = CenterPosition() + x*xAxis + y*yAxis;
  return position;
}


G4double PlaneWaveRectanglePrimaryGen::GenerationArea()
{
  G4double cosTheta = Direction().cosTheta();
  G4double area = m_SizeX * m_SizeY * std::abs(cosTheta);
  return area;
}
