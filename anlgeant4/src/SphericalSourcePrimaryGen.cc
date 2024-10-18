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

#include "SphericalSourcePrimaryGen.hh"
#include "AstroUnits.hh"
#include "Randomize.hh"

using namespace anlnext;

namespace anlgeant4
{

SphericalSourcePrimaryGen::SphericalSourcePrimaryGen()
  : source_inner_radius_(0.0),
    source_outer_radius_(0.0)
{
  add_alias("SphericalSourcePrimaryGen");
}

SphericalSourcePrimaryGen::~SphericalSourcePrimaryGen() = default;

ANLStatus SphericalSourcePrimaryGen::mod_define()
{
  PointSourcePrimaryGen::mod_define();

  register_parameter(&source_inner_radius_, "source_inner_radius", LengthUnit(), LengthUnitName());
  register_parameter(&source_outer_radius_, "source_outer_radius", LengthUnit(), LengthUnitName());

  return AS_OK;
}

G4ThreeVector SphericalSourcePrimaryGen::samplePosition()
{
  const double ri = source_inner_radius_;
  const double ro = source_outer_radius_;
  const double r3min = ri*ri*ri;
  const double r3max = ro*ro*ro;
  const double r = std::cbrt(r3min+G4UniformRand()*(r3max-r3min));
  const double phi = G4UniformRand()*CLHEP::twopi;
  const double cosTheta = -1.0 + G4UniformRand()*2.0;
  const double sinTheta = std::sqrt(1.0-cosTheta*cosTheta);
  G4ThreeVector position(r*sinTheta*std::cos(phi), r*sinTheta*std::sin(phi), r*cosTheta);
  return position;
}

} /* namespace anlgeant4 */
