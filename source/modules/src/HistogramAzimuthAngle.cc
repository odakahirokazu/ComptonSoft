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

#include "HistogramAzimuthAngle.hh"

#include <cmath>
#include "TH1.h"
#include "AstroUnits.hh"
#include "EventReconstruction.hh"
#include "BasicComptonEvent.hh"

using namespace anl;

namespace comptonsoft
{

HistogramAzimuthAngle::HistogramAzimuthAngle()
  : eventReconstruction_(nullptr),
    numBins_(64),
    theta_min_(-1.0*degree), theta_max_(181.0*degree),
    phi_origin_(0.0)
{
}

ANLStatus HistogramAzimuthAngle::mod_startup()
{
  register_parameter(&numBins_, "number_of_bins");
  register_parameter(&theta_min_, "theta_min", degree, "degree");
  register_parameter(&theta_max_, "theta_max", degree, "degree");
  register_parameter(&phi_origin_, "phi_origin", 1.0, "degree");
  
  return AS_OK;
}

ANLStatus HistogramAzimuthAngle::mod_his()
{
  GetANLModule("EventReconstruction", &eventReconstruction_);
  
  VCSModule::mod_his();
  mkdir();

  const double phi_min = -180.0;
  const double phi_max = +180.0;
  hist_all_ = new TH1D("phi_all","Azimuth angle (All)",
                       numBins_, phi_min, phi_max);
  
  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t numHitPatterns = hitPatterns.size();
  hist_vec_.resize(numHitPatterns);
  for (std::size_t i=0; i<numHitPatterns; i++) {
    std::string histName = "phi_";
    std::string histTitle = "Azimuth angle (";
    histName += hitPatterns[i].ShortName();
    histTitle += hitPatterns[i].Name();
    histTitle += ")";
    hist_vec_[i] = new TH1D(histName.c_str(), histTitle.c_str(),
                            numBins_, phi_min, phi_max);
  }

  return AS_OK;
}

ANLStatus HistogramAzimuthAngle::mod_ana()
{
  if (!Evs("EventReconstruction:OK")) {
    return AS_OK;
  }
  
  const BasicComptonEvent& event = eventReconstruction_->getComptonEvent();
  const double thetaE = event.ThetaE();

  if (thetaE < theta_min_) {
    return AS_OK;
  }

  if (thetaE > theta_max_) {
    return AS_OK;
  }

  const double phi0 = (event.PhiG()/degree) - phi_origin_;
  const int n_phi = std::floor((phi0+180.0)/360.0);
  const double phi1 = phi0 - n_phi*360.0;

  hist_all_->Fill(phi1);

  for (std::size_t i=0; i<hist_vec_.size(); i++) {
    if (eventReconstruction_->HitPatternFlag(i)) {
      hist_vec_[i]->Fill(phi1);
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
