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
#include "AstroUnit.hh"
#include "PhaseSpaceVector.hh"
#include "SimXIF.hh"

using namespace anl;
using namespace anlgeant4;
using namespace comptonsoft;


SimXPrimaryGen::SimXPrimaryGen()
  : m_SimXIF(0),
    m_CenterPosition(0.0, 0.0, 0.0),
    m_Radius(100.0*cm),
    m_Polarization0(1.0, 0.0, 0.0),
    m_PolarizationDegree(0.0),
    m_Flux(1.0e-9*(erg/s/cm2))
{
  add_alias("SimXPrimaryGen");
}


SimXPrimaryGen::~SimXPrimaryGen()
{
}


ANLStatus SimXPrimaryGen::mod_startup()
{
  BasicPrimaryGen::mod_startup();

  register_parameter(&m_CenterPosition, "Center position", cm, "cm");
  set_parameter_description("Position of the sphere.");
  register_parameter(&m_Radius, "Radius", cm, "cm");
  set_parameter_description("Radius of the sphere.");

  register_parameter(&m_Polarization0, "Polarization vector");
  set_parameter_description("Polarization vector (if polarization is enable).");
  register_parameter(&m_PolarizationDegree, "Degree of polarization");
  set_parameter_description("Degree of polarization (if polarization is enable).");

  register_parameter(&m_Flux, "Flux", (erg/cm2/s/sr), "erg/cm2/s/sr");
  set_parameter_description("Energy flux of the primaries. This parameter is used only for calculating real time correspoing to a simulation.");

  unregister_parameter("Photon index");
  unregister_parameter("Energy min");
  unregister_parameter("Energy max");

  return AS_OK;
}


ANLStatus SimXPrimaryGen::mod_init()
{
  G4double area = pi * m_Radius * m_Radius;
  GetANLModuleNC("SimXIF", &m_SimXIF);
  m_SimXIF->generatePrimaries(area);
  std::cout << "Number of primaries: " << m_SimXIF->NumberOfPrimaries() << std::endl;
  BasicPrimaryGen::mod_init();
  
  return AS_OK;
}


ANLStatus SimXPrimaryGen::mod_ana()
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
  G4double chi = twopi * G4UniformRand();
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

  return BasicPrimaryGen::mod_ana();
}


ANLStatus SimXPrimaryGen::mod_endrun()
{
  G4double area = pi*m_Radius*m_Radius;
  G4double realTime = 0.;
  G4double pflux = 0.;
  realTime = TotalEnergy()/(m_Flux*area);
  pflux = Number()/area/realTime;
    
  G4cout.setf(std::ios::scientific);
  G4cout << "SimXPrimaryGen::mod_endrun \n"
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
