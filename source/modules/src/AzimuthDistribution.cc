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

#include "AzimuthDistribution.hh"

#include "TDirectory.h"

#include "TRandom3.h"
#include "TMath.h"
#include "TwoHitComptonEvent.hh"

using namespace comptonsoft;


using namespace anl;

AzimuthDistribution::AzimuthDistribution()
  : EventReconstruction_ptr(0),
    NumBin(72), RangePhi1(-180.0), RangePhi2(+180.0),
    ThetaLD(-1.0), ThetaUD(181.0)
{
}


ANLStatus AzimuthDistribution::mod_startup()
{
  register_parameter(&NumBin, "number_of_bins");
  register_parameter(&RangePhi1, "phi_min", 1.0, "degree");
  register_parameter(&RangePhi2, "phi_max", 1.0, "degree");
  register_parameter(&ThetaLD, "theta_LD", 1.0, "degree");
  register_parameter(&ThetaUD, "theta_UD", 1.0, "degree");
  
  return AS_OK;
}


ANLStatus AzimuthDistribution::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();

  hist_phi_All = new TH1D("h_phi_All","PHI DISTRIBUTION (All)", NumBin, RangePhi1, RangePhi2);
  hist_phi_All_nf = new TH1D("h_phi_All_nf","PHI DISTRIBUTION (All, Fl Cut)", NumBin, RangePhi1, RangePhi2);

  GetANLModuleNC("EventReconstruction", &EventReconstruction_ptr);
  std::vector<HitPattern>& hitpat_vec = EventReconstruction_ptr->GetHitPatternVector();
  hist_vec.resize(hitpat_vec.size());
  hist_nf_vec.resize(hitpat_vec.size());
  
  for (size_t i=0; i<hitpat_vec.size(); i++) {
    std::string hist_name = "h_phi_";
    std::string hist_title = "Azimuth Distribution (";
    hist_name += hitpat_vec[i].ShortName();
    hist_title += hitpat_vec[i].Name();
    hist_title += ")";
    hist_vec[i] = new TH1D(hist_name.c_str(), hist_title.c_str(), NumBin, RangePhi1, RangePhi2);

    std::string hist_nf_name = "h_phi_";
    std::string hist_nf_title = "Azimuth Distribution (";
    hist_nf_name += hitpat_vec[i].ShortName();
    hist_nf_name += "_nf";
    hist_nf_title += hitpat_vec[i].Name();
    hist_nf_title += ", Fl Cut)";
    hist_nf_vec[i] = new TH1D(hist_nf_name.c_str(), hist_nf_title.c_str(), NumBin, RangePhi1, RangePhi2);
  }

  return AS_OK;
}


ANLStatus AzimuthDistribution::mod_ana()
{
  const TwoHitComptonEvent& compton_event = EventReconstruction_ptr->GetTwoHitData();
  double azimuth = (-compton_event.ConeAxis()).phi() * 180.0 / TMath::Pi();

  double theta = TMath::ACos(compton_event.CosThetaE())*180.0 / TMath::Pi();

  if(ThetaLD>=0.0) {
    if(ThetaLD>theta) {
      return AS_OK;
    }
  }

  if(ThetaUD<=180.0) {
    if(ThetaUD<theta) {
      return AS_OK;
    }
  }

  // Filling histograms
  // All
  hist_phi_All->Fill(azimuth);
  if (!Evs("EventReconst:CdTeFluor")) { hist_phi_All_nf->Fill(azimuth); }

  for(size_t i=0; i<hist_vec.size(); i++) {
    if(EventReconstruction_ptr->HitPatternFlag(i)) {
      hist_vec[i]->Fill(azimuth);
      if(!Evs("EventReconst:CdTeFluor")) { hist_nf_vec[i]->Fill(azimuth); }
    }
  }

  return AS_OK;
}
