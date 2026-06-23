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

#include "SphericalSourcePrimaryGenerator.hh"
#include "AstroUnits.hh"
#include "Randomize.hh"

using namespace anlnext;

namespace anlgeant4
{

SphericalSourcePrimaryGenerator::SphericalSourcePrimaryGenerator()
  : source_inner_radius_(0.0),
    source_outer_radius_(0.0)
{
  add_alias("SphericalSourcePrimaryGenerator");
}

SphericalSourcePrimaryGenerator::~SphericalSourcePrimaryGenerator() = default;

ANLStatus SphericalSourcePrimaryGenerator::mod_define()
{
  PointSourcePrimaryGenerator::mod_define();

  define_parameter("source_inner_radius", &mod_class::source_inner_radius_, unit::cm, "cm");
  define_parameter("source_outer_radius", &mod_class::source_outer_radius_, unit::cm, "cm");

  return AS_OK;
}

G4ThreeVector SphericalSourcePrimaryGenerator::sample_position() const
{
  const double ri = source_inner_radius_;
  const double ro = source_outer_radius_;
  const double r3min = ri*ri*ri;
  const double r3max = ro*ro*ro;
  const double r = std::cbrt(r3min+G4UniformRand()*(r3max-r3min));
  const double phi = G4UniformRand()*constant::twopi;
  const double costheta = -1.0 + G4UniformRand()*2.0;
  const double sintheta = std::sqrt(1.0-costheta*costheta);
  G4ThreeVector position(r*sintheta*std::cos(phi), r*sintheta*std::sin(phi), r*costheta);
  return position;
}

} /* namespace anlgeant4 */
