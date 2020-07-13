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
#include <cmath>
#include "TGraph.h"
#include "AstroUnits.hh"
#include "BasicComptonEvent.hh"
#include "EventReconstruction.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

ComptonModeFilter::ComptonModeFilter()
  : m_EventReconstruction(nullptr),
    m_ARMCutCurveSS(nullptr), m_ARMCutCurveSC(nullptr), m_ARMCutCurveCC(nullptr)
{
}

ANLStatus ComptonModeFilter::mod_initialize()
{
  VCSModule::mod_initialize();

  define_evs("ComptonMode:Exception");
  define_evs("ComptonMode:DeltaTheta:02");
  define_evs("ComptonMode:DeltaTheta:04");
  define_evs("ComptonMode:DeltaTheta:06");
  define_evs("ComptonMode:DeltaTheta:08");
  define_evs("ComptonMode:DeltaTheta:12");
  define_evs("ComptonMode:DeltaTheta:16");
  define_evs("ComptonMode:DeltaTheta:24");
  define_evs("ComptonMode:DeltaTheta:GOOD");
  define_evs("ComptonMode:DeltaTheta:GOOD2");

  get_module_NC("EventReconstruction", &m_EventReconstruction);
  
  mkdir();
  create_armcut_curves();

  return AS_OK;
}

ANLStatus ComptonModeFilter::mod_analyze()
{
  if (!evs("EventReconstruction:OK")) {
    return AS_OK;
  }

  if (m_EventReconstruction->NumberOfReconstructedEvents()!=1) {
    set_evs("ComptonMode:Exception");
    return AS_OK;
  }
  
  const BasicComptonEvent& ComptonEvent
    = *(m_EventReconstruction->getReconstructedEvents()[0]);
  const double dtheta = std::abs(ComptonEvent.DeltaTheta());
  const double e_gamma = ComptonEvent.IncidentEnergy();

  if (dtheta < 2.0*unit::degree) {
    set_evs("ComptonMode:DeltaTheta:02");
  }

  if (dtheta < 4.0*unit::degree) {
    set_evs("ComptonMode:DeltaTheta:04");
  }

  if (dtheta < 6.0*unit::degree) {
    set_evs("ComptonMode:DeltaTheta:06");
  }

  if (dtheta < 8.0*unit::degree) {
    set_evs("ComptonMode:DeltaTheta:08");
  }

  if (dtheta < 12.0*unit::degree) {
    set_evs("ComptonMode:DeltaTheta:12");
  }

  if (dtheta < 16.0*unit::degree) {
    set_evs("ComptonMode:DeltaTheta:16");
  }

  if (dtheta < 24.0*unit::degree) {
    set_evs("ComptonMode:DeltaTheta:24");
  }

  if (dtheta < theta_cut_limit(e_gamma)) {
    set_evs("ComptonMode:DeltaTheta:GOOD");
  }
  
  if (dtheta < theta_cut_limit2(e_gamma)) {
    set_evs("ComptonMode:DeltaTheta:GOOD2");
  }
  
  return AS_OK;
}

// function formula taken from HXISGDComptonMode_Ana.cc by S.Watanabe
double ComptonModeFilter::theta_cut_limit(double energy) const
{
  if( energy > 300. *unit::keV) energy = 300. * unit::keV;
  double limit = (26.0 - energy/unit::keV * 0.06) * unit::degree;
  return limit;
}

// function formula taken from MakeAnaSpectrum.cc by S.Watanabe
double ComptonModeFilter::theta_cut_limit2(double energy) const
{
  double armcut = 180.0*unit::degree;
  if (evs("HitPattern:SS")) {
    armcut = m_ARMCutCurveSS->Eval(energy/unit::keV,0,"S") * unit::degree;
  }
  else if (evs("HitPattern:SC")) {
    armcut = m_ARMCutCurveSC->Eval(energy/unit::keV,0,"S") * unit::degree;
  }
  else if (evs("HitPattern:CC")) {
    armcut = m_ARMCutCurveCC->Eval(energy/unit::keV,0,"S") * unit::degree;
  }

  return armcut;
}

void ComptonModeFilter::create_armcut_curves()
{
  const int graph_point = 15;
  double energy_ss[graph_point]={0., 40., 50., 60., 70., 80., 90.,100.,120.,140.,160.,200.,600.,800.,1000.};
  double energy_sc[graph_point]={0., 60., 80.,100.,120.,150.,180.,220.,250.,300.,350.,400.,500.,600.,1000.};
  double energy_cc[graph_point]={0.,100.,150.,200.,250.,300.,350.,400.,450.,500.,600.,700.,800.,900.,1000.};

  double armlimit_ss[graph_point]={62.37,62.37,62.37,49.95,45.63,44.19,44.01,42.93,42.93,42.93,42.93,42.93,42.93,42.93,42.93};
  double armlimit_sc[graph_point]={29.97,29.97,20.07,16.29,13.95,12.51,11.79,10.53, 9.81, 9.09, 9.09, 8.91, 6.39, 5.13, 5.13};
  double armlimit_cc[graph_point]={48.15,48.15,48.15,47.97,42.39,32.49,26.19,21.15,19.17,16.65,13.23,13.23,13.23,13.23,13.23};
  
  m_ARMCutCurveSS = new TGraph(graph_point, energy_ss, armlimit_ss);
  m_ARMCutCurveSC = new TGraph(graph_point, energy_sc, armlimit_sc);
  m_ARMCutCurveCC = new TGraph(graph_point, energy_cc, armlimit_cc);
  
  m_ARMCutCurveSS->SetName("armcut_curve_ss");
  m_ARMCutCurveSC->SetName("armcut_curve_sc");
  m_ARMCutCurveCC->SetName("armcut_curve_cc");
  
  m_ARMCutCurveSS->Write();
  m_ARMCutCurveSC->Write();
  m_ARMCutCurveCC->Write();
}

} /* namespace comptonsoft */
