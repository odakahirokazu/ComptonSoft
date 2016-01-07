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

#include "GaussianBeamPrimaryGen.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anl;

namespace anlgeant4
{

GaussianBeamPrimaryGen::GaussianBeamPrimaryGen()
  : m_RSigma(1.0*cm)
{
  add_alias("GaussianBeamPrimaryGen");
}

GaussianBeamPrimaryGen::~GaussianBeamPrimaryGen() = default;

ANLStatus GaussianBeamPrimaryGen::mod_startup()
{
  PlaneWavePrimaryGen::mod_startup();

  unregister_parameter("radius");
  
  register_parameter(&m_RSigma, "radial_sigma", LengthUnit(), LengthUnitName());
  set_parameter_description("Radial sigma of the beam size.");
  
  return AS_OK;
}

G4ThreeVector GaussianBeamPrimaryGen::samplePosition()
{
  const G4ThreeVector xaxis(DirectionOrthogonal());
  G4ThreeVector yaxis = xaxis;
  yaxis.rotate(0.5*pi, Direction());
  const double x = m_RSigma * CLHEP::RandGauss::shoot(CLHEP::HepRandom::getTheEngine());
  const double y = m_RSigma * CLHEP::RandGauss::shoot(CLHEP::HepRandom::getTheEngine());
  const G4ThreeVector position = CenterPosition() + x * xaxis + y * yaxis;
  return position;
}

} /* namespace anlgeant4 */
