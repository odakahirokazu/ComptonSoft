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

#include "CalcARMEne.hh"

#include "TDirectory.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TwoHitComptonEvent.hh"

#include "NextCLI.hh"

using namespace comptonsoft;
using namespace anl;

ANLStatus CalcARMEne::mod_com()
{
  CalcARM::mod_com();

  std::string name("DSSD");

  std::cout << "Definition of noise level (FWHM) : " << std::endl;
  while (1) {
    CLread("Detector name prefix (OK for exit) :", &name);
    if (name == "OK" or name == "ok") { break; }
    
    double noiseParam0 = 1.0; // keV
    CLread("Noise level Param 0", &noiseParam0, 1.0, "keV");
    double noiseParam1 = 0.0;
    CLread("Noise level Param 1", &noiseParam1);
    double noiseParam2 = 0.008;
    CLread("Noise level Param 2", &noiseParam2);
      
    noiselevel_map.insert(std::make_pair(name, boost::make_tuple(noiseParam0, noiseParam1, noiseParam2)));
    
    name = "OK";
  }
  
  return AS_OK;
}


ANLStatus CalcARMEne::mod_ana()
{
  const TwoHitComptonEvent& compton_event = EventReconstruction_ptr->GetTwoHitData();
  double cosThetaE = compton_event.CosThetaE();

  if (cosThetaE < -1. || +1. < cosThetaE) {
    return AS_OK;
  }

  const double h1edep = compton_event.getH1Energy();
  const double h2edep = compton_event.getH2Energy();
  const int h1detid = compton_event.getH1DetId();
  const int h2detid = compton_event.getH2DetId();

  const RealDetectorUnit* h1det = GetDetectorManager()->getDetectorByID(h1detid);
  const std::string h1detName = h1det->getNamePrefix();
  const double h1eres = calculateNoiseFWHM(h1detName, h1edep) / 2.354;

  const RealDetectorUnit* h2det = GetDetectorManager()->getDetectorByID(h2detid);
  const std::string h2detName = h2det->getNamePrefix();
  const double h2eres = calculateNoiseFWHM(h2detName, h2edep) / 2.354;

  double thetaE = TMath::ACos(cosThetaE) * 180. / TMath::Pi();
  
  const double me = electron_mass_c2;

  const int TIMES = 1000;
  const double FillWeight = 1./(double)TIMES;
  static TRandom3 randgen;
  
  for (int t=0; t<TIMES; t++) {
    double h1edep1 = randgen.Gaus(h1edep, h1eres);
    double h2edep1 = randgen.Gaus(h2edep, h2eres);
    
    double cosThetaE1 = 1. - me/h2edep1 + me/(h1edep1+h2edep1);
    if (cosThetaE1 < -1. || +1. < cosThetaE1) {
      return AS_OK;
      continue;
    }
    double thetaE1 = TMath::ACos(cosThetaE1) * 180. / TMath::Pi();
    double arm = thetaE - thetaE1;

    // Filling histograms 
    // All
    hist_arm_All->Fill(arm, FillWeight);
    if (!Evs("EventReconst:CdTeFluor")) { hist_arm_All_nf->Fill(arm, FillWeight); }
    
    for(size_t i=0; i<hist_vec.size(); i++) {
      if(EventReconstruction_ptr->HitPatternFlag(i)) {
	hist_vec[i]->Fill(arm, FillWeight);
	if(!Evs("EventReconst:CdTeFluor")) { hist_nf_vec[i]->Fill(arm, FillWeight); }
      }
    }
  }

  return AS_OK;
}


double CalcARMEne::calculateNoiseFWHM(const std::string& name, double edep)
{
  if (noiselevel_map.count(name) == 0) {
    std::cout << "No detector name prefix is found in Noise level map. "
	      << name << std::endl;
    return 0.0;
  }

  boost::tuple<double, double, double> noise = noiselevel_map[name];
  double noise0 = noise.get<0>();
  double noise1 = noise.get<1>();
  double noise2 = noise.get<2>();
  
  double ene = edep/keV; // keV
  double noiseA1 = noise0; // keV
  double noiseB2 = noise1 * noise1 * ene; // keV2
  double noiseC1 = noise2 * ene; // keV
  
  double fwhm2 = noiseA1*noiseA1 + noiseB2 + noiseC1*noiseC1; // keV
  double sigma = TMath::Sqrt(fwhm2)/2.3548 * keV;

  return sigma;
}
