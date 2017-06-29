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

#include "PlaneWavePrimaryGen.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anl;

namespace anlgeant4
{

PlaneWavePrimaryGen::PlaneWavePrimaryGen()
  : m_CenterPosition(0.0, 0.0, 0.0),
    m_Direction0(0.0, 0.0, -1.0),
    m_DirectionOrthogonal(0.0, 1.0, 0.0),
    m_Radius(100.0*cm),
    m_Polarization0(1.0, 0.0, 0.0),
    m_PolarizationDegree(0.0),
    m_Flux(1.0e-9*(erg/s/cm2))
{
  add_alias("PlaneWavePrimaryGen");
}

PlaneWavePrimaryGen::~PlaneWavePrimaryGen() = default;

ANLStatus PlaneWavePrimaryGen::mod_startup()
{
  BasicPrimaryGen::mod_startup();

  enablePowerLawInput();
  register_parameter(&m_CenterPosition, "position",
                     LengthUnit(), LengthUnitName());
  set_parameter_description("Center of the circle where primary particles are generated.");
  register_parameter(&m_Direction0, "direction");
  set_parameter_description("Propagation direction of the plane wave.");
  register_parameter(&m_Radius, "radius",
                     LengthUnit(), LengthUnitName());
  set_parameter_description("Radius of the circle where parimary particles are generated.");
  register_parameter(&m_Flux, "flux", erg/s/cm2, "erg/s/cm2");
  set_parameter_description("Energy flux of the plane wave. This parameter is used only for calculating real time correspoing to a simulation.");
  register_parameter(&m_Polarization0, "polarization_vector");
  set_parameter_description("Polarization vector (if polarization is enable).");
  register_parameter(&m_PolarizationDegree, "degree_of_polarization");
  set_parameter_description("Degree of polarization (if polarization is enable).");
 
  return AS_OK;
}

ANLStatus PlaneWavePrimaryGen::mod_com()
{
  if (PolarizationMode() < 1) {
    hide_parameter("polarization_vector");
  }
  
  return BasicPrimaryGen::mod_com();
}

ANLStatus PlaneWavePrimaryGen::mod_init()
{
  BasicPrimaryGen::mod_init();

  m_Direction0 = m_Direction0.unit();
  m_DirectionOrthogonal = m_Direction0.orthogonal().unit();

  G4cout << "--------" << G4endl;
  G4cout << "PrimaryGen status (plane wave)" << G4endl;
  G4double posx = m_CenterPosition.x();
  G4double posy = m_CenterPosition.y();
  G4double posz = m_CenterPosition.z();
  G4cout << "  Center position: "
         << posx/cm << " " << posy/cm << " " << posz/cm << " cm" << G4endl;
  printSpectralInfo();
  G4double dirx = m_Direction0.x();
  G4double diry = m_Direction0.y();
  G4double dirz = m_Direction0.z();
  G4cout << "  Direction: "
         << dirx << " " << diry << " " << dirz << G4endl;
  
  return AS_OK;
}

void PlaneWavePrimaryGen::makePrimarySetting()
{
  const G4ThreeVector position = samplePosition();
  const G4double energy = sampleEnergy();
  
  if (PolarizationMode()==0) {
    setPrimary(position, energy, m_Direction0);
    setUnpolarized();
  }
  else if (PolarizationMode()==1) {
    setPrimary(position, energy, m_Direction0, m_Polarization0);
  }
  else if (PolarizationMode()==2) {
    if (G4UniformRand() < m_PolarizationDegree) {
      setPrimary(position, energy, m_Direction0, m_Polarization0);
    }
    else {
      setPrimary(position, energy, m_Direction0);
      setUnpolarized();
    }
  }
  else {
    setPrimary(position, energy, m_Direction0);
  }
}

G4ThreeVector PlaneWavePrimaryGen::samplePosition()
{
  using std::sqrt;

  G4ThreeVector position(m_DirectionOrthogonal);
  const double r = m_Radius * sqrt(G4UniformRand());
  const double t = twopi * G4UniformRand();
  position.rotate(t, m_Direction0);
  position = m_CenterPosition + r * position;
  return position;
}

ANLStatus PlaneWavePrimaryGen::mod_endrun()
{
  const double area = GenerationArea();
  const double realTime = TotalEnergy()/(m_Flux*area);
  const double pflux = Number()/area/realTime;
  
  G4cout.setf(std::ios::scientific);
  G4cout << "PWPrimaryGen::mod_endrun \n"
         << "  Number: " << Number() << "\n"
         << "  Flux: " << m_Flux/(erg/cm2/s) << " erg/cm2/s\n"
         << "  Total Energy: " << TotalEnergy()/keV << " keV = "
         << TotalEnergy()/erg << " erg\n"
         << "  Area: " << area/cm2 << " cm2\n"
         << "  Real time: " << realTime/s << " s\n"
         << "  Photon flux: " << pflux/(1.0/cm2/s) << " photons/cm2/s\n"
         << G4endl;
  G4cout.unsetf(std::ios::scientific);
  
  return AS_OK;
}

G4double PlaneWavePrimaryGen::GenerationArea()
{
  return pi*m_Radius*m_Radius;
}

} /* namespace anlgeant4 */
