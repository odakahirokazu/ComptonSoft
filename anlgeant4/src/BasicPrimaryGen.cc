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
#include "VANLGeometry.hh"

using namespace anl;
using namespace anlgeant4;


BasicPrimaryGen::BasicPrimaryGen()
  : InitialInformation(true),
    m_PrimaryGen(0), m_PickUpData(0), m_Geometry(0),
    m_ParticleName("gamma"),
    m_Time(0.0),
    m_Position(0.0, 0.0, 0.0),
    m_Energy(0.0), m_Direction(0.0, 0.0, -1.0),
    m_Polarization(0.0, 0.0, 0.0),
    m_Number(0), m_TotalEnergy(0.0),
    m_Definition(0),
    m_PolarizationMode(-1),
    m_EnergyDistributionName("power law"), m_EnergyDistribution(Undefined),
    m_EnergyMin(0.1*keV), m_EnergyMax(1000.0*keV),
    m_PhotonIndex(0.0), 
    m_EnergyMean(511.0*keV), m_EnergySigma(2.0*keV),
    m_KT(10.0*keV)
{
  add_alias("BasicPrimaryGen");
  add_alias("InitialInformation");
}


ANLStatus BasicPrimaryGen::mod_startup()
{
  VANLPrimaryGen::mod_startup();

  GetANLModule("VANLGeometry", &m_Geometry);

  register_parameter(&m_ParticleName, "Particle name");
  set_parameter_description("Particle name (gamma, e-, e+, proton, neutron, geantino...)");

  register_parameter(&m_EnergyDistributionName, "Energy distribution");

  register_parameter(&m_EnergyMin, "Energy min", keV, "keV");
  set_parameter_description("Minimum value of the energy distribution");
  register_parameter(&m_EnergyMax, "Energy max", keV, "keV");
  set_parameter_description("Maximum value of the energy distribution");
  register_parameter(&m_PhotonIndex, "Photon index");
  set_parameter_description("Power law index of the photon spectrum");
  register_parameter(&m_EnergyMean, "Energy mean", keV, "keV");
  set_parameter_description("Mean energy of the Gaussian distribution");
  register_parameter(&m_EnergySigma, "Energy sigma", keV, "keV");
  set_parameter_description("Standard deviation of the Gaussian distribution");
  register_parameter(&m_KT, "Radiation temperature", keV, "keV");
  set_parameter_description("Radiation temperature in units of keV");

  disableDefaultEnergyInput();

  register_parameter(&m_PolarizationMode, "Polarization mode");
  set_parameter_description("Polarization mode. -1: polarization disable, 0: unpolarized, 1: linearly polarized (100%), 2: partially linearly polarized");

  return AS_OK;
}


ANLStatus BasicPrimaryGen::mod_prepare()
{
  disableDefaultEnergyInput();
  
  if (m_EnergyDistribution==Undefined) {
    if (m_EnergyDistributionName=="user") {
      m_EnergyDistribution = User;
    }
    else if (m_EnergyDistributionName=="mono") {
      m_EnergyDistribution = Mono;
      enableGaussianInput();
    }
    else if (m_EnergyDistributionName=="power law") {
      m_EnergyDistribution = PowerLaw;
      enablePowerLawInput();
    }
    else if (m_EnergyDistributionName=="gaussian") {
      m_EnergyDistribution = Gaussian;
      enableGaussianInput();
    }
    else if (m_EnergyDistributionName=="black body") {
      m_EnergyDistribution = BlackBody;
      enableBlackBodyInput();
    }
    else if (m_EnergyDistributionName=="undefined") {
      m_EnergyDistribution = Undefined;
    }
    else {
      std::cout << "Invalid input to \"Energy distribution\": "
                << m_EnergyDistributionName
                << std::endl;
      return AS_QUIT_ERR;
    }
  }

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


void BasicPrimaryGen::enableGaussianInput()
{
  expose_parameter("Energy mean");
  expose_parameter("Energy sigma");
}


void BasicPrimaryGen::enableBlackBodyInput()
{
  expose_parameter("Radiation temperature");
  expose_parameter("Energy max");
}


void BasicPrimaryGen::disableDefaultEnergyInput()
{
  hide_parameter("Energy min");
  hide_parameter("Energy max");
  hide_parameter("Photon index");
  hide_parameter("Energy mean");
  hide_parameter("Energy sigma");
  hide_parameter("Radiation temperature");
}


void BasicPrimaryGen::printSpectralInfo()
{
  switch (m_EnergyDistribution) {
  case Mono:
    G4cout << "  Spectral shape: mono => "
           << "Mean: " << m_EnergyMean/keV  << " keV" 
           << G4endl;
    break;
  case PowerLaw:
    G4cout << "  Spectral shape: power law =>"
           << " photon index = " << m_PhotonIndex
           << " ( " << m_EnergyMin/keV  << " -- " << m_EnergyMax/keV  << " keV )"
           << G4endl;
    break;
  case Gaussian:
    G4cout << "  Spectral shape: Gaussian =>"
           << " mean: " << m_EnergyMean/keV  << " keV ;"
           << " sigma: " << m_EnergySigma/keV  << " keV" 
           << G4endl;
    break;
  case BlackBody:
    G4cout << "  Spectral shape: black body => "
           << " temperature: " << m_KT/keV  << " keV" 
           << " ( < " << m_EnergyMax/keV  << " keV )"
           << G4endl;
    break;
  default:
    break;
  }
}


G4double BasicPrimaryGen::sampleEnergy()
{
  switch (m_EnergyDistribution) {
  case Mono:
    break;
  case PowerLaw:
    return sampleFromPowerLaw();
  case Gaussian:
    return sampleFromGaussian();
  case BlackBody:
    return sampleFromBlackBody();
  default:
    break;
  }
  return m_EnergyMean;
}


G4double BasicPrimaryGen::sampleFromPowerLaw()
{
  return sampleFromPowerLaw(m_PhotonIndex, m_EnergyMin, m_EnergyMax);
}


G4double BasicPrimaryGen::sampleFromPowerLaw(double gamma, double e0, double e1)
{
  using std::pow;

  G4double energy = 0.0;
  if ( 0.999 < gamma && gamma < 1.001 ) {
    // Photon index ~ 1
    energy = e0 * pow(e1/e0, G4UniformRand());
  }
  else {
    G4double s = 1.0-gamma;
    G4double a0 = pow(e0, s);
    G4double a1 = pow(e1, s);
    G4double a = a0 + G4UniformRand()*(a1-a0);
    energy = pow(a, 1./s);
  }

  return energy;
}


G4double BasicPrimaryGen::sampleFromGaussian()
{
  return sampleFromGaussian(m_EnergyMean, m_EnergySigma);
}


G4double BasicPrimaryGen::sampleFromGaussian(double mean, double sigma)
{
  double x = CLHEP::RandGauss::shoot(CLHEP::HepRandom::getTheEngine());
  double energy = mean + x * sigma;
  return energy;
}


G4double BasicPrimaryGen::sampleFromBlackBody()
{
  return sampleFromBlackBody(m_KT, m_EnergyMax/m_KT);
}


G4double BasicPrimaryGen::sampleFromBlackBody(double kT,
                                              double upper_limit_factor)
{
  // sample from f(x) = x^2/(e^x-1)
  using std::exp;

  const double xmax = upper_limit_factor;
  const double ymax = 0.65; // an upper bound of f(x)
  double x=0.0, y=0.0, fx=0.0;
  while (1) {
    x = xmax * G4UniformRand();
    y = ymax * G4UniformRand();
    fx = x*x/(exp(x)-1.0);
    if (y <= fx) break;
  }
  double energy = kT * x;
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


double BasicPrimaryGen::LengthUnit() const
{
  return m_Geometry->GetLengthUnit();
}

  
std::string BasicPrimaryGen::LengthUnitName() const
{
  return m_Geometry->GetLengthUnitName();
}
