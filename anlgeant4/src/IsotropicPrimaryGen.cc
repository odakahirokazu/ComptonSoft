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

#include "IsotropicPrimaryGen.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anl;

namespace anlgeant4
{

IsotropicPrimaryGen::IsotropicPrimaryGen()
  : m_CenterPosition(0.0, 0.0, 0.0),
    m_Radius(100.0*unit::cm), m_Distance(-1.0),
    m_CenterDirection(0.0, 0.0, 1.0),
    m_ThetaMin(0.0), m_ThetaMax(CLHEP::pi),
    m_CosTheta0(1.0), m_CosTheta1(-1.0), m_CoveringFactor(1.0),
    m_Flux(1.0e-9*(unit::erg/unit::s/unit::cm2/unit::sr))
{
  add_alias("IsotropicPrimaryGen");
}

IsotropicPrimaryGen::~IsotropicPrimaryGen() = default;

ANLStatus IsotropicPrimaryGen::mod_startup()
{
  BasicPrimaryGen::mod_startup();

  enablePowerLawInput();
  register_parameter(&m_CenterPosition, "center_position", unit::cm, "cm");
  set_parameter_description("Position of the sphere.");
  register_parameter(&m_Radius, "radius", unit::cm, "cm");
  set_parameter_description("Radius of a disk where primaries are generated.");
  register_parameter(&m_Distance, "distance", unit::cm, "cm");
  set_parameter_description("Distance between the sphere center and a disk where primaries are generated. If this value is negative, then this parameter is set to be the same as the disk radius [Radius].");
  register_parameter(&m_CenterDirection, "center_direction");
  set_parameter_description("Center direction of the primaries.");
  register_parameter(&m_ThetaMin, "theta_min", 1.0, "radian");
  set_parameter_description("Minimum angle between the primary direction and the center direction.");
  register_parameter(&m_ThetaMax, "theta_max", 1.0, "radian");
  set_parameter_description("Maximum angle between the primary direction and the center direction.");
  register_parameter(&m_Flux, "flux", (unit::erg/unit::cm2/unit::s/unit::sr), "erg/cm2/s/sr");
  set_parameter_description("Energy flux of the primaries. This parameter is used only for calculating real time correspoing to a simulation.");

  return AS_OK;
}

ANLStatus IsotropicPrimaryGen::mod_init()
{
  using std::cos;
  using std::acos;
  using std::sqrt;
  
  BasicPrimaryGen::mod_init();

  m_CenterDirection = m_CenterDirection.unit();
  if (m_Distance < 0.0) { m_Distance = m_Radius; }
  
  const double posx = m_CenterPosition.x();
  const double posy = m_CenterPosition.y();
  const double posz = m_CenterPosition.z();

  const double dirx = m_CenterDirection.x();
  const double diry = m_CenterDirection.y();
  const double dirz = m_CenterDirection.z();

  m_CosTheta0 = cos(m_ThetaMin);
  m_CosTheta1 = cos(m_ThetaMax);
  m_CoveringFactor = 0.5*(m_CosTheta0-m_CosTheta1);

  std::cout << "--------" << std::endl;
  std::cout << "PrimaryGen status (isotropic)" << std::endl;
  std::cout << "  Center Position: "
            << posx/unit::cm << " " << posy/unit::cm << " " << posz/unit::cm << " cm" <<std::endl;
  std::cout << "  Radius: " << m_Radius/unit::cm << " cm" << std::endl;
  std::cout << "  Distance: " << m_Distance/unit::cm << " cm" << std::endl;
  std::cout << "  Center Direction: "
            << dirx << " " << diry << " " << dirz << std::endl;
  std::cout << "  Theta: "
            << m_ThetaMin/unit::degree << " - " << m_ThetaMax/unit::degree << " deg" << std::endl;
  printSpectralInfo();
  
  return AS_OK;
}

void IsotropicPrimaryGen::makePrimarySetting()
{
  using std::cos;
  using std::sin;
  using std::sqrt;

  // set position and direction
  const double cosTheta = m_CosTheta0 + (m_CosTheta1-m_CosTheta0) * G4UniformRand();
  const double sinTheta = sqrt(1.0-cosTheta*cosTheta);
  const double phi = CLHEP::twopi * G4UniformRand();
  G4ThreeVector v(m_Distance*sinTheta*cos(phi),
                  m_Distance*sinTheta*sin(phi),
                  m_Distance*cosTheta);
  v.rotateUz(-m_CenterDirection);
  
  G4ThreeVector v2 = v.orthogonal();
  v2.setMag( m_Radius * sqrt(G4UniformRand()) );
  const G4double chi = CLHEP::twopi * G4UniformRand();
  v2.rotate(chi, v);

  const G4ThreeVector position = m_CenterPosition + v + v2;
  const G4ThreeVector direction = (-v).unit();

  // set energy
  const double energy = sampleEnergy();

  setPrimary(position, energy, direction);
}

ANLStatus IsotropicPrimaryGen::mod_endrun()
{
  const double area = CLHEP::pi*m_Radius*m_Radius;
  const G4double solid = 4*CLHEP::pi*m_CoveringFactor*unit::sr;
  double realTime = 0.;
  double pflux = 0.;
  if (m_CoveringFactor != 0.0) {
    realTime = TotalEnergy()/(m_Flux*area*solid);
    pflux = Number()/area/realTime/solid;
  }
  
  std::cout.setf(std::ios::scientific);
  std::cout << "IsotropicPrimaryGen::mod_endrun \n"
            << "  Number: " << Number() << "\n"
            << "  Flux: " << m_Flux/(unit::erg/unit::cm2/unit::s/unit::sr) << " erg/cm2/s/sr\n"
            << "  Total Energy: " << TotalEnergy()/unit::keV << " keV = "
            << TotalEnergy()/unit::erg << " erg\n"
            << "  Covering Factor: " << m_CoveringFactor << "\n"
            << "  Area: " << area/unit::cm2 << " cm2\n"
            << "  Real time: " << realTime/unit::s << " s\n"
            << "  Photon flux: " << pflux/(1.0/unit::cm2/unit::s/unit::sr) << " photons/cm2/s/sr\n"
            << std::endl;
  std::cout.unsetf(std::ios::scientific);

  return AS_OK;
}

} /* namespace anlgeant4 */
