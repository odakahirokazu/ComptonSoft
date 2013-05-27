/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka, Shin Watanabe                      *
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

#include "ComptonModeFilter.hh"
#include "TDirectory.h"

#include "globals.hh"
#include "TMath.h"

using namespace comptonsoft;
using namespace anl;

ComptonModeFilter::ComptonModeFilter()
  : EventReconstruction_ptr(0),
    armcut_curve_ss(0), armcut_curve_sc(0), armcut_curve_cc(0)
{
}


ANLStatus ComptonModeFilter::mod_init()
{
  EvsDef("ComptonMode:DeltaTheta:02");
  EvsDef("ComptonMode:DeltaTheta:04");
  EvsDef("ComptonMode:DeltaTheta:06");
  EvsDef("ComptonMode:DeltaTheta:08");
  EvsDef("ComptonMode:DeltaTheta:12");
  EvsDef("ComptonMode:DeltaTheta:16");
  EvsDef("ComptonMode:DeltaTheta:24");
  EvsDef("ComptonMode:DeltaTheta:GOOD");
  EvsDef("ComptonMode:DeltaTheta:GOOD2");

  GetANLModuleNC("EventReconstruction", &EventReconstruction_ptr);
  
  return AS_OK;
}


ANLStatus ComptonModeFilter::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();
  create_armcut_curves();

  return AS_OK;
}


ANLStatus ComptonModeFilter::mod_ana()
{
  if (!Evs("EventReconst:Stored")) {
    return AS_OK;
  }
  
  const TwoHitComptonEvent& twoHitData = EventReconstruction_ptr->GetTwoHitData();
  const double dtheta = TMath::Abs(twoHitData.DeltaTheta());
  const double e_gamma = twoHitData.TotalEnergy();

  if (dtheta < 2.0*degree) {
    EvsSet("ComptonMode:DeltaTheta:02");
  }

  if (dtheta < 4.0*degree) {
    EvsSet("ComptonMode:DeltaTheta:04");
  }

  if (dtheta < 6.0*degree) {
    EvsSet("ComptonMode:DeltaTheta:06");
  }

  if (dtheta < 8.0*degree) {
    EvsSet("ComptonMode:DeltaTheta:08");
  }

  if (dtheta < 12.0*degree) {
    EvsSet("ComptonMode:DeltaTheta:12");
  }

  if (dtheta < 16.0*degree) {
    EvsSet("ComptonMode:DeltaTheta:16");
  }

  if (dtheta < 24.0*degree) {
    EvsSet("ComptonMode:DeltaTheta:24");
  }

  if (dtheta < theta_cut_limit(e_gamma)) {
    EvsSet("ComptonMode:DeltaTheta:GOOD");
  }
  
  if (dtheta < theta_cut_limit2(e_gamma)) {
    EvsSet("ComptonMode:DeltaTheta:GOOD2");
  }
  
  return AS_OK;
}


// function formula taken from HXISGDComptonMode_Ana.cc by S.Watanabe
double ComptonModeFilter::theta_cut_limit(double ene) const
{
  if( ene > 300. *keV) ene = 300. * keV;
  double limit = (26.0 - ene/keV * 0.06) * deg;
  return limit;
}

// function formula taken from MakeAnaSpectrum.cc by S.Watanabe
double ComptonModeFilter::theta_cut_limit2(double ene) const
{
  double armcut = 180.0*deg;
  if (Evs("EventReconst:2Hit:Si-Si")) {
    armcut = armcut_curve_ss->Eval(ene/keV,0,"S") * deg;
  }
  else if (Evs("EventReconst:2Hit:Si-CdTe")) {
    armcut = armcut_curve_sc->Eval(ene/keV,0,"S") * deg;
  }
  else if (Evs("EventReconst:2Hit:CdTe-CdTe")) {
    armcut = armcut_curve_cc->Eval(ene/keV,0,"S") * deg;
  }
  else if (Evs("EventReconst:3Hit")) {
    armcut = armcut_curve_sc->Eval(ene/keV,0,"S") * deg;
  }

  return armcut;
}


void ComptonModeFilter::create_armcut_curves()
{
  const int graph_point = 15;
  double ene_ss[graph_point]={0., 40., 50., 60., 70., 80., 90.,100.,120.,140.,160.,200.,600.,800.,1000.};
  double ene_sc[graph_point]={0., 60., 80.,100.,120.,150.,180.,220.,250.,300.,350.,400.,500.,600.,1000.};
  double ene_cc[graph_point]={0.,100.,150.,200.,250.,300.,350.,400.,450.,500.,600.,700.,800.,900.,1000.};

  double armlimit_ss[graph_point]={62.37,62.37,62.37,49.95,45.63,44.19,44.01,42.93,42.93,42.93,42.93,42.93,42.93,42.93,42.93};
  double armlimit_sc[graph_point]={29.97,29.97,20.07,16.29,13.95,12.51,11.79,10.53, 9.81, 9.09, 9.09, 8.91, 6.39, 5.13, 5.13};
  double armlimit_cc[graph_point]={48.15,48.15,48.15,47.97,42.39,32.49,26.19,21.15,19.17,16.65,13.23,13.23,13.23,13.23,13.23};
  
  armcut_curve_ss = new TGraph(graph_point, ene_ss, armlimit_ss);
  armcut_curve_sc = new TGraph(graph_point, ene_sc, armlimit_sc);
  armcut_curve_cc = new TGraph(graph_point, ene_cc, armlimit_cc);
  
  armcut_curve_ss->SetName("armcut_curve_ss");
  armcut_curve_sc->SetName("armcut_curve_sc");
  armcut_curve_cc->SetName("armcut_curve_cc");
  
  armcut_curve_ss->Write();
  armcut_curve_sc->Write();
  armcut_curve_cc->Write();
}
