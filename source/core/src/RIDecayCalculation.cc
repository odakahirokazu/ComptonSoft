/**
 * @file RIDecayCalculation.cc
 * @brief source file of class RIDecayCalculation
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#include "RIDecayCalculation.hh"

#include <iostream>

#include "CLHEP/Units/SystemOfUnits.h"
#include "G4RunManager.hh"
#include "Shielding.hh"
#include "DummyDetectorConstruction.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4Ions.hh"
#include "G4RadioactiveDecay.hh"

#include "TimeProfile.hh"
#include "RateData.hh"
#include "RIDecayProperties.hh"
#include "RIDecayChains.hh"

namespace comptonsoft
{

RIDecayCalculation::RIDecayCalculation()
  : verbose_level_(0),
    filename_production_rate_("production_rate.dat"),
    filename_irradiation_("irradiation.dat"),
    average_mode_(false),
    measurement_time_(0.0),
    count_threshold_(0.0),
    decay_constant_threshold_(0.0),
    branching_ratio_threshold_(1.0e-6),
    filename_decay_rate_("decay_rate.dat"),
    geant4_run_manager_(new G4RunManager),
    irradiation_profile_(new TimeProfile),
    production_rate_data_(new RateData),
    decay_rate_data_(new RateData)
{
}

RIDecayCalculation::~RIDecayCalculation() = default;

void RIDecayCalculation::setMeasurementTime(double t)
{
  average_mode_ = false;
  measurement_time_ = t*CLHEP::second;
}

double RIDecayCalculation::MeasurememtTime() const
{
  return measurement_time_/CLHEP::second;
}

void RIDecayCalculation::addMeasurementWindow(double t1, double t2)
{
  average_mode_ = true;
  measurement_windows_.emplace_back(t1*CLHEP::second, t2*CLHEP::second);
}

double RIDecayCalculation::getStartingTimeOfWindow(std::size_t i) const
{
  return measurement_windows_[i].first/CLHEP::second;
}

double RIDecayCalculation::getEndingTimeOfWindow(std::size_t i) const
{
  return measurement_windows_[i].second/CLHEP::second;
}

void RIDecayCalculation::setDecayConstantThreshold(double v)
{
  decay_constant_threshold_ = v * (1.0/CLHEP::second);
}

double RIDecayCalculation::DecayConstantThreshold() const
{
  return decay_constant_threshold_/(1.0/CLHEP::second);
}

bool RIDecayCalculation::initialize()
{
  if (verbose_level_ > 0) {
    std::cout << '\n'
              << "RIDecayCalculation: Initialization started.\n" << std::endl;
  }

  initialize_geant4();

  if (verbose_level_ > 0) {
    std::cout << '\n'
              << "Loading irradiation file: " << filename_irradiation_
              << std::endl;
  }
  if (!irradiation_profile_->readFile(filename_irradiation_)) {
    return false;
  }

  if (verbose_level_ > 0) {
    std::cout << '\n'
              << "Loading production rate file: " << filename_production_rate_
              << std::endl;
  }
  production_rate_data_->setCountThreshold(count_threshold_);
  if (!production_rate_data_->readFile(filename_production_rate_)) {
    return false;
  }

  return true;
}

void RIDecayCalculation::initialize_geant4()
{
  geant4_run_manager_->SetUserInitialization(new comptonsoft::DummyDetectorConstruction);
  geant4_run_manager_->SetUserInitialization(new Shielding);
  geant4_run_manager_->Initialize();

#if 0 /* probably not necessary */
  geant4_run_manager_->BeamOn(0);
#endif
}

void RIDecayCalculation::perform()
{
  G4RadioactiveDecay* decayProcess = new G4RadioactiveDecay;

  const std::size_t NumberOfVolumes
    = production_rate_data_->NumberOfVolumes();

  for (std::size_t iVolume=0; iVolume<NumberOfVolumes; iVolume++) {
    const std::string volumeName = production_rate_data_->getVolumeName(iVolume);
    const RateVector rateVector = production_rate_data_->getRateVector(iVolume);

    if (verbose_level_ > 0) {
      std::cout << "\n################################\n"
                << "Decay calculation for \n"
                << "Volume[" << iVolume << "] " << volumeName
                << std::endl;
    }

    std::map<int64_t, double> accumulation;

    for (const IsotopeInfo& isotope: rateVector) {
      const double productionEfficiency = isotope.Rate();
      std::unique_ptr<RIDecayChains> chains(new RIDecayChains(isotope));
      chains->setVerboseLevel(verbose_level_);
      chains->setDecayConstantThreshold(decay_constant_threshold_);
      chains->setBranchingRatioThreshold(branching_ratio_threshold_);
      chains->setDecayProcess(decayProcess);
      chains->build();
      chains->prepareSolutions();
      if (!average_mode_) {
        chains->solve(*irradiation_profile_, measurement_time_);
      }
      else {
        chains->solve(*irradiation_profile_, measurement_windows_);
      }

      const std::map<int64_t, double>& results = chains->getResults();
      for (auto& pair: results) {
        const int64_t isotopeID = pair.first;
        const double solution = pair.second;
        const double isotopeAmount = productionEfficiency * solution;
        if (accumulation.count(isotopeID)==0) {
          accumulation[isotopeID] = isotopeAmount;
        }
        else {
          accumulation[isotopeID] += isotopeAmount;
        }
      }
    }

    RateVector decayRateVector = makeDecayRateVector(accumulation);
    decay_rate_data_->pushData(volumeName, decayRateVector);
  }
}

RateVector RIDecayCalculation::
makeDecayRateVector(const std::map<int64_t, double>& accumulation)
{
  RateVector rateVector;
  for (auto& pair: accumulation) {
    const int64_t isotopeID = pair.first;
    const double isotopeAmount = pair.second;
    IsotopeInfo isotope(isotopeID);
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    if (isotope.Z() == 0) {
      // to avoid an error by G4IonTable::GetIon() with Z=0, A=1
      continue;
    }

    const G4ParticleDefinition* nucleus
      = particleTable->GetIonTable()->GetIon(isotope.Z(),
                                             isotope.A(),
                                             isotope.Energy(),
                                             G4Ions::FloatLevelBase(isotope.FloatingLevel()));
    const double lifetime = nucleus->GetPDGLifeTime();
    const double decayConstant = invert_lifetime(lifetime);
    const double decayRate = decayConstant * isotopeAmount;
    isotope.setRate(decayRate);
    rateVector.push_back(isotope);
  }
  return rateVector;
}

void RIDecayCalculation::output()
{
  if (verbose_level_ > 0) {
    std::cout << '\n'
              << "RIDecayCalculation: Saving results to "
              << filename_decay_rate_
              << ".\n" << std::endl;
  }

  decay_rate_data_->writeFile(filename_decay_rate_);
}

} /* namespace comptonsoft */
