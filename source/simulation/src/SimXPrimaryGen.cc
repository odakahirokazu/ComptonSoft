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

#include "SimXPrimaryGen.hh"

#include "Randomize.hh"
#include "AstroUnits.hh"
#include "PhaseSpaceVector.hh"
#include "SimXIF.hh"

using namespace anl;
using namespace anlgeant4;

namespace comptonsoft
{

SimXPrimaryGen::SimXPrimaryGen()
  : m_SimXIF(0),
    m_CenterPosition(0.0, 0.0, 0.0),
    m_Radius(100.0*unit::cm),
    m_Polarization0(1.0, 0.0, 0.0),
    m_PolarizationDegree(0.0),
    m_Flux(1.0e-9*(unit::erg/unit::s/unit::cm2))
{
  add_alias("SimXPrimaryGen");
}

SimXPrimaryGen::~SimXPrimaryGen()
{
}

ANLStatus SimXPrimaryGen::mod_startup()
{
  BasicPrimaryGen::mod_startup();

  register_parameter(&m_CenterPosition, "center_position", unit::cm, "cm");
  set_parameter_description("Position of the sphere.");
  register_parameter(&m_Radius, "radius", unit::cm, "cm");
  set_parameter_description("Radius of the sphere.");

  register_parameter(&m_Polarization0, "polarization_vector");
  set_parameter_description("Polarization vector (if polarization is enable).");
  register_parameter(&m_PolarizationDegree, "degree_of_polarization");
  set_parameter_description("Degree of polarization (if polarization is enable).");

  register_parameter(&m_Flux, "flux", (unit::erg/unit::cm2/unit::s/unit::sr), "erg/cm2/s/sr");
  set_parameter_description("Energy flux of the primaries. This parameter is used only for calculating real time correspoing to a simulation.");

  unregister_parameter("photon_index");
  unregister_parameter("energy_min");
  unregister_parameter("energy_max");

  return AS_OK;
}

ANLStatus SimXPrimaryGen::mod_init()
{
  G4double area = CLHEP::pi * m_Radius * m_Radius;
  GetModuleNC("SimXIF", &m_SimXIF);
  m_SimXIF->generatePrimaries(area);
  std::cout << "Number of primaries: " << m_SimXIF->NumberOfPrimaries() << std::endl;
  BasicPrimaryGen::mod_init();
  
  return AS_OK;
}

void SimXPrimaryGen::makePrimarySetting()
{
  using std::sqrt;

  PhaseSpaceVector primary = m_SimXIF->takePrimary();

  G4double energy = primary.Energy();
  G4ThreeVector direction(primary.DirectionX(),
                          primary.DirectionY(),
                          primary.DirectionZ());
  setTime(primary.Time());

  G4ThreeVector v = (-m_Radius) * direction;
  G4ThreeVector v2 = v.orthogonal();
  v2.setMag( m_Radius * sqrt(G4UniformRand()) );
  G4double chi = CLHEP::twopi * G4UniformRand();
  v2.rotate(chi, v);
  G4ThreeVector position = m_CenterPosition + v + v2;

  if (PolarizationMode()==0) {
    setPrimary(position, energy, direction);
    setUnpolarized();
  }
  else if (PolarizationMode()==1) {
    G4ThreeVector polarization = (m_Polarization0 - (m_Polarization0.dot(direction))*direction).unit();
    setPrimary(position, energy, direction, polarization);
  }
  else if (PolarizationMode()==2) {
    if (G4UniformRand() < m_PolarizationDegree) {
      G4ThreeVector polarization = (m_Polarization0 - (m_Polarization0.dot(direction))*direction).unit(); 
      setPrimary(position, energy, direction, polarization);
    }
    else {
      setPrimary(position, energy, direction);
      setUnpolarized();
    }
  }
  else {
    setPrimary(position, energy, direction);
  }
}

ANLStatus SimXPrimaryGen::mod_endrun()
{
  const double area = CLHEP::pi*m_Radius*m_Radius;
  const double realTime = TotalEnergy()/(m_Flux*area);
  const double pflux = Number()/area/realTime;
    
  std::cout.setf(std::ios::scientific);
  std::cout << "SimXPrimaryGen::mod_endrun \n"
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

} /* namespace comptonsoft */
