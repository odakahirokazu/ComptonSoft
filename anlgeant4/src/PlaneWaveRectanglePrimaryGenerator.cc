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

#include "PlaneWaveRectanglePrimaryGenerator.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

PlaneWaveRectanglePrimaryGenerator::PlaneWaveRectanglePrimaryGenerator()
  : size_x_(100.0*unit::cm), size_y_(100.0*unit::cm), roll_angle_(0.0)
{
  add_alias("PlaneWaveRectanglePrimaryGenerator");
}

PlaneWaveRectanglePrimaryGenerator::~PlaneWaveRectanglePrimaryGenerator() = default;

ANLStatus PlaneWaveRectanglePrimaryGenerator::mod_define()
{
  PlaneWavePrimaryGenerator::mod_define();

  undefine_parameter("radius");

  define_parameter("size_x", &mod_class::size_x_, unit::cm, "cm");
  set_parameter_description("Size x of the rectangle where parimary particles are generated.");
  define_parameter("size_y", &mod_class::size_y_, unit::cm, "cm");
  set_parameter_description("Size y of the rectangle where parimary particles are generated.");
  define_parameter("roll_angle", &mod_class::roll_angle_, 1.0, "radian");
  set_parameter_description("Roll angle of the rectangle where parimary particles are generated.");

  return AS_OK;
}

G4ThreeVector PlaneWaveRectanglePrimaryGenerator::sample_position() const
{
  // set position
  const double theta = roll_angle_;
  const double costheta = std::cos(theta);
  const double sintheta = std::sin(theta);
  const G4ThreeVector x_axis(costheta, sintheta, 0.0);
  const G4ThreeVector y_axis(-sintheta, costheta, 0.0);
  const double x = (-0.5+G4UniformRand())*size_x_;
  const double y = (-0.5+G4UniformRand())*size_y_;
  const G4ThreeVector position = center_position() + x*x_axis + y*y_axis;
  return position;
}

double PlaneWaveRectanglePrimaryGenerator::generation_area() const
{
  const double cosTheta = direction().cosTheta();
  const double area = size_x_ * size_y_ * std::abs(cosTheta);
  return area;
}

} /* namespace anlgeant4 */
