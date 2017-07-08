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

#include "PointSourcePrimaryGen.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"
#include "VANLGeometry.hh"

using namespace anl;

namespace anlgeant4
{

PointSourcePrimaryGen::PointSourcePrimaryGen()
  : m_SourcePosition(0.0, 0.0, 0.0),
    m_CenterDirection(0.0, 0.0, -1.0),
    m_Theta0(0.0), m_Theta1(CLHEP::pi), m_CosTheta0(1.0), m_CosTheta1(-1.0),
    m_CoveringFactor(1.0), m_Luminosity(1.0*(unit::erg/unit::s))
{
  add_alias("PointSourcePrimaryGen");
}

PointSourcePrimaryGen::~PointSourcePrimaryGen() = default;

ANLStatus PointSourcePrimaryGen::mod_define()
{
  BasicPrimaryGen::mod_define();

  enablePowerLawInput();
  register_parameter(&m_SourcePosition, "position",
                     LengthUnit(), LengthUnitName());
  set_parameter_description("Position of the source.");
  register_parameter(&m_CenterDirection, "direction");
  set_parameter_description("Center direction of the primaries.");
  register_parameter(&m_Theta0, "theta_min", 1.0, "radian");
  set_parameter_description("Minimum angle between the primary direction and the center direction.");
  register_parameter(&m_Theta1, "theta_max", 1.0, "radian");
  set_parameter_description("Maximum angle between the primary direction and the center direction.");
  register_parameter(&m_Luminosity, "luminosity", unit::erg/unit::s, "erg/s");
  set_parameter_description("Energy luminosity of the point source. This parameter is used only for calculating real time correspoing to a simulation.");

  return AS_OK;
}

ANLStatus PointSourcePrimaryGen::mod_initialize()
{
  using std::cos;
  
  BasicPrimaryGen::mod_initialize();
  m_CenterDirection = m_CenterDirection.unit();

  const double posx = m_SourcePosition.x();
  const double posy = m_SourcePosition.y();
  const double posz = m_SourcePosition.z();

  const double dirx = m_CenterDirection.x();
  const double diry = m_CenterDirection.y();
  const double dirz = m_CenterDirection.z();

  m_CosTheta0 = cos(m_Theta0);
  m_CosTheta1 = cos(m_Theta1);
  m_CoveringFactor = 0.5*(m_CosTheta0-m_CosTheta1);
  
  std::cout << "--------" << std::endl;
  std::cout << "PrimaryGen status (point source)" << std::endl;
  std::cout << "  Source Position: "
            << posx/unit::cm << " " << posy/unit::cm << " " << posz/unit::cm << " cm" <<std::endl;
  printSpectralInfo();
  std::cout << "  Direction: "
            << dirx << " " << diry << " " << dirz << '\n' 
            << "    theta: " << m_Theta0/unit::degree << " - " << m_Theta1/unit::degree
            << " deg ( covering factor: " << m_CoveringFactor << " )"<< std::endl;

  return AS_OK;
}

void PointSourcePrimaryGen::makePrimarySetting()
{
  const G4ThreeVector position = samplePosition();
  const G4ThreeVector direction = sampleDirection();
  const double energy = sampleEnergy();

  setPrimary(position, energy, direction);

  if (PolarizationMode()==0) {
    setUnpolarized();
  }
}

G4ThreeVector PointSourcePrimaryGen::sampleDirection()
{
  const double phi = CLHEP::twopi * G4UniformRand();
  const double cosTheta = m_CosTheta0+(m_CosTheta1-m_CosTheta0)*G4UniformRand();
  const double sinTheta = sqrt(1.0-cosTheta*cosTheta);
  G4ThreeVector direction(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
  direction.rotateUz(m_CenterDirection);
  return direction;
}

G4ThreeVector PointSourcePrimaryGen::samplePosition()
{
  return m_SourcePosition;
}

ANLStatus PointSourcePrimaryGen::mod_end_run()
{
  double realTime = 0.;
  double pflux = 0.;
  if (m_CoveringFactor != 0.0) {
    realTime = (TotalEnergy()/m_CoveringFactor)/m_Luminosity;
    pflux = (Number()/m_CoveringFactor)/realTime;
  }

  std::cout.setf(std::ios::scientific);
  std::cout << "PSPrimaryGen::mod_end_run \n"
            << "  Number: " << Number() << "\n"
            << "  Luminosity: " << m_Luminosity/(unit::erg/unit::s) << " erg/s\n"
            << "  Total Energy: " << TotalEnergy()/unit::keV << " keV = "
            << TotalEnergy()/unit::erg << " erg\n"
            << "  Covering Factor: " << m_CoveringFactor << "\n"
            << "  Real Time: " << realTime/unit::s << " s"
            << "  Photon rate: " << pflux/(1.0/unit::s) << " photons/s\n"
            << std::endl;
  std::cout.unsetf(std::ios::scientific);

  return AS_OK;
}

} /* namespace anlgeant4 */
