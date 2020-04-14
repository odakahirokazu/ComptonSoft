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
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

PlaneWavePrimaryGen::PlaneWavePrimaryGen()
  : m_CenterPosition(0.0, 0.0, 0.0),
    m_Direction0(0.0, 0.0, -1.0),
    m_DirectionOrthogonal(0.0, 1.0, 0.0),
    m_Radius(100.0*unit::cm),
    m_Polarization0(1.0, 0.0, 0.0),
    m_PolarizationDegree(0.0),
    m_Flux(1.0e-9*(unit::erg/unit::s/unit::cm2))
{
  add_alias("PlaneWavePrimaryGen");
}

PlaneWavePrimaryGen::~PlaneWavePrimaryGen() = default;

ANLStatus PlaneWavePrimaryGen::mod_define()
{
  BasicPrimaryGen::mod_define();

  enablePowerLawInput();
  register_parameter(&m_CenterPosition, "position",
                     LengthUnit(), LengthUnitName());
  set_parameter_description("Center of the circle where primary particles are generated.");
  register_parameter(&m_Direction0, "direction");
  set_parameter_description("Propagation direction of the plane wave.");
  register_parameter(&m_Radius, "radius",
                     LengthUnit(), LengthUnitName());
  set_parameter_description("Radius of the circle where parimary particles are generated.");
  register_parameter(&m_Flux, "flux", unit::erg/unit::s/unit::cm2, "erg/s/cm2");
  set_parameter_description("Energy flux of the plane wave. This parameter is used only for calculating real time correspoing to a simulation.");
  register_parameter(&m_Polarization0, "polarization_vector");
  set_parameter_description("Polarization vector");
  register_parameter(&m_PolarizationDegree, "degree_of_polarization");
  set_parameter_description("Degree of polarization");
 
  return AS_OK;
}

ANLStatus PlaneWavePrimaryGen::mod_initialize()
{
  BasicPrimaryGen::mod_initialize();

  m_Direction0 = m_Direction0.unit();
  m_DirectionOrthogonal = m_Direction0.orthogonal().unit();

  std::cout << "--------" << std::endl;
  std::cout << "PrimaryGen status (plane wave)" << std::endl;
  const double posx = m_CenterPosition.x();
  const double posy = m_CenterPosition.y();
  const double posz = m_CenterPosition.z();
  std::cout << "  Center position: "
            << posx/unit::cm << " " << posy/unit::cm << " " << posz/unit::cm << " cm" << std::endl;
  printSpectralInfo();
  const double dirx = m_Direction0.x();
  const double diry = m_Direction0.y();
  const double dirz = m_Direction0.z();
  std::cout << "  Direction: "
            << dirx << " " << diry << " " << dirz << std::endl;
  
  return AS_OK;
}

void PlaneWavePrimaryGen::makePrimarySetting()
{
  const G4ThreeVector position = samplePosition();
  const double energy = sampleEnergy();
  
  if (G4UniformRand() < m_PolarizationDegree) {
    setPrimary(position, energy, m_Direction0, m_Polarization0);
  }
  else {
    setPrimary(position, energy, m_Direction0);
    setUnpolarized();
  }
}

G4ThreeVector PlaneWavePrimaryGen::samplePosition()
{
  using std::sqrt;

  G4ThreeVector position(m_DirectionOrthogonal);
  const double r = m_Radius * sqrt(G4UniformRand());
  const double t = CLHEP::twopi * G4UniformRand();
  position.rotate(t, m_Direction0);
  position = m_CenterPosition + r * position;
  return position;
}

ANLStatus PlaneWavePrimaryGen::mod_end_run()
{
  const double area = GenerationArea();
  const double realTime = TotalEnergy()/(m_Flux*area);
  const double pflux = Number()/area/realTime;

  setRealTime(realTime);
  
  std::cout.setf(std::ios::scientific);
  std::cout << "PWPrimaryGen::mod_end_run \n"
            << "  Number: " << Number() << "\n"
            << "  Flux: " << m_Flux/(unit::erg/unit::cm2/unit::s) << " erg/cm2/s\n"
            << "  Total Energy: " << TotalEnergy()/unit::keV << " keV = "
            << TotalEnergy()/unit::erg << " erg\n"
            << "  Area: " << area/unit::cm2 << " cm2\n"
            << "  Real time: " << realTime/unit::s << " s\n"
            << "  Photon flux: " << pflux/(1.0/unit::cm2/unit::s) << " photons/cm2/s\n"
            << std::endl;
  std::cout.unsetf(std::ios::scientific);
  
  return AS_OK;
}

double PlaneWavePrimaryGen::GenerationArea()
{
  return CLHEP::pi*m_Radius*m_Radius;
}

} /* namespace anlgeant4 */
