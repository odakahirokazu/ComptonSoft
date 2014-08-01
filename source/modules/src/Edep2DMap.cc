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

#include "Edep2DMap.hh"

using namespace comptonsoft;
using namespace anl;


Edep2DMap::Edep2DMap()
  : EventReconstruction_ptr(0),
    NumBinEnergy(720), RangeEnergy1(0.0), RangeEnergy2(720.0)
{
}


ANLStatus Edep2DMap::mod_startup()
{
  register_parameter(&NumBinEnergy, "number_of_bins");
  register_parameter(&RangeEnergy1, "energy_min", 1, "keV");
  register_parameter(&RangeEnergy2, "energy_max", 1, "keV");
  
  return AS_OK;
}


ANLStatus Edep2DMap::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();

  hist_all = new TH2D("h_e2d_All","Energy1:Energy2 (All)", NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
  
  hist_all_nf = new TH2D("h_e2d_All_nf","Energy1:Energy2 (All, Fl Cut)", NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
  
  GetANLModuleNC("EventReconstruction", &EventReconstruction_ptr);
  std::vector<HitPattern>& hitpat_vec = EventReconstruction_ptr->GetHitPatternVector();
  hist_vec.resize(hitpat_vec.size());
  hist_nf_vec.resize(hitpat_vec.size());
  
  for (unsigned int i=0; i<hitpat_vec.size(); i++) {
    std::string hist_name = "h_e2d_";
    std::string hist_title = "Energy1:Energy2 (";
    hist_name += hitpat_vec[i].ShortName();
    hist_title += hitpat_vec[i].Name();
    hist_title += ")";
    hist_vec[i] = new TH2D(hist_name.c_str(), hist_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_name = "h_e2d_";
    std::string hist_nf_title = "Energy1:Energy2 (";
    hist_nf_name += hitpat_vec[i].ShortName();
    hist_nf_name += "_nf";
    hist_nf_title += hitpat_vec[i].Name();
    hist_nf_title += ", Fl Cut)";
    hist_nf_vec[i] = new TH2D(hist_nf_name.c_str(), hist_nf_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
  }

  return AS_OK;
}


ANLStatus Edep2DMap::mod_ana()
{
  if (!Evs("EventReconst:Stored")) {
    return AS_OK;
  }
  
  const TwoHitComptonEvent& twoHitData = EventReconstruction_ptr->GetTwoHitData();
  double e1 = twoHitData.getH1Energy() / keV;
  double e2 = twoHitData.getH2Energy() / keV;

  // Filling histograms 
  hist_all->Fill(e2, e1);
  if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf->Fill(e2, e1); }

  for (unsigned int i=0; i<hist_vec.size(); i++) {
    if (EventReconstruction_ptr->HitPatternFlag(i)) {
      hist_vec[i]->Fill(e2, e1);
      if (!Evs("EventReconst:CdTeFluor")) { hist_nf_vec[i]->Fill(e2, e1); }
    }
  }

  return AS_OK;
}


