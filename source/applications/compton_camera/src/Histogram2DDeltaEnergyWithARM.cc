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

#include "Histogram2DDeltaEnergyWithARM.hh"
#include "TH2.h"
#include "AstroUnits.hh"
#include "EventReconstruction.hh"
#include "InitialInformation.hh"
#include "BasicComptonEvent.hh"
#include "FlagDefinition.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

Histogram2DDeltaEnergyWithARM::Histogram2DDeltaEnergyWithARM()
  : eventReconstruction_(nullptr),
    initialInfo_(nullptr),
    hist_all_(nullptr), hist_compton_all_(nullptr),
    num_energy_bins_(128), energy0_(-64.0), energy1_(+64.0),
    num_arm_bins_(72), arm0_(-180.0), arm1_(+180.0)
{
}

ANLStatus Histogram2DDeltaEnergyWithARM::mod_define()
{
  define_parameter("number_of_energy_bins", &mod_class::num_energy_bins_);
  define_parameter("energy_min", &mod_class::energy0_, 1, "keV");
  define_parameter("energy_max", &mod_class::energy1_,1, "keV");
  define_parameter("number_of_arm_bins", &mod_class::num_arm_bins_);
  define_parameter("arm_min", &mod_class::arm0_, 1, "degree");
  define_parameter("arm_max", &mod_class::arm1_, 1, "degree");

  return AS_OK;
}

ANLStatus Histogram2DDeltaEnergyWithARM::mod_initialize()
{
  get_module("EventReconstruction", &eventReconstruction_);
  get_module_IF("InitialInformation", &initialInfo_);

  VCSModule::mod_initialize();
  mkdir();

  hist_all_ = new TH2D("de_arm_all","ARM:DeltaEnergy (All)",
                       num_energy_bins_, energy0_, energy1_,
                       num_arm_bins_, arm0_, arm1_);
  hist_compton_all_ = new TH2D("de_arm_compton_all","ARM:DeltaEnergy (All, Compton)",
                               num_energy_bins_, energy0_, energy1_,
                               num_arm_bins_, arm0_, arm1_);

  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t numHitPatterns = hitPatterns.size();
  hist_vec_.resize(numHitPatterns);
  hist_compton_vec_.resize(numHitPatterns);
  for (std::size_t i=0; i<numHitPatterns; i++) {
    std::string histName = "de_arm_";
    std::string histTitle = "ARM:DeltaEnergy (";
    histName += hitPatterns[i].ShortName();
    histTitle += hitPatterns[i].Name();
    histTitle += ")";
    hist_vec_[i] = new TH2D(histName.c_str(), histTitle.c_str(),
                            num_energy_bins_, energy0_, energy1_,
                            num_arm_bins_, arm0_, arm1_);
    histName = "de_arm_compton_";
    histTitle = "ARM:DeltaEnergy (";
    histName += hitPatterns[i].ShortName();
    histTitle += hitPatterns[i].Name();
    histTitle += ", Compton)";
    hist_compton_vec_[i] = new TH2D(histName.c_str(), histTitle.c_str(),
                                    num_energy_bins_, energy0_, energy1_,
                                    num_arm_bins_, arm0_, arm1_);
  }

  return AS_OK;
}

ANLStatus Histogram2DDeltaEnergyWithARM::mod_analyze()
{
  if (!evs("EventReconstruction:OK")) {
    return AS_OK;
  }

  const std::vector<BasicComptonEvent_sptr> events = eventReconstruction_->getReconstructedEvents();
  for (const auto& event: events) {
    const double fraction = event->ReconstructionFraction();

    const double cosThetaE = event->CosThetaE();
    if (cosThetaE < -1.0 || +1.0 < cosThetaE) {
      return AS_OK;
    }

    const double energy = event->IncidentEnergy() / unit::keV;
    const double ini_energy = initialInfo_->initial_energy() / unit::keV;
    const double de = energy - ini_energy;
    const double arm = event->DeltaTheta() / unit::degree;
    const unsigned int hit1Process = event->Hit1Process();

    hist_all_->Fill(de, arm, fraction);
    for (std::size_t i=0; i<hist_vec_.size(); i++) {
      if (eventReconstruction_->HitPatternFlag(i)) {
        hist_vec_[i]->Fill(de, arm, fraction);
      }
    }

    if (hit1Process==process::ComptonScattering) {
      hist_compton_all_->Fill(de, arm, fraction);
      for (std::size_t i=0; i<hist_vec_.size(); i++) {
        if (eventReconstruction_->HitPatternFlag(i)) {
          hist_compton_vec_[i]->Fill(de, arm, fraction);
        }
      }
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
