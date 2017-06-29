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
    m_Radius(100.0*cm), m_Distance(-1.0),
    m_CenterDirection(0.0, 0.0, 1.0),
    m_ThetaMin(0.0), m_ThetaMax(pi),
    m_CosTheta0(1.0), m_CosTheta1(-1.0), m_CoveringFactor(1.0),
    m_Flux(1.0e-9*(erg/s/cm2/sr))
{
  add_alias("IsotropicPrimaryGen");
}

IsotropicPrimaryGen::~IsotropicPrimaryGen() = default;

ANLStatus IsotropicPrimaryGen::mod_startup()
{
  BasicPrimaryGen::mod_startup();

  enablePowerLawInput();
  register_parameter(&m_CenterPosition, "center_position", cm, "cm");
  set_parameter_description("Position of the sphere.");
  register_parameter(&m_Radius, "radius", cm, "cm");
  set_parameter_description("Radius of a disk where primaries are generated.");
  register_parameter(&m_Distance, "distance", cm, "cm");
  set_parameter_description("Distance between the sphere center and a disk where primaries are generated. If this value is negative, then this parameter is set to be the same as the disk radius [Radius].");
  register_parameter(&m_CenterDirection, "center_direction");
  set_parameter_description("Center direction of the primaries.");
  register_parameter(&m_ThetaMin, "theta_min", 1.0, "radian");
  set_parameter_description("Minimum angle between the primary direction and the center direction.");
  register_parameter(&m_ThetaMax, "theta_max", 1.0, "radian");
  set_parameter_description("Maximum angle between the primary direction and the center direction.");
  register_parameter(&m_Flux, "flux", (erg/cm2/s/sr), "erg/cm2/s/sr");
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
  
  const G4double posx = m_CenterPosition.x();
  const G4double posy = m_CenterPosition.y();
  const G4double posz = m_CenterPosition.z();

  const G4double dirx = m_CenterDirection.x();
  const G4double diry = m_CenterDirection.y();
  const G4double dirz = m_CenterDirection.z();

  m_CosTheta0 = cos(m_ThetaMin);
  m_CosTheta1 = cos(m_ThetaMax);
  m_CoveringFactor = 0.5*(m_CosTheta0-m_CosTheta1);

  G4cout << "--------" << G4endl;
  G4cout << "PrimaryGen status (isotropic)" << G4endl;
  G4cout << "  Center Position: "
         << posx/cm << " " << posy/cm << " " << posz/cm << " cm" <<G4endl;
  G4cout << "  Radius: " << m_Radius/cm << " cm" << G4endl;
  G4cout << "  Distance: " << m_Distance/cm << " cm" << G4endl;
  G4cout << "  Center Direction: "
         << dirx << " " << diry << " " << dirz << G4endl;
  G4cout << "  Theta: "
         << m_ThetaMin/degree << " - " << m_ThetaMax/degree << " deg" << G4endl;
  printSpectralInfo();
  
  return AS_OK;
}

void IsotropicPrimaryGen::makePrimarySetting()
{
  using std::cos;
  using std::sin;
  using std::sqrt;

  // set position and direction
  G4double cosTheta = m_CosTheta0 + (m_CosTheta1-m_CosTheta0) * G4UniformRand();
  G4double sinTheta = sqrt(1.0-cosTheta*cosTheta);
  G4double phi = twopi * G4UniformRand();
  G4ThreeVector v(m_Distance*sinTheta*cos(phi),
                  m_Distance*sinTheta*sin(phi),
                  m_Distance*cosTheta);
  v.rotateUz(-m_CenterDirection);
  
  G4ThreeVector v2 = v.orthogonal();
  v2.setMag( m_Radius * sqrt(G4UniformRand()) );
  G4double chi = twopi * G4UniformRand();
  v2.rotate(chi, v);

  G4ThreeVector position = m_CenterPosition + v + v2;
  G4ThreeVector direction = (-v).unit();

  // set energy
  G4double energy = sampleEnergy();

  setPrimary(position, energy, direction);
}

ANLStatus IsotropicPrimaryGen::mod_endrun()
{
  G4double area = pi*m_Radius*m_Radius;
  G4double realTime = 0.;
  G4double solid = 4*pi*m_CoveringFactor*sr;
  G4double pflux = 0.;
  if (m_CoveringFactor != 0.0) {
    realTime = TotalEnergy()/(m_Flux*area*solid);
    pflux = Number()/area/realTime/solid;
  }
  
  G4cout.setf(std::ios::scientific);
  G4cout << "IsotropicPrimaryGen::mod_endrun \n"
         << "  Number: " << Number() << "\n"
         << "  Flux: " << m_Flux/(erg/cm2/s/sr) << " erg/cm2/s/sr\n"
         << "  Total Energy: " << TotalEnergy()/keV << " keV = "
         << TotalEnergy()/erg << " erg\n"
         << "  Covering Factor: " << m_CoveringFactor << "\n"
         << "  Area: " << area/cm2 << " cm2\n"
         << "  Real time: " << realTime/s << " s\n"
         << "  Photon flux: " << pflux/(1.0/cm2/s/sr) << " photons/cm2/s/sr\n"
         << G4endl;
  G4cout.unsetf(std::ios::scientific);

  return AS_OK;
}

} /* namespace anlgeant4 */
