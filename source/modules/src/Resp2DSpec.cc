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

#include "Resp2DSpec.hh"
#include "TDirectory.h"

using namespace comptonsoft;
using namespace anl;


Resp2DSpec::Resp2DSpec()
  : EventReconstruction_ptr(0), initial_info(0),
    NumBinEnergy(720.0), RangeEnergy1(0.0), RangeEnergy2(720.0)
{
}


ANLStatus Resp2DSpec::mod_startup()
{
  register_parameter(&NumBinEnergy, "Number of bins");
  register_parameter(&RangeEnergy1, "Energy min", 1, "keV");
  register_parameter(&RangeEnergy2, "Energy max", 1, "keV");
  
  return AS_OK;
}


ANLStatus Resp2DSpec::mod_init()
{
  GetANLModuleIFNC("InitialInformation", &initial_info);
  return AS_OK;
}


ANLStatus Resp2DSpec::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();

  if (ModuleExist("PhAbsModeSpectrum")) {
    GetANLModuleNC("PhAbsModeSpectrum", &phabs_mode);
    hist_phabs = new TH2F("h_resp_phabs","ini_Energy:tot_Energy (Phabs)", NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
  }

  if (ModuleExist("EventReconstruction")) {
    hist_all = new TH2F("h_resp_All","ini_Energy:tot_Energy (All)", NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
    hist_all_nf = new TH2F("h_resp_All_nf","ini_Energy:tot_Energy (All, Fl Cut)", NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);

    GetANLModuleNC("EventReconstruction", &EventReconstruction_ptr);
    std::vector<HitPattern>& hitpat_vec = EventReconstruction_ptr->GetHitPatternVector();
    hist_dt04_vec.resize(hitpat_vec.size());
    hist_dt04_nf_vec.resize(hitpat_vec.size());
    hist_dt08_vec.resize(hitpat_vec.size());
    hist_dt08_nf_vec.resize(hitpat_vec.size());
    hist_dt12_vec.resize(hitpat_vec.size());
    hist_dt12_nf_vec.resize(hitpat_vec.size());
    hist_dt16_vec.resize(hitpat_vec.size());
    hist_dt16_nf_vec.resize(hitpat_vec.size());
    hist_dt24_vec.resize(hitpat_vec.size());
    hist_dt24_nf_vec.resize(hitpat_vec.size());
    hist_good_vec.resize(hitpat_vec.size());
    hist_good_nf_vec.resize(hitpat_vec.size());
    hist_good2_vec.resize(hitpat_vec.size());
    hist_good2_nf_vec.resize(hitpat_vec.size());
    
    for (unsigned int i=0; i<hitpat_vec.size(); i++) {
      std::string hist_dt04_name = "h_resp_";
      std::string hist_dt04_title = "ini_Energy:tot_Energy (";
      hist_dt04_name += hitpat_vec[i].ShortName();
      hist_dt04_name += "_dt04";
      hist_dt04_title += hitpat_vec[i].Name();
      hist_dt04_title += ", DTheta<4deg)";
      hist_dt04_vec[i] = new TH2F(hist_dt04_name.c_str(), hist_dt04_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_dt08_name = "h_resp_";
      std::string hist_dt08_title = "ini_Energy:tot_Energy (";
      hist_dt08_name += hitpat_vec[i].ShortName();
      hist_dt08_name += "_dt08";
      hist_dt08_title += hitpat_vec[i].Name();
      hist_dt08_title += ", DTheta<8deg)";
      hist_dt08_vec[i] = new TH2F(hist_dt08_name.c_str(), hist_dt08_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_dt12_name = "h_resp_";
      std::string hist_dt12_title = "ini_Energy:tot_Energy (";
      hist_dt12_name += hitpat_vec[i].ShortName();
      hist_dt12_name += "_dt12";
      hist_dt12_title += hitpat_vec[i].Name();
      hist_dt12_title += ", DTheta<12deg)";
      hist_dt12_vec[i] = new TH2F(hist_dt12_name.c_str(), hist_dt12_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_dt16_name = "h_resp_";
      std::string hist_dt16_title = "ini_Energy:tot_Energy (";
      hist_dt16_name += hitpat_vec[i].ShortName();
      hist_dt16_name += "_dt16";
      hist_dt16_title += hitpat_vec[i].Name();
      hist_dt16_title += ", DTheta<16deg)";
      hist_dt16_vec[i] = new TH2F(hist_dt16_name.c_str(), hist_dt16_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_dt24_name = "h_resp_";
      std::string hist_dt24_title = "ini_Energy:tot_Energy (";
      hist_dt24_name += hitpat_vec[i].ShortName();
      hist_dt24_name += "_dt24";
      hist_dt24_title += hitpat_vec[i].Name();
      hist_dt24_title += ", DTheta<24deg)";
      hist_dt24_vec[i] = new TH2F(hist_dt24_name.c_str(), hist_dt24_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_good_name = "h_resp_";
      std::string hist_good_title = "ini_Energy:tot_Energy (";
      hist_good_name += hitpat_vec[i].ShortName();
      hist_good_name += "_good";
      hist_good_title += hitpat_vec[i].Name();
      hist_good_title += ", DTheta<limit(Ene))";
      hist_good_vec[i] = new TH2F(hist_good_name.c_str(), hist_good_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_good2_name = "h_resp_";
      std::string hist_good2_title = "ini_Energy:tot_Energy (";
      hist_good2_name += hitpat_vec[i].ShortName();
      hist_good2_name += "_good2";
      hist_good2_title += hitpat_vec[i].Name();
      hist_good2_title += ", DTheta<limit2(Ene))";
      hist_good2_vec[i] = new TH2F(hist_good2_name.c_str(), hist_good2_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);

      std::string hist_dt04_nf_name = "h_resp_";
      std::string hist_dt04_nf_title = "ini_Energy:tot_Energy (";
      hist_dt04_nf_name += hitpat_vec[i].ShortName();
      hist_dt04_nf_name += "_nf";
      hist_dt04_nf_name += "_dt04";
      hist_dt04_nf_title += hitpat_vec[i].Name();
      hist_dt04_nf_title += ", DTheta<4deg, Fl Cut)";
      hist_dt04_nf_vec[i] = new TH2F(hist_dt04_nf_name.c_str(), hist_dt04_nf_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_dt08_nf_name = "h_resp_";
      std::string hist_dt08_nf_title = "ini_Energy:tot_Energy (";
      hist_dt08_nf_name += hitpat_vec[i].ShortName();
      hist_dt08_nf_name += "_nf";
      hist_dt08_nf_name += "_dt08";
      hist_dt08_nf_title += hitpat_vec[i].Name();
      hist_dt08_nf_title += ", DTheta<8deg, Fl Cut)";
      hist_dt08_nf_vec[i] = new TH2F(hist_dt08_nf_name.c_str(), hist_dt08_nf_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_dt12_nf_name = "h_resp_";
      std::string hist_dt12_nf_title = "ini_Energy:tot_Energy (";
      hist_dt12_nf_name += hitpat_vec[i].ShortName();
      hist_dt12_nf_name += "_nf";
      hist_dt12_nf_name += "_dt12";
      hist_dt12_nf_title += hitpat_vec[i].Name();
      hist_dt12_nf_title += ", DTheta<12deg, Fl Cut)";
      hist_dt12_nf_vec[i] = new TH2F(hist_dt12_nf_name.c_str(), hist_dt12_nf_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_dt16_nf_name = "h_resp_";
      std::string hist_dt16_nf_title = "ini_Energy:tot_Energy (";
      hist_dt16_nf_name += hitpat_vec[i].ShortName();
      hist_dt16_nf_name += "_nf";
      hist_dt16_nf_name += "_dt16";
      hist_dt16_nf_title += hitpat_vec[i].Name();
      hist_dt16_nf_title += ", DTheta<16deg, Fl Cut)";
      hist_dt16_nf_vec[i] = new TH2F(hist_dt16_nf_name.c_str(), hist_dt16_nf_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_dt24_nf_name = "h_resp_";
      std::string hist_dt24_nf_title = "ini_Energy:tot_Energy (";
      hist_dt24_nf_name += hitpat_vec[i].ShortName();
      hist_dt24_nf_name += "_nf";
      hist_dt24_nf_name += "_dt24";
      hist_dt24_nf_title += hitpat_vec[i].Name();
      hist_dt24_nf_title += ", DTheta<24deg, Fl Cut)";
      hist_dt24_nf_vec[i] = new TH2F(hist_dt24_nf_name.c_str(), hist_dt24_nf_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_good_nf_name = "h_resp_";
      std::string hist_good_nf_title = "ini_Energy:tot_Energy (";
      hist_good_nf_name += hitpat_vec[i].ShortName();
      hist_good_nf_name += "_nf";
      hist_good_nf_name += "_good";
      hist_good_nf_title += hitpat_vec[i].Name();
      hist_good_nf_title += ", DTheta<limit(Ene), Fl Cut)";
      hist_good_nf_vec[i] = new TH2F(hist_good_nf_name.c_str(), hist_good_nf_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
      std::string hist_good2_nf_name = "h_resp_";
      std::string hist_good2_nf_title = "ini_Energy:tot_Energy (";
      hist_good2_nf_name += hitpat_vec[i].ShortName();
      hist_good2_nf_name += "_nf";
      hist_good2_nf_name += "_good2";
      hist_good2_nf_title += hitpat_vec[i].Name();
      hist_good2_nf_title += ", DTheta<limit2(Ene), Fl Cut)";
      hist_good2_nf_vec[i] = new TH2F(hist_good2_nf_name.c_str(), hist_good2_nf_title.c_str(), NumBinEnergy, RangeEnergy1, RangeEnergy2, NumBinEnergy, RangeEnergy1, RangeEnergy2);
      
    }
  }

  return AS_OK;
}


ANLStatus Resp2DSpec::mod_ana()
{
  double ini_ene = initial_info->InitialEnergy();
  if (hist_phabs) {
    hist_phabs->Fill(ini_ene/keV, phabs_mode->Energy()/keV);
  }
  
  if (hist_all==0 || !Evs("EventReconst:Stored")) {
    return AS_OK;
  }
  
  const TwoHitComptonEvent& twoHitData = EventReconstruction_ptr->GetTwoHitData();
  double energy = twoHitData.TotalEnergy();
  
  // Filling histograms 
  hist_all->Fill(ini_ene/keV, energy/keV);
  if (!Evs("EventReconst:CdTeFluor")) { hist_all_nf->Fill(ini_ene/keV, energy/keV); }

  for (unsigned int i=0; i<hist_good_vec.size(); i++) {
    if (EventReconstruction_ptr->HitPatternFlag(i)) {

      if (Evs("ComptonMode:DeltaTheta:GOOD")){
	hist_good_vec[i]->Fill(ini_ene/keV, energy/keV);
	if (!Evs("EventReconst:CdTeFluor")) { hist_good_nf_vec[i]->Fill(ini_ene/keV, energy/keV); }
      }

      if (Evs("ComptonMode:DeltaTheta:GOOD2")){
	hist_good2_vec[i]->Fill(ini_ene/keV, energy/keV);
	if (!Evs("EventReconst:CdTeFluor")) { hist_good2_nf_vec[i]->Fill(ini_ene/keV, energy/keV); }
      }

      if (Evs("ComptonMode:DeltaTheta:04")){
	hist_dt04_vec[i]->Fill(ini_ene/keV, energy/keV);
	if (!Evs("EventReconst:CdTeFluor")) { hist_dt04_nf_vec[i]->Fill(ini_ene/keV, energy/keV); }
      }

      if (Evs("ComptonMode:DeltaTheta:08")){
	hist_dt08_vec[i]->Fill(ini_ene/keV, energy/keV);
	if (!Evs("EventReconst:CdTeFluor")) { hist_dt08_nf_vec[i]->Fill(ini_ene/keV, energy/keV); }
      }

      if (Evs("ComptonMode:DeltaTheta:12")){
	hist_dt12_vec[i]->Fill(ini_ene/keV, energy/keV);
	if (!Evs("EventReconst:CdTeFluor")) { hist_dt12_nf_vec[i]->Fill(ini_ene/keV, energy/keV); }
      }

      if (Evs("ComptonMode:DeltaTheta:16")){
	hist_dt16_vec[i]->Fill(ini_ene/keV, energy/keV);
	if (!Evs("EventReconst:CdTeFluor")) { hist_dt16_nf_vec[i]->Fill(ini_ene/keV, energy/keV); }
      }

      if (Evs("ComptonMode:DeltaTheta:24")){
	hist_dt24_vec[i]->Fill(ini_ene/keV, energy/keV);
	if (!Evs("EventReconst:CdTeFluor")) { hist_dt24_nf_vec[i]->Fill(ini_ene/keV, energy/keV); }
      }

    }
  }

  return AS_OK;
}
