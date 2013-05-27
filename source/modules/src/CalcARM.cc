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

#include "CalcARM.hh"

#include "TDirectory.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TMath.h"

#include "SaveData.hh"

using namespace comptonsoft;
using namespace anl;

CalcARM::CalcARM()
  : NumBin(500), RangeTheta1(-25.0), RangeTheta2(+25.0)
{
}


ANLStatus CalcARM::mod_startup()
{
  register_parameter(&NumBin, "Number of bins");
  register_parameter(&RangeTheta1, "Range min", 1.0, "degree");
  register_parameter(&RangeTheta2, "Range max", 1.0, "degree");
  
  return AS_OK;
}


ANLStatus CalcARM::mod_com()
{
  hide_parameter("Number of bins");
  hide_parameter("Range min");
  hide_parameter("Range max");
  
  return VCSModule::mod_com();
}


ANLStatus CalcARM::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();
  
  hist_arm_All = new TH1D("h_arm_All","ARM (All)", NumBin, RangeTheta1, RangeTheta2);
  hist_arm_All_nf = new TH1D("h_arm_All_nf","ARM (All, Fl Cut)", NumBin, RangeTheta1, RangeTheta2);

  GetANLModuleNC("EventReconstruction", &EventReconstruction_ptr);
  std::vector<HitPattern>& hitpat_vec = EventReconstruction_ptr->GetHitPatternVector();
  hist_vec.resize(hitpat_vec.size());
  hist_nf_vec.resize(hitpat_vec.size());

  for (unsigned int i=0; i<hitpat_vec.size(); i++) {
    std::string hist_name = "h_arm_";
    std::string hist_title = "ARM (";
    hist_name += hitpat_vec[i].ShortName();
    hist_title += hitpat_vec[i].Name();
    hist_title += ")";
    hist_vec[i] = new TH1D(hist_name.c_str(), hist_title.c_str(), NumBin, RangeTheta1, RangeTheta2);

    std::string hist_nf_name = "h_arm_";
    std::string hist_nf_title = "ARM (";
    hist_nf_name += hitpat_vec[i].ShortName();
    hist_nf_name += "_nf";
    hist_nf_title += hitpat_vec[i].Name();
    hist_nf_title += ", Fl Cut)";
    hist_nf_vec[i] = new TH1D(hist_nf_name.c_str(), hist_nf_title.c_str(), NumBin, RangeTheta1, RangeTheta2);
  }

  return AS_OK;
}


ANLStatus CalcARM::mod_ana()
{
  const TwoHitComptonEvent& compton_event = EventReconstruction_ptr->GetTwoHitData();

  double cosThetaE = compton_event.CosThetaE();
  if (cosThetaE < -1. || +1. < cosThetaE) {
    return AS_OK;
  }

  double arm = compton_event.DeltaTheta() * 180. / TMath::Pi();

  // Filling histograms 
  // All
  hist_arm_All->Fill(arm);
  if (!Evs("EventReconst:CdTeFluor")) { hist_arm_All_nf->Fill(arm); }

  for(unsigned int i=0; i<hist_vec.size(); i++) {
    if (EventReconstruction_ptr->HitPatternFlag(i)) {
      hist_vec[i]->Fill(arm);
      if(!Evs("EventReconst:CdTeFluor")) { hist_nf_vec[i]->Fill(arm); }
    }
  }

  return AS_OK;
}
