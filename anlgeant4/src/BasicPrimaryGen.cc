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

#include "BasicPrimaryGen.hh"

#include "Randomize.hh"
#include "BasicPrimaryGeneratorAction.hh"

using namespace anl;
using namespace anlgeant4;


BasicPrimaryGen::BasicPrimaryGen()
  : InitialInformation(true),
    m_PrimaryGen(0), m_PickUpData(0),
    m_ParticleName("gamma"),
    m_Time(0.0),
    m_Position(0.0, 0.0, 0.0),
    m_Energy(0.0), m_Direction(0.0, 0.0, -1.0),
    m_Polarization(0.0, 0.0, 0.0),
    m_Number(0), m_TotalEnergy(0.0),
    m_Definition(0),
    m_PhotonIndex(0.0), m_EnergyMin(0.1*keV), m_EnergyMax(1000.0*keV),
    m_PolarizationMode(-1)
{
  add_alias("BasicPrimaryGen");
  add_alias("InitialInformation");
}


ANLStatus BasicPrimaryGen::mod_startup()
{
  VANLPrimaryGen::mod_startup();
  register_parameter(&m_ParticleName, "Particle name");
  set_parameter_description("Particle name (gamma, electron, positron, proton, neutron, geantino...)");

  register_parameter(&m_PhotonIndex, "Photon index");
  set_parameter_description("Power law index of the photon spectrum");
  register_parameter(&m_EnergyMin, "Energy min", keV, "keV");
  set_parameter_description("Minimum value of the energy distribution");
  register_parameter(&m_EnergyMax, "Energy max", keV, "keV");
  set_parameter_description("Maximum value of the energy distribution");

  disablePowerLawInput();

  register_parameter(&m_PolarizationMode, "Polarization mode");
  set_parameter_description("Polarization mode. -1: polarization disable, 0: unpolarized, 1: linearly polarized.");
  hide_parameter("Polarization mode");

  return AS_OK;
}


ANLStatus BasicPrimaryGen::mod_init()
{
  GetANLModule("PickUpData", &m_PickUpData);
 
  EvsDef("Polarization enable");
  if (PolarizationMode()>=0) {
    EvsSet("Polarization enable");
  }

  if (m_EnergyMin == 0.0) {
    m_EnergyMin = 1.0e-9 * keV;
    std::cout << "Energy min is reset to 1.0e-9 keV." << std::endl;
  }
  
  return AS_OK;
}


ANLStatus BasicPrimaryGen::mod_bgnrun()
{
  m_TotalEnergy = 0.0;
  return AS_OK;
}


ANLStatus BasicPrimaryGen::mod_ana()
{
  m_Number++;
  m_TotalEnergy += m_Energy;
  m_PrimaryGen->Set(m_Time, m_Position, m_Energy, m_Direction, m_Polarization);

  setEventID(m_PickUpData->EventID());
  setInitialEnergy(m_Energy);
  setInitialDirection(m_Direction);
  setInitialTime(m_Time);
  setInitialPosition(m_Position);
  setInitialPolarization(m_Polarization);

  return AS_OK;
}


void BasicPrimaryGen::setDefinition(G4ParticleDefinition* def)
{
  m_Definition = def;
  if (m_PrimaryGen) m_PrimaryGen->SetDefinition(def);
}


void BasicPrimaryGen::enablePowerLawInput()
{
  expose_parameter("Photon index");
  expose_parameter("Energy min");
  expose_parameter("Energy max");
}


void BasicPrimaryGen::disablePowerLawInput()
{
  hide_parameter("Photon index");
  hide_parameter("Energy min");
  hide_parameter("Energy max");
}


void BasicPrimaryGen::printSpectralInfo()
{
  G4cout << "  Power-Law:"
         << "  From: " << m_EnergyMin/keV  << " keV" 
         << "  To: " << m_EnergyMax/keV  << " keV" 
         << "  Photon Index: " << m_PhotonIndex << G4endl;
}


G4double BasicPrimaryGen::sampleEnergy()
{
  using std::pow;

  G4double energy = 0.0;
  if ( 0.999 < m_PhotonIndex && m_PhotonIndex < 1.001 ) {
    // Photon index ~ 1
    energy = m_EnergyMin * pow(m_EnergyMax/m_EnergyMin, G4UniformRand());
  }
  else {
    G4double s = 1.0-m_PhotonIndex;
    G4double a0 = pow(m_EnergyMin, s);
    G4double a1 = pow(m_EnergyMax, s);
    G4double a = a0 + G4UniformRand()*(a1-a0);
    energy = pow(a, 1./s);
  }

  // std::cout << energy / keV << std::endl;

  return energy;
}


void BasicPrimaryGen::setUnpolarized()
{
  G4double phi2 = twopi * G4UniformRand();
  G4ThreeVector directionOrthogonal = m_Direction.orthogonal().unit();
  m_Polarization = directionOrthogonal.rotate(phi2, m_Direction);
}


G4VUserPrimaryGeneratorAction* BasicPrimaryGen::create()
{
  if (m_Definition) {
    m_PrimaryGen = new BasicPrimaryGeneratorAction(m_Definition);
  }
  else if (m_ParticleName != "") {
    m_PrimaryGen = new BasicPrimaryGeneratorAction(m_ParticleName);
  }
  else {
    m_PrimaryGen = new BasicPrimaryGeneratorAction;
  }
  return m_PrimaryGen;
}
