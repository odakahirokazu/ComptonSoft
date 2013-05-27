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

// 2008-11-xx Hirokazu Odaka

#include "PhAbsModeSpectrum.hh"
#include "TDirectory.h"
#include "DetectorHit.hh"

using namespace comptonsoft;
using namespace anl;


PhAbsModeSpectrum::PhAbsModeSpectrum()
  : hit_collection(0), hist_phall(0), hist_1hit(0), hist_2hit(0),
    hist_3hit(0), hist_4hit(0), hist_mhit(0),
    NumBinEnergy(720), RangeEnergy1(0.0), RangeEnergy2(720.0), energy(0.0)
{
}


ANLStatus PhAbsModeSpectrum::mod_startup()
{
  register_parameter(&NumBinEnergy, "Number of bins");
  register_parameter(&RangeEnergy1, "Energy min", 1, "keV");
  register_parameter(&RangeEnergy2, "Energy max", 1, "keV");
  
  return AS_OK;
}


ANLStatus PhAbsModeSpectrum::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();

  hist_phall = new TH1D("h_spec_PhAll","Energy (Ph All)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_1hit = new TH1D("h_spec_1hit","Energy (1-hit)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_2hit = new TH1D("h_spec_2hit","Energy (2-hit)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_3hit = new TH1D("h_spec_3hit","Energy (3-hit)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_4hit = new TH1D("h_spec_4hit","Energy (4-hit)", NumBinEnergy, RangeEnergy1, RangeEnergy2);
  hist_mhit = new TH1D("h_spec_mhit","Energy (m-hit)", NumBinEnergy, RangeEnergy1, RangeEnergy2);

  return AS_OK;
}


ANLStatus PhAbsModeSpectrum::mod_init()
{
  GetANLModuleNC("CSHitCollection", &hit_collection);
  
  return AS_OK;
}


ANLStatus PhAbsModeSpectrum::mod_ana()
{
  int numTimeGroup = hit_collection->NumTimeGroup();
  int timeGroup = numTimeGroup - 1;
  const std::vector<DetectorHit_sptr>& hitVec
    = hit_collection->GetHits(timeGroup);

  energy = 0.0;
  const int num = hitVec.size();
  for (int i=0; i<num; i++) {
    energy += hitVec[i]->getPI();
  }

  hist_phall->Fill(energy/keV);

  if (Evs("EventReconst:1Hit")) {
    hist_1hit->Fill(energy/keV);
  }

  if (Evs("EventReconst:2Hit")) {
    hist_2hit->Fill(energy/keV);
  }

  if (Evs("EventReconst:3Hit")) {
    hist_3hit->Fill(energy/keV);
  }

  if (Evs("EventReconst:4Hit")) {
    hist_4hit->Fill(energy/keV);
  }

  if (Evs("EventReconst:ManyHit")) {
    hist_mhit->Fill(energy/keV);
  }

  return AS_OK;
}
