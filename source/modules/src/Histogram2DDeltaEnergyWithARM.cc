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

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

Histogram2DDeltaEnergyWithARM::Histogram2DDeltaEnergyWithARM()
  : eventReconstruction_(nullptr),
    initialInfo_(nullptr),
    hist_all_(nullptr),
    numEnergyBins_(128), energy0_(-64.0), energy1_(+64.0),
    numARMBins_(72), arm0_(-180.0), arm1_(+180.0)
{
}

ANLStatus Histogram2DDeltaEnergyWithARM::mod_define()
{
  register_parameter(&numEnergyBins_, "number_of_energy_bins");
  register_parameter(&energy0_, "energy_min", 1, "keV");
  register_parameter(&energy1_, "energy_max", 1, "keV");
  register_parameter(&numARMBins_, "number_of_arm_bins");
  register_parameter(&arm0_, "arm_min", 1, "degree");
  register_parameter(&arm1_, "arm_max", 1, "degree");
  
  return AS_OK;
}

ANLStatus Histogram2DDeltaEnergyWithARM::mod_initialize()
{
  get_module("EventReconstruction", &eventReconstruction_);
  get_module_IF("InitialInformation", &initialInfo_);
  
  VCSModule::mod_initialize();
  mkdir();
  
  hist_all_ = new TH2D("de_arm_all","ARM:DeltaEnergy (All)",
                       numEnergyBins_, energy0_, energy1_,
                       numARMBins_, arm0_, arm1_);

  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t numHitPatterns = hitPatterns.size();
  hist_vec_.resize(numHitPatterns);
  for (std::size_t i=0; i<numHitPatterns; i++) {
    std::string histName = "de_arm_";
    std::string histTitle = "ARM:DeltaEnergy (";
    histName += hitPatterns[i].ShortName();
    histTitle += hitPatterns[i].Name();
    histTitle += ")";
    hist_vec_[i] = new TH2D(histName.c_str(), histTitle.c_str(),
                            numEnergyBins_, energy0_, energy1_,
                            numARMBins_, arm0_, arm1_);
  }

  return AS_OK;
}

ANLStatus Histogram2DDeltaEnergyWithARM::mod_analyze()
{
  if (!evs("EventReconstruction:OK")) {
    return AS_OK;
  }
  
  const BasicComptonEvent& event = eventReconstruction_->getComptonEvent();
  const double energy = event.TotalEnergy() / unit::keV;
  const double ini_energy = initialInfo_->InitialEnergy() / unit::keV;
  const double de = energy - ini_energy;
  const double arm = event.DeltaTheta() / unit::degree;

  hist_all_->Fill(de, arm);
  for (std::size_t i=0; i<hist_vec_.size(); i++) {
    if (eventReconstruction_->HitPatternFlag(i)) {
      hist_vec_[i]->Fill(de, arm);
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
