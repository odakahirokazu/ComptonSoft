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

#include "GaussianBeamPrimaryGenerator.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

GaussianBeamPrimaryGenerator::GaussianBeamPrimaryGenerator()
  : radial_sigma_(1.0*unit::cm)
{
  add_alias("GaussianBeamPrimaryGenerator");
}

GaussianBeamPrimaryGenerator::~GaussianBeamPrimaryGenerator() = default;

ANLStatus GaussianBeamPrimaryGenerator::mod_define()
{
  PlaneWavePrimaryGenerator::mod_define();

  undefine_parameter("radius");

  define_parameter("radial_sigma", &mod_class::radial_sigma_, unit::cm, "cm");
  set_parameter_description("Radial sigma of the beam size.");

  return AS_OK;
}

G4ThreeVector GaussianBeamPrimaryGenerator::sample_position() const
{
  const G4ThreeVector xaxis(direction_orthogonal());
  G4ThreeVector yaxis = xaxis;
  yaxis.rotate(0.5*CLHEP::pi, direction());
  const double x = radial_sigma_ * CLHEP::RandGauss::shoot(CLHEP::HepRandom::getTheEngine());
  const double y = radial_sigma_ * CLHEP::RandGauss::shoot(CLHEP::HepRandom::getTheEngine());
  const G4ThreeVector position = center_position() + x * xaxis + y * yaxis;
  return position;
}

} /* namespace anlgeant4 */
