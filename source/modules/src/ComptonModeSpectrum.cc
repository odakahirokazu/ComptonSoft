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

#include "ComptonModeSpectrum.hh"

using namespace comptonsoft;
using namespace anl;


ComptonModeSpectrum::ComptonModeSpectrum()
  : EventReconstruction_ptr(0),
    NumBinEnergy(720), RangeEnergy1(0.0), RangeEnergy2(720.0)
{
}


ANLStatus ComptonModeSpectrum::mod_startup()
{
  register_parameter(&NumBinEnergy, "number_of_bins");
  register_parameter(&RangeEnergy1, "energy_min", 1, "keV");
  register_parameter(&RangeEnergy2, "energy_max", 1, "keV");
  
  return AS_OK;
}


ANLStatus ComptonModeSpectrum::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();

  hist_all = new TH1D("h_spec_CoAll","Energy (All)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf = new TH1D("h_spec_CoAll_nf","Energy (All, Fl Cut)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_dt02 = new TH1D("h_spec_CoAll_dt02","Energy (All, DTheta<2degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf_dt02 = new TH1D("h_spec_CoAll_nf_dt02","Energy (All, Fl Cut, DTheta<2degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_dt04 = new TH1D("h_spec_CoAll_dt04","Energy (All, DTheta<4degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf_dt04 = new TH1D("h_spec_CoAll_nf_dt04","Energy (All, Fl Cut, DTheta<4degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_dt06 = new TH1D("h_spec_CoAll_dt06","Energy (All, DTheta<6degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf_dt06 = new TH1D("h_spec_CoAll_nf_dt06","Energy (All, Fl Cut, DTheta<6degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_dt08 = new TH1D("h_spec_CoAll_dt08","Energy (All, DTheta<8degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf_dt08 = new TH1D("h_spec_CoAll_nf_dt08","Energy (All, Fl Cut, DTheta<8degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_dt12 = new TH1D("h_spec_CoAll_dt12","Energy (All, DTheta<12degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf_dt12 = new TH1D("h_spec_CoAll_nf_dt12","Energy (All, Fl Cut, DTheta<12degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_dt16 = new TH1D("h_spec_CoAll_dt16","Energy (All, DTheta<16degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf_dt16 = new TH1D("h_spec_CoAll_nf_dt16","Energy (All, Fl Cut, DTheta<16degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_dt24 = new TH1D("h_spec_CoAll_dt24","Energy (All, DTheta<24degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf_dt24 = new TH1D("h_spec_CoAll_nf_dt24","Energy (All, Fl Cut, DTheta<24degree)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_good = new TH1D("h_spec_CoAll_good","Energy (All, DTheta<lim(E))", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf_good = new TH1D("h_spec_CoAll_nf_good","Energy (All, Fl Cut, DTheta<lim(E))", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_good2 = new TH1D("h_spec_CoAll_good2","Energy (All, DTheta<lim2(E))", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_all_nf_good2 = new TH1D("h_spec_CoAll_nf_good2","Energy (All, Fl Cut, DTheta<lim2(E))", NumBinEnergy, RangeEnergy1, RangeEnergy2);

  GetANLModuleNC("EventReconstruction", &EventReconstruction_ptr);
  std::vector<HitPattern>& hitpat_vec = EventReconstruction_ptr->GetHitPatternVector();
  int hist_vec_size = hitpat_vec.size();
  hist_vec.resize(hist_vec_size);
  hist_nf_vec.resize(hist_vec_size);
  hist_dt02_vec.resize(hist_vec_size);
  hist_nf_dt02_vec.resize(hist_vec_size);
  hist_dt04_vec.resize(hist_vec_size);
  hist_nf_dt04_vec.resize(hist_vec_size);
  hist_dt06_vec.resize(hist_vec_size);
  hist_nf_dt06_vec.resize(hist_vec_size);
  hist_dt08_vec.resize(hist_vec_size);
  hist_nf_dt08_vec.resize(hist_vec_size);
  hist_dt12_vec.resize(hist_vec_size);
  hist_nf_dt12_vec.resize(hist_vec_size);
  hist_dt16_vec.resize(hist_vec_size);
  hist_nf_dt16_vec.resize(hist_vec_size);
  hist_dt24_vec.resize(hist_vec_size);
  hist_nf_dt24_vec.resize(hist_vec_size);
  hist_good_vec.resize(hist_vec_size);
  hist_nf_good_vec.resize(hist_vec_size);
  hist_good2_vec.resize(hist_vec_size);
  hist_nf_good2_vec.resize(hist_vec_size);
  
  for (int i=0; i<hist_vec_size; i++) {
    std::string hist_name = "h_spec_";
    std::string hist_title = "Energy (";
    hist_name += hitpat_vec[i].ShortName();
    hist_title += hitpat_vec[i].Name();
    hist_title += ")";
    hist_vec[i] = new TH1D(hist_name.c_str(), hist_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_name = "h_spec_";
    std::string hist_nf_title = "Energy (";
    hist_nf_name += hitpat_vec[i].ShortName();
    hist_nf_name += "_nf";
    hist_nf_title += hitpat_vec[i].Name();
    hist_nf_title += ", Fl Cut)";
    hist_nf_vec[i] = new TH1D(hist_nf_name.c_str(), hist_nf_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_dt02_name = "h_spec_";
    std::string hist_dt02_title = "Energy (";
    hist_dt02_name += hitpat_vec[i].ShortName();
    hist_dt02_name += "_dt02";
    hist_dt02_title += hitpat_vec[i].Name();
    hist_dt02_title += ", DTheta<2degree)";
    hist_dt02_title += ")";
    hist_dt02_vec[i] = new TH1D(hist_dt02_name.c_str(), hist_dt02_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_dt02_name = "h_spec_";
    std::string hist_nf_dt02_title = "Energy (";
    hist_nf_dt02_name += hitpat_vec[i].ShortName();
    hist_nf_dt02_name += "_nf";
    hist_nf_dt02_name += "_dt02";
    hist_nf_dt02_title += hitpat_vec[i].Name();
    hist_nf_dt02_title += ", Fl Cut)";
    hist_nf_dt02_title += ", DTheta<2degree)";
    hist_nf_dt02_vec[i] = new TH1D(hist_nf_dt02_name.c_str(), hist_nf_dt02_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2); 

    std::string hist_dt04_name = "h_spec_";
    std::string hist_dt04_title = "Energy (";
    hist_dt04_name += hitpat_vec[i].ShortName();
    hist_dt04_name += "_dt04";
    hist_dt04_title += hitpat_vec[i].Name();
    hist_dt04_title += ", DTheta<4degree)";
    hist_dt04_title += ")";
    hist_dt04_vec[i] = new TH1D(hist_dt04_name.c_str(), hist_dt04_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_dt04_name = "h_spec_";
    std::string hist_nf_dt04_title = "Energy (";
    hist_nf_dt04_name += hitpat_vec[i].ShortName();
    hist_nf_dt04_name += "_nf";
    hist_nf_dt04_name += "_dt04";
    hist_nf_dt04_title += hitpat_vec[i].Name();
    hist_nf_dt04_title += ", Fl Cut)";
    hist_nf_dt04_title += ", DTheta<4degree)";
    hist_nf_dt04_vec[i] = new TH1D(hist_nf_dt04_name.c_str(), hist_nf_dt04_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2); 

    std::string hist_dt06_name = "h_spec_";
    std::string hist_dt06_title = "Energy (";
    hist_dt06_name += hitpat_vec[i].ShortName();
    hist_dt06_name += "_dt06";
    hist_dt06_title += hitpat_vec[i].Name();
    hist_dt06_title += ", DTheta<6degree)";
    hist_dt06_title += ")";
    hist_dt06_vec[i] = new TH1D(hist_dt06_name.c_str(), hist_dt06_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_dt06_name = "h_spec_";
    std::string hist_nf_dt06_title = "Energy (";
    hist_nf_dt06_name += hitpat_vec[i].ShortName();
    hist_nf_dt06_name += "_nf";
    hist_nf_dt06_name += "_dt06";
    hist_nf_dt06_title += hitpat_vec[i].Name();
    hist_nf_dt06_title += ", Fl Cut)";
    hist_nf_dt06_title += ", DTheta<6degree)";
    hist_nf_dt06_vec[i] = new TH1D(hist_nf_dt06_name.c_str(), hist_nf_dt06_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2); 

    std::string hist_dt08_name = "h_spec_";
    std::string hist_dt08_title = "Energy (";
    hist_dt08_name += hitpat_vec[i].ShortName();
    hist_dt08_name += "_dt08";
    hist_dt08_title += hitpat_vec[i].Name();
    hist_dt08_title += ", DTheta<8degree)";
    hist_dt08_title += ")";
    hist_dt08_vec[i] = new TH1D(hist_dt08_name.c_str(), hist_dt08_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_dt08_name = "h_spec_";
    std::string hist_nf_dt08_title = "Energy (";
    hist_nf_dt08_name += hitpat_vec[i].ShortName();
    hist_nf_dt08_name += "_nf";
    hist_nf_dt08_name += "_dt08";
    hist_nf_dt08_title += hitpat_vec[i].Name();
    hist_nf_dt08_title += ", Fl Cut)";
    hist_nf_dt08_title += ", DTheta<8degree)";
    hist_nf_dt08_vec[i] = new TH1D(hist_nf_dt08_name.c_str(), hist_nf_dt08_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2); 

    std::string hist_dt12_name = "h_spec_";
    std::string hist_dt12_title = "Energy (";
    hist_dt12_name += hitpat_vec[i].ShortName();
    hist_dt12_name += "_dt12";
    hist_dt12_title += hitpat_vec[i].Name();
    hist_dt12_title += ", DTheta<12degree)";
    hist_dt12_title += ")";
    hist_dt12_vec[i] = new TH1D(hist_dt12_name.c_str(), hist_dt12_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_dt12_name = "h_spec_";
    std::string hist_nf_dt12_title = "Energy (";
    hist_nf_dt12_name += hitpat_vec[i].ShortName();
    hist_nf_dt12_name += "_nf";
    hist_nf_dt12_name += "_dt12";
    hist_nf_dt12_title += hitpat_vec[i].Name();
    hist_nf_dt12_title += ", Fl Cut)";
    hist_nf_dt12_title += ", DTheta<12degree)";
    hist_nf_dt12_vec[i] = new TH1D(hist_nf_dt12_name.c_str(), hist_nf_dt12_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2); 

    std::string hist_dt16_name = "h_spec_";
    std::string hist_dt16_title = "Energy (";
    hist_dt16_name += hitpat_vec[i].ShortName();
    hist_dt16_name += "_dt16";
    hist_dt16_title += hitpat_vec[i].Name();
    hist_dt16_title += ", DTheta<16degree)";
    hist_dt16_title += ")";
    hist_dt16_vec[i] = new TH1D(hist_dt16_name.c_str(), hist_dt16_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_dt16_name = "h_spec_";
    std::string hist_nf_dt16_title = "Energy (";
    hist_nf_dt16_name += hitpat_vec[i].ShortName();
    hist_nf_dt16_name += "_nf";
    hist_nf_dt16_name += "_dt16";
    hist_nf_dt16_title += hitpat_vec[i].Name();
    hist_nf_dt16_title += ", Fl Cut)";
    hist_nf_dt16_title += ", DTheta<16degree)";
    hist_nf_dt16_vec[i] = new TH1D(hist_nf_dt16_name.c_str(), hist_nf_dt16_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2); 

    std::string hist_dt24_name = "h_spec_";
    std::string hist_dt24_title = "Energy (";
    hist_dt24_name += hitpat_vec[i].ShortName();
    hist_dt24_name += "_dt24";
    hist_dt24_title += hitpat_vec[i].Name();
    hist_dt24_title += ", DTheta<24degree)";
    hist_dt24_title += ")";
    hist_dt24_vec[i] = new TH1D(hist_dt24_name.c_str(), hist_dt24_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_dt24_name = "h_spec_";
    std::string hist_nf_dt24_title = "Energy (";
    hist_nf_dt24_name += hitpat_vec[i].ShortName();
    hist_nf_dt24_name += "_nf";
    hist_nf_dt24_name += "_dt24";
    hist_nf_dt24_title += hitpat_vec[i].Name();
    hist_nf_dt24_title += ", Fl Cut)";
    hist_nf_dt24_title += ", DTheta<24degree)";
    hist_nf_dt24_vec[i] = new TH1D(hist_nf_dt24_name.c_str(), hist_nf_dt24_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_good_name = "h_spec_";
    std::string hist_good_title = "Energy (";
    hist_good_name += hitpat_vec[i].ShortName();
    hist_good_name += "_good";
    hist_good_title += hitpat_vec[i].Name();
    hist_good_title += ", DTheta<Lim(E))";
    hist_good_title += ")";
    hist_good_vec[i] = new TH1D(hist_good_name.c_str(), hist_good_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_good_name = "h_spec_";
    std::string hist_nf_good_title = "Energy (";
    hist_nf_good_name += hitpat_vec[i].ShortName();
    hist_nf_good_name += "_nf";
    hist_nf_good_name += "_good";
    hist_nf_good_title += hitpat_vec[i].Name();
    hist_nf_good_title += ", Fl Cut)";
    hist_nf_good_title += ", DTheta<Lim(E))";
    hist_nf_good_vec[i] = new TH1D(hist_nf_good_name.c_str(), hist_nf_good_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2); 

    std::string hist_good2_name = "h_spec_";
    std::string hist_good2_title = "Energy (";
    hist_good2_name += hitpat_vec[i].ShortName();
    hist_good2_name += "_good2";
    hist_good2_title += hitpat_vec[i].Name();
    hist_good2_title += ", DTheta<Lim2(E))";
    hist_good2_title += ")";
    hist_good2_vec[i] = new TH1D(hist_good2_name.c_str(), hist_good2_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2);

    std::string hist_nf_good2_name = "h_spec_";
    std::string hist_nf_good2_title = "Energy (";
    hist_nf_good2_name += hitpat_vec[i].ShortName();
    hist_nf_good2_name += "_nf";
    hist_nf_good2_name += "_good2";
    hist_nf_good2_title += hitpat_vec[i].Name();
    hist_nf_good2_title += ", Fl Cut)";
    hist_nf_good2_title += ", DTheta<Lim2(E))";
    hist_nf_good2_vec[i] = new TH1D(hist_nf_good2_name.c_str(), hist_nf_good2_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2); 
  }

  return AS_OK;
}


ANLStatus ComptonModeSpectrum::mod_ana()
{
  if (!Evs("EventReconst:Stored")) {
    return AS_SKIP;
  }
  
  const TwoHitComptonEvent& twoHitData = EventReconstruction_ptr->GetTwoHitData();
  double energy = twoHitData.TotalEnergy() / keV;

  // Filling histograms 
  hist_all->Fill(energy);
  if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf->Fill(energy); }
  
  if (Evs("ComptonMode:DeltaTheta:02")) {
    hist_all_dt02->Fill(energy);
    if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf_dt02->Fill(energy); }
  }

  if (Evs("ComptonMode:DeltaTheta:04")) {
    hist_all_dt04->Fill(energy);
    if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf_dt04->Fill(energy); }
  }

  if (Evs("ComptonMode:DeltaTheta:06")) {
    hist_all_dt06->Fill(energy);
    if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf_dt06->Fill(energy); }
  }

  if (Evs("ComptonMode:DeltaTheta:08")) {
    hist_all_dt08->Fill(energy);
    if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf_dt08->Fill(energy); }
  }
  
  if (Evs("ComptonMode:DeltaTheta:12")) {
    hist_all_dt12->Fill(energy);
    if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf_dt12->Fill(energy); }
  }

  if (Evs("ComptonMode:DeltaTheta:16")) {
    hist_all_dt16->Fill(energy);
    if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf_dt16->Fill(energy); }
  }

  if (Evs("ComptonMode:DeltaTheta:24")) {
    hist_all_dt24->Fill(energy);
    if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf_dt24->Fill(energy); }
  }

  if (Evs("ComptonMode:DeltaTheta:GOOD")) {
    hist_all_good->Fill(energy);
    if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf_good->Fill(energy); }
  }

  if (Evs("ComptonMode:DeltaTheta:GOOD2")) {
    hist_all_good2->Fill(energy);
    if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf_good2->Fill(energy); }
  }

  int hist_vec_size = hist_vec.size();
  for (int i=0; i<hist_vec_size; i++) {
    if (EventReconstruction_ptr->HitPatternFlag(i)) {
      hist_vec[i]->Fill(energy);
      if (!Evs("EventReconst:CdTeFluor")) { hist_nf_vec[i]->Fill(energy); }

      if (Evs("ComptonMode:DeltaTheta:02")) {
	hist_dt02_vec[i]->Fill(energy);
	if (!Evs("EventReconst:CdTeFluor")) { hist_nf_dt02_vec[i]->Fill(energy); }
      }
 
      if (Evs("ComptonMode:DeltaTheta:04")) {
	hist_dt04_vec[i]->Fill(energy);
	if (!Evs("EventReconst:CdTeFluor")) { hist_nf_dt04_vec[i]->Fill(energy); }
      }

      if (Evs("ComptonMode:DeltaTheta:06")) {
	hist_dt06_vec[i]->Fill(energy);
	if (!Evs("EventReconst:CdTeFluor")) { hist_nf_dt06_vec[i]->Fill(energy); }
      }

      if (Evs("ComptonMode:DeltaTheta:08")) {
	hist_dt08_vec[i]->Fill(energy);
	if (!Evs("EventReconst:CdTeFluor")) { hist_nf_dt08_vec[i]->Fill(energy); }
      }

      if (Evs("ComptonMode:DeltaTheta:12")) {
	hist_dt12_vec[i]->Fill(energy);
	if (!Evs("EventReconst:CdTeFluor")) { hist_nf_dt12_vec[i]->Fill(energy); }
      }
 
      if (Evs("ComptonMode:DeltaTheta:16")) {
	hist_dt16_vec[i]->Fill(energy);
	if (!Evs("EventReconst:CdTeFluor")) { hist_nf_dt16_vec[i]->Fill(energy); }
      }
  
      if (Evs("ComptonMode:DeltaTheta:24")) {
	hist_dt24_vec[i]->Fill(energy);
	if (!Evs("EventReconst:CdTeFluor")) { hist_nf_dt24_vec[i]->Fill(energy); }
      }

      if (Evs("ComptonMode:DeltaTheta:GOOD")) {
	hist_good_vec[i]->Fill(energy);
	if (!Evs("EventReconst:CdTeFluor")) { hist_nf_good_vec[i]->Fill(energy); }
      }

      if (Evs("ComptonMode:DeltaTheta:GOOD2")) {
	hist_good2_vec[i]->Fill(energy);
	if (!Evs("EventReconst:CdTeFluor")) { hist_nf_good2_vec[i]->Fill(energy); }
      }
    }
  }

  return AS_OK;
}
