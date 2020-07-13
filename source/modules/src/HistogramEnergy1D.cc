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

#include "HistogramEnergy1D.hh"
#include "TH1.h"
#include "AstroUnits.hh"
#include "EventReconstruction.hh"
#include "BasicComptonEvent.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

HistogramEnergy1D::HistogramEnergy1D()
  : eventReconstruction_(nullptr),
    hist_all_(nullptr),
    numBins_(720), energy0_(0.0), energy1_(720.0)
{
}

ANLStatus HistogramEnergy1D::mod_define()
{
  register_parameter(&numBins_, "number_of_bins");
  register_parameter(&energy0_, "energy_min", 1, "keV");
  register_parameter(&energy1_, "energy_max", 1, "keV");
  
  return AS_OK;
}

ANLStatus HistogramEnergy1D::mod_initialize()
{
  get_module("EventReconstruction", &eventReconstruction_);
  
  VCSModule::mod_initialize();
  mkdir();
  
  hist_all_ = new TH1D("energy1d_all","Energy1+Energy2 (All)",
                       numBins_, energy0_, energy1_);
  
  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t numHitPatterns = hitPatterns.size();
  hist_vec_.resize(numHitPatterns);
  for (std::size_t i=0; i<numHitPatterns; i++) {
    std::string histName = "energy1d_";
    std::string histTitle = "Energy1+Energy2 (";
    histName += hitPatterns[i].ShortName();
    histTitle += hitPatterns[i].Name();
    histTitle += ")";
    hist_vec_[i] = new TH1D(histName.c_str(), histTitle.c_str(),
                            numBins_, energy0_, energy1_);
  }

  return AS_OK;
}

ANLStatus HistogramEnergy1D::mod_analyze()
{
  if (!evs("EventReconstruction:OK")) {
    return AS_OK;
  }

  const std::vector<BasicComptonEvent_sptr> events = eventReconstruction_->getReconstructedEvents();
  for (const auto& event: events) {
    const double fraction = event->ReconstructionFraction();
    const double energy = event->IncidentEnergy() / unit::keV;
    
    hist_all_->Fill(energy, fraction);
    for (std::size_t i=0; i<hist_vec_.size(); i++) {
      if (eventReconstruction_->HitPatternFlag(i)) {
        hist_vec_[i]->Fill(energy, fraction);
      }
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
