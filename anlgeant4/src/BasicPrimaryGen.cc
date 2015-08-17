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

#include <algorithm>
#include <iterator>
#include <boost/format.hpp>
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"
#include "BasicPrimaryGeneratorAction.hh"
#include "VANLGeometry.hh"

// using CLHEP::keV;
using namespace anl;

namespace anlgeant4
{

BasicPrimaryGen::BasicPrimaryGen()
  : InitialInformation(true),
    primaryGenerator_(0), pickupData_(0), geometry_(0),
    particleName_("gamma"),
    time_(0.0),
    position_(0.0, 0.0, 0.0),
    energy_(0.0), direction_(0.0, 0.0, -1.0),
    polarization_(0.0, 0.0, 0.0),
    number_(0), totalEnergy_(0.0),
    definition_(0),
    polarizationMode_(-1),
    energyDistributionName_("power law"),
    energyDistribution_(SpectralShape::Undefined),
    energyMin_(0.1*keV), energyMax_(1000.0*keV),
    photonIndex_(0.0),
    energyMean_(511.0*keV), energySigma_(2.0*keV),
    kT_(10.0*keV)
{
  add_alias("BasicPrimaryGen");
  add_alias("InitialInformation");
}

ANLStatus BasicPrimaryGen::mod_startup()
{
  VANLPrimaryGen::mod_startup();

  GetANLModule("VANLGeometry", &geometry_);

  register_parameter(&particleName_, "particle");
  set_parameter_description("Particle name (gamma, e-, e+, proton, neutron, geantino...)");

  register_parameter(&energyDistributionName_, "spectral_distribution");

  register_parameter(&energyMin_, "energy_min", keV, "keV");
  set_parameter_description("Minimum value of the energy distribution");
  register_parameter(&energyMax_, "energy_max", keV, "keV");
  set_parameter_description("Maximum value of the energy distribution");
  register_parameter(&photonIndex_, "photon_index");
  set_parameter_description("Power law index of the photon spectrum");
  register_parameter(&energyMean_, "energy_mean", keV, "keV");
  set_parameter_description("Mean energy of the Gaussian distribution");
  register_parameter(&energySigma_, "energy_sigma", keV, "keV");
  set_parameter_description("Standard deviation of the Gaussian distribution");
  register_parameter(&kT_, "radiation_temperature", keV, "keV");
  set_parameter_description("Radiation temperature in units of keV");
  register_parameter(&spectrumEnergy_, "energy_array", keV, "keV");
  set_parameter_description("Energy array of spectral histogram");
  register_parameter(&spectrumPhotons_, "photons_array");
  set_parameter_description("Photons array of spectral histogram");

  disableDefaultEnergyInput();

  register_parameter(&polarizationMode_, "polarization_mode");
  set_parameter_description("Polarization mode. -1: polarization disable, 0: unpolarized, 1: linearly polarized (100%), 2: partially linearly polarized");

  return AS_OK;
}

ANLStatus BasicPrimaryGen::mod_prepare()
{
  disableDefaultEnergyInput();
  
  if (energyDistribution_==SpectralShape::Undefined) {
    if (energyDistributionName_=="user") {
      energyDistribution_ = SpectralShape::User;
    }
    else if (energyDistributionName_=="mono") {
      energyDistribution_ = SpectralShape::Mono;
      enableGaussianInput();
    }
    else if (energyDistributionName_=="power law") {
      energyDistribution_ = SpectralShape::PowerLaw;
      enablePowerLawInput();
    }
    else if (energyDistributionName_=="gaussian") {
      energyDistribution_ = SpectralShape::Gaussian;
      enableGaussianInput();
    }
    else if (energyDistributionName_=="black body") {
      energyDistribution_ = SpectralShape::BlackBody;
      enableBlackBodyInput();
    }
    else if (energyDistributionName_=="histogram") {
      energyDistribution_ = SpectralShape::Histogram;
      enableHistogramInput();
    }
    else if (energyDistributionName_=="undefined") {
      energyDistribution_ = SpectralShape::Undefined;
    }
    else {
      std::cout << "Invalid input to \"Energy distribution\": "
                << energyDistributionName_
                << std::endl;
      return AS_QUIT_ERR;
    }
  }

  return AS_OK;
}

ANLStatus BasicPrimaryGen::mod_init()
{
  GetANLModule("PickUpData", &pickupData_);
 
  EvsDef("Polarization enable");
  if (PolarizationMode()>=0) {
    EvsSet("Polarization enable");
  }

  if (energyMin_ == 0.0) {
    energyMin_ = 1.0e-9 * keV;
    std::cout << "Energy min is reset to 1.0e-9 keV." << std::endl;
  }

  if (energyDistribution_ == SpectralShape::Histogram) {
    if (spectrumEnergy_.size() != spectrumPhotons_.size()+1) {
      std::cout << "Spectral historam binning is invalid.\n"
                << "Should be energy_array.size == photons_array.size+1."
                << std::endl;
      return AS_QUIT_ERR;
    }

    buildSpectrumPhotonIntegral();
  }
  
  return AS_OK;
}

ANLStatus BasicPrimaryGen::mod_bgnrun()
{
  totalEnergy_ = 0.0;
  return AS_OK;
}

ANLStatus BasicPrimaryGen::mod_ana()
{
  number_++;
  totalEnergy_ += energy_;
  primaryGenerator_->Set(time_, position_, energy_, direction_, polarization_);

  setInitialEnergy(energy_);
  setInitialDirection(direction_);
  setInitialTime(time_);
  setInitialPosition(position_);
  setInitialPolarization(polarization_);

  return AS_OK;
}

void BasicPrimaryGen::setDefinition(G4ParticleDefinition* def)
{
  definition_ = def;
  if (primaryGenerator_) primaryGenerator_->SetDefinition(def);
}

void BasicPrimaryGen::enablePowerLawInput()
{
  expose_parameter("photon_index");
  expose_parameter("energy_min");
  expose_parameter("energy_max");
}

void BasicPrimaryGen::enableGaussianInput()
{
  expose_parameter("energy_mean");
  expose_parameter("energy_sigma");
}

void BasicPrimaryGen::enableBlackBodyInput()
{
  expose_parameter("radiation_temperature");
  expose_parameter("energy_max");
}

void BasicPrimaryGen::enableHistogramInput()
{
  expose_parameter("energy_array");
  expose_parameter("photons_array");
}

void BasicPrimaryGen::disableDefaultEnergyInput()
{
  hide_parameter("energy_min");
  hide_parameter("energy_max");
  hide_parameter("photon_index");
  hide_parameter("energy_mean");
  hide_parameter("energy_sigma");
  hide_parameter("radiation_temperature");
  hide_parameter("energy_array");
  hide_parameter("photons_array");
}

void BasicPrimaryGen::printSpectralInfo()
{
  switch (energyDistribution_) {
  case SpectralShape::Mono:
    G4cout << "  Spectral shape: mono => "
           << "Mean: " << energyMean_/keV  << " keV"
           << G4endl;
    break;
  case SpectralShape::PowerLaw:
    G4cout << "  Spectral shape: power law =>"
           << " photon index = " << photonIndex_
           << " ( " << energyMin_/keV  << " -- " << energyMax_/keV  << " keV )"
           << G4endl;
    break;
  case SpectralShape::Gaussian:
    G4cout << "  Spectral shape: Gaussian =>"
           << " mean: " << energyMean_/keV  << " keV ;"
           << " sigma: " << energySigma_/keV  << " keV"
           << G4endl;
    break;
  case SpectralShape::BlackBody:
    G4cout << "  Spectral shape: black body => "
           << " temperature: " << kT_/keV  << " keV"
           << " ( < " << energyMax_/keV  << " keV )"
           << G4endl;
    break;
  default:
    break;
  }
}

G4double BasicPrimaryGen::sampleEnergy()
{
  switch (energyDistribution_) {
    case SpectralShape::Mono:
      break;
    case SpectralShape::PowerLaw:
      return sampleFromPowerLaw();
    case SpectralShape::Gaussian:
      return sampleFromGaussian();
    case SpectralShape::BlackBody:
      return sampleFromBlackBody();
    case SpectralShape::Histogram:
      return sampleFromHistogram();
    default:
      break;
  }
  return energyMean_;
}

G4double BasicPrimaryGen::sampleFromPowerLaw()
{
  return sampleFromPowerLaw(photonIndex_, energyMin_, energyMax_);
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
  return sampleFromGaussian(energyMean_, energySigma_);
}

G4double BasicPrimaryGen::sampleFromGaussian(double mean, double sigma)
{
  double x = CLHEP::RandGauss::shoot(CLHEP::HepRandom::getTheEngine());
  double energy = mean + x * sigma;
  return energy;
}

G4double BasicPrimaryGen::sampleFromBlackBody()
{
  return sampleFromBlackBody(kT_, energyMax_/kT_);
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

void BasicPrimaryGen::buildSpectrumPhotonIntegral()
{
  std::size_t NBins = spectrumPhotons_.size();
  spectrumPhotonIntegral_.resize(spectrumEnergy_.size());
  spectrumPhotonIntegral_[0] = 0.0;
  for (std::size_t i=0; i<NBins; i++) {
    spectrumPhotonIntegral_[i+1]
      = spectrumPhotonIntegral_[i] + spectrumPhotons_[i];
  }
  const double Norm = spectrumPhotonIntegral_.back();
  for (auto& v: spectrumPhotonIntegral_) {
    v /= Norm;
  }

  std::cout << "Spectrum: (energy in keV, photon integral)\n";
  for (std::size_t i=0; i<spectrumEnergy_.size(); i++) {
    std::cout << boost::format("%10.3E %8.3f")
      % (spectrumEnergy_[i]/keV) % spectrumPhotonIntegral_[i] << '\n';
  }
  std::cout << std::endl;
}

G4double BasicPrimaryGen::sampleFromHistogram()
{
  const std::vector<double>& energies = spectrumEnergy_;
  const std::vector<double>& integrals = spectrumPhotonIntegral_;
  const double r = G4UniformRand();
  const std::vector<double>::const_iterator it
    = std::upper_bound(std::begin(integrals), std::end(integrals), r);
  const double r0 = *(it-1);
  const double r1 = *it;
  const std::vector<double>::const_iterator itEnergy
    = std::begin(energies)+(it-std::begin(integrals));
  const double energy0 = *(itEnergy-1);
  const double energy1 = *itEnergy;
  const double energy = energy0 + (energy1-energy0)*(r-r0)/(r1-r0);
  return energy;
}

void BasicPrimaryGen::setUnpolarized()
{
  G4double phi2 = twopi * G4UniformRand();
  G4ThreeVector directionOrthogonal = direction_.orthogonal().unit();
  polarization_ = directionOrthogonal.rotate(phi2, direction_);
}

G4VUserPrimaryGeneratorAction* BasicPrimaryGen::create()
{
  if (definition_) {
    primaryGenerator_ = new BasicPrimaryGeneratorAction(definition_);
  }
  else if (particleName_ != "") {
    primaryGenerator_ = new BasicPrimaryGeneratorAction(particleName_);
  }
  else {
    primaryGenerator_ = new BasicPrimaryGeneratorAction;
  }
  return primaryGenerator_;
}

double BasicPrimaryGen::LengthUnit() const
{
  return geometry_->GetLengthUnit();
}

std::string BasicPrimaryGen::LengthUnitName() const
{
  return geometry_->GetLengthUnitName();
}

} /* namespace anlgeant4 */
