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

#include "PolarizedPointSourcePrimaryGenerator.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"
#include "VANLGeometry.hh"

using namespace anlnext;

namespace anlgeant4
{

PolarizedPointSourcePrimaryGenerator::PolarizedPointSourcePrimaryGenerator()
  : m_SourcePosition(0.0, 0.0, 0.0),
    m_CenterDirection(0.0, 0.0, -1.0),
    m_Theta0(0.0), m_Theta1(CLHEP::pi), m_CosTheta0(1.0), m_CosTheta1(-1.0),
    m_CoveringFactor(1.0), 
    m_Polarization0(1.0, 0.0, 0.0), m_PolarizationDegree(0.0),
    m_Luminosity(1.0*(unit::erg/unit::s))
{
  add_alias("PolarizedPointSourcePrimaryGenerator");
}

PolarizedPointSourcePrimaryGenerator::~PolarizedPointSourcePrimaryGenerator() = default;

ANLStatus PolarizedPointSourcePrimaryGenerator::mod_define()
{
  BasicPrimaryGenerator::mod_define();

  enable_powerlaw_input();
  define_parameter("position", &mod_class::m_SourcePosition,
                     unit::cm, "cm");
  set_parameter_description("Position of the source.");
  define_parameter("direction", &mod_class::m_CenterDirection);
  set_parameter_description("Center direction of the primaries.");
  define_parameter("theta_min", &mod_class::m_Theta0, 1.0, "radian");
  set_parameter_description("Minimum angle between the primary direction and the center direction.");
  define_parameter("theta_max", &mod_class::m_Theta1, 1.0, "radian");
  set_parameter_description("Maximum angle between the primary direction and the center direction.");
  define_parameter("luminosity", &mod_class::m_Luminosity, unit::erg/unit::s, "erg/s");
  set_parameter_description("Energy luminosity of the point source. This parameter is used only for calculating real time correspoing to a simulation.");
  define_parameter("polarization_vector", &mod_class::m_Polarization0);
  set_parameter_description("Polarization vector");
  define_parameter("degree_of_polarization", &mod_class::m_PolarizationDegree);
  set_parameter_description("Degree of polarization");

  return AS_OK;
}

ANLStatus PolarizedPointSourcePrimaryGenerator::mod_initialize()
{
  using std::cos;
  
  BasicPrimaryGenerator::mod_initialize();
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
  std::cout << "PrimaryGenerator status (point source)" << std::endl;
  std::cout << "  Source Position: "
            << posx/unit::cm << " " << posy/unit::cm << " " << posz/unit::cm << " cm" <<std::endl;
  print_spectral_info();
  std::cout << "  Direction: "
            << dirx << " " << diry << " " << dirz << '\n' 
            << "    theta: " << m_Theta0/unit::degree << " - " << m_Theta1/unit::degree
            << " deg ( covering factor: " << m_CoveringFactor << " )"<< std::endl;

  return AS_OK;
}

PrimarySetting PolarizedPointSourcePrimaryGenerator::make_primary_setting() const
{
  const G4ThreeVector position = sample_position();
  const G4ThreeVector direction = sample_direction();
  const double energy = sample_energy();

  if (G4UniformRand() < m_PolarizationDegree) {
    G4ThreeVector polarization = (m_Polarization0 - (m_Polarization0.dot(direction))*direction).unit();
    return make_primary(position, energy, direction, polarization);
  }
  else {
    return make_primary(position, energy, direction, unpolarized_vector(direction));
  }
}

G4ThreeVector PolarizedPointSourcePrimaryGenerator::sample_direction() const
{
  const double phi = CLHEP::twopi * G4UniformRand();
  const double cosTheta = m_CosTheta0+(m_CosTheta1-m_CosTheta0)*G4UniformRand();
  const double sinTheta = sqrt(1.0-cosTheta*cosTheta);
  G4ThreeVector direction(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
  direction.rotateUz(m_CenterDirection);
  return direction;
}

G4ThreeVector PolarizedPointSourcePrimaryGenerator::sample_position() const
{
  return m_SourcePosition;
}

ANLStatus PolarizedPointSourcePrimaryGenerator::mod_end_run()
{
  double realTime = 0.;
  double pflux = 0.;
  if (m_CoveringFactor != 0.0) {
    realTime = (total_energy()/m_CoveringFactor)/m_Luminosity;
    pflux = (number()/m_CoveringFactor)/realTime;
  }
  set_real_time(realTime);

  std::cout.setf(std::ios::scientific);
  std::cout << "PSPrimaryGenerator::mod_end_run \n"
            << "  Number: " << number() << "\n"
            << "  Luminosity: " << m_Luminosity/(unit::erg/unit::s) << " erg/s\n"
            << "  Total Energy: " << total_energy()/unit::keV << " keV = "
            << total_energy()/unit::erg << " erg\n"
            << "  Covering Factor: " << m_CoveringFactor << "\n"
            << "  Real Time: " << realTime/unit::s << " s"
            << "  Photon rate: " << pflux/(1.0/unit::s) << " photons/s\n"
            << std::endl;
  std::cout.unsetf(std::ios::scientific);

  return AS_OK;
}

} /* namespace anlgeant4 */
