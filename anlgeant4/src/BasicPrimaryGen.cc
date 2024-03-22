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
#include "Randomize.hh"
#include "G4Ions.hh"
#include "G4IonTable.hh"
#include "G4VIsotopeTable.hh"
#include "AstroUnits.hh"
#include "BasicPrimaryGeneratorAction.hh"
#include "VANLGeometry.hh"

using namespace anlnext;

namespace anlgeant4
{

BasicPrimaryGen::BasicPrimaryGen()
  : InitialInformation(true),
    particleName_("gamma"),
    nucleus_atomic_number_(0),
    nucleus_mass_number_(0),
    nucleus_excitation_energy_(0.0),
    nucleus_floating_level_(0),
    time_(0.0),
    position_(0.0, 0.0, 0.0),
    energy_(0.0), direction_(0.0, 0.0, -1.0),
    polarization_(0.0, 0.0, 0.0),
    number_(0), totalEnergy_(0.0), realTime_(0.0),
    definition_(0),
    energyDistributionName_("power law"),
    energyDistribution_(SpectralShape::Undefined),
    energyMin_(0.1*unit::keV), energyMax_(1000.0*unit::keV),
    photonIndex_(0.0),
    energyMean_(511.0*unit::keV), energySigma_(0.0*unit::keV),
    kT_(10.0*unit::keV)
{
  add_alias("BasicPrimaryGen");
  add_alias("InitialInformation");
}

BasicPrimaryGen::~BasicPrimaryGen() = default;

ANLStatus BasicPrimaryGen::mod_define()
{
  VANLPrimaryGen::mod_define();

  define_parameter("particle", &mod_class::particleName_);
  set_parameter_description("Particle name (gamma, e-, e+, proton, neutron, geantino...) Set this to \"nucleus\" for nucleus/isotope such as C-12, Cs-137.");

  define_parameter("nucleus_atomic_number", &mod_class::nucleus_atomic_number_);
  define_parameter("nucleus_mass_number", &mod_class::nucleus_mass_number_);
  define_parameter("nucleus_excitation_energy", &mod_class::nucleus_excitation_energy_, unit::keV, "keV");
  define_parameter("nucleus_floating_level", &mod_class::nucleus_floating_level_);

  hide_parameter("nucleus_atomic_number");
  hide_parameter("nucleus_mass_number");
  hide_parameter("nucleus_excitation_energy");
  hide_parameter("nucleus_floating_level");

  define_parameter("spectral_distribution", &mod_class::energyDistributionName_);

  define_parameter("energy_min", &mod_class::energyMin_, unit::keV, "keV");
  set_parameter_description("Minimum value of the energy distribution");
  define_parameter("energy_max", &mod_class::energyMax_, unit::keV, "keV");
  set_parameter_description("Maximum value of the energy distribution");
  define_parameter("photon_index", &mod_class::photonIndex_);
  set_parameter_description("Power law index of the photon spectrum");
  define_parameter("energy_mean", &mod_class::energyMean_, unit::keV, "keV");
  set_parameter_description("Mean energy of the Gaussian distribution");
  define_parameter("energy_sigma", &mod_class::energySigma_, unit::keV, "keV");
  set_parameter_description("Standard deviation of the Gaussian distribution");
  define_parameter("radiation_temperature", &mod_class::kT_, unit::keV, "keV");
  set_parameter_description("Radiation temperature in units of keV");
  define_parameter("energy_array", &mod_class::spectrumEnergy_, unit::keV, "keV");
  set_parameter_description("Energy array of spectral histogram");
  define_parameter("photons_array", &mod_class::spectrumPhotons_);
  set_parameter_description("Photons array of spectral histogram");

  define_result("real_time", &mod_class::realTime_, unit::s, "s");

  disableDefaultEnergyInput();

  return AS_OK;
}

ANLStatus BasicPrimaryGen::mod_pre_initialize()
{
  get_module("VANLGeometry", &geometry_);

  if (particleName_=="nucleus") {
    expose_parameter("nucleus_atomic_number");
    expose_parameter("nucleus_mass_number");
    expose_parameter("nucleus_excitation_energy");
    expose_parameter("nucleus_floating_level");

    setNucleusDefinition(nucleus_atomic_number_,
                         nucleus_mass_number_,
                         nucleus_excitation_energy_,
                         nucleus_floating_level_);
  }

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
      return AS_QUIT_ERROR;
    }
  }

  return AS_OK;
}

ANLStatus BasicPrimaryGen::mod_initialize()
{
  if (energyMin_ == 0.0) {
    energyMin_ = 1.0e-9 * unit::keV;
    std::cout << "Energy min is reset to 1.0e-9 keV." << std::endl;
  }

  if (energyDistribution_ == SpectralShape::Histogram) {
    if (spectrumEnergy_.size() != spectrumPhotons_.size()+1) {
      std::cout << "Spectral historam binning is invalid.\n"
                << "Should be energy_array.size == photons_array.size+1."
                << std::endl;
      return AS_QUIT_ERROR;
    }

    buildSpectrumPhotonIntegral();
  }
  
  return AS_OK;
}

ANLStatus BasicPrimaryGen::mod_begin_run()
{
  number_ = 0;
  totalEnergy_ = 0.0;

  return AS_OK;
}

void BasicPrimaryGen::confirmPrimarySetting()
{
  number_++;
  totalEnergy_ += energy_;
  primaryGenerator_->Set(time_, position_, energy_, direction_, polarization_);
  storeInitialCondition();
}

void BasicPrimaryGen::storeInitialCondition()
{
  setInitialEnergy(energy_);
  setInitialDirection(direction_);
  setInitialTime(time_);
  setInitialPosition(position_);
  setInitialPolarization(polarization_);
}

void BasicPrimaryGen::setDefinition(G4ParticleDefinition* def)
{
  definition_ = def;
  if (primaryGenerator_) {
    primaryGenerator_->SetDefinition(def);
  }
}

void BasicPrimaryGen::setNucleusDefinition(int atomic_number,
                                           int mass_number,
                                           double excitation_energy,
                                           int floating_level)
{
  G4IonTable* ionTable = static_cast<G4IonTable*>(G4ParticleTable::GetParticleTable()->GetIonTable());
  G4ParticleDefinition* particle_base =
    ionTable->GetIon(atomic_number,
                     mass_number,
                     excitation_energy,
                     G4Ions::FloatLevelBase(floating_level));
  G4Ions* particle = dynamic_cast<G4Ions*>(particle_base);
  if (particle == nullptr) {
    std::cout << "Error: the particle can not be converted into G4Ions." << std::endl;
  }
  setDefinition(particle);
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
    std::cout << "  Spectral shape: mono => "
              << "Mean: " << energyMean_/unit::keV  << " keV"
              << std::endl;
    break;
  case SpectralShape::PowerLaw:
    std::cout << "  Spectral shape: power law =>"
              << " photon index = " << photonIndex_
              << " ( " << energyMin_/unit::keV  << " -- " << energyMax_/unit::keV  << " keV )"
              << std::endl;
    break;
  case SpectralShape::Gaussian:
    std::cout << "  Spectral shape: Gaussian =>"
              << " mean: " << energyMean_/unit::keV  << " keV ;"
              << " sigma: " << energySigma_/unit::keV  << " keV"
              << std::endl;
    break;
  case SpectralShape::BlackBody:
    std::cout << "  Spectral shape: black body => "
              << " temperature: " << kT_/unit::keV  << " keV"
              << " ( < " << energyMax_/unit::keV  << " keV )"
              << std::endl;
    break;
  default:
    break;
  }
}

double BasicPrimaryGen::sampleEnergy()
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

double BasicPrimaryGen::sampleFromPowerLaw()
{
  return sampleFromPowerLaw(photonIndex_, energyMin_, energyMax_);
}

double BasicPrimaryGen::sampleFromPowerLaw(double gamma, double e0, double e1)
{
  using std::pow;

  double energy = 0.0;
  if ( 0.999 < gamma && gamma < 1.001 ) {
    // Photon index ~ 1
    energy = e0 * pow(e1/e0, G4UniformRand());
  }
  else {
    const double s = 1.0-gamma;
    const double a0 = pow(e0, s);
    const double a1 = pow(e1, s);
    const double a = a0 + G4UniformRand()*(a1-a0);
    energy = pow(a, 1./s);
  }

  return energy;
}

double BasicPrimaryGen::sampleFromGaussian()
{
  return sampleFromGaussian(energyMean_, energySigma_);
}

double BasicPrimaryGen::sampleFromGaussian(double mean, double sigma)
{
  const double x = CLHEP::RandGauss::shoot(CLHEP::HepRandom::getTheEngine());
  const double energy = mean + x * sigma;
  return energy;
}

double BasicPrimaryGen::sampleFromBlackBody()
{
  return sampleFromBlackBody(kT_, energyMax_/kT_);
}

double BasicPrimaryGen::sampleFromBlackBody(double kT,
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
  const double energy = kT * x;
  return energy;
}

void BasicPrimaryGen::buildSpectrumPhotonIntegral()
{
  const std::size_t NBins = spectrumPhotons_.size();
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
      % (spectrumEnergy_[i]/unit::keV) % spectrumPhotonIntegral_[i] << '\n';
  }
  std::cout << std::endl;
}

double BasicPrimaryGen::sampleFromHistogram()
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
  const double phi2 = CLHEP::twopi * G4UniformRand();
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

  primaryGenerator_->RegisterGeneratorSetting(this);

  return primaryGenerator_;
}

double BasicPrimaryGen::LengthUnit() const
{
  return unit::cm;
}

std::string BasicPrimaryGen::LengthUnitName() const
{
  return "cm";
}

} /* namespace anlgeant4 */
