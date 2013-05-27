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

#include "CalcARMPos.hh"

#include "TDirectory.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TMath.h"
#include "cs_globals.hh"
#include "TwoHitComptonEvent.hh"

using namespace comptonsoft;


using namespace anl;

CalcARMPos::CalcARMPos()
  : SourceDirection(0.0, 0.0, 1.0)
{
}


ANLStatus CalcARMPos::mod_startup()
{
  CalcARM::mod_startup();
  register_parameter(&SourceDirection, "Source direction");
  
  return AS_OK;
}


ANLStatus CalcARMPos::mod_ana()
{
  const TwoHitComptonEvent& compton_event = EventReconstruction_ptr->GetTwoHitData();

  //const vector3_t coneVertex = compton_event->ConeVertex();
  //const vector3_t SourceDirection(0.0, 0.0, 1.0);
  const vector3_t coneAxis = compton_event.ConeAxis();
  const double h1posx = compton_event.getH1PosX();
  const double h1posy = compton_event.getH1PosY();
  const double h1posz = compton_event.getH1PosZ();
  const vector3_t h1pos(h1posx, h1posy, h1posz);

  const double h2posx = compton_event.getH2PosX();
  const double h2posy = compton_event.getH2PosY();
  const double h2posz = compton_event.getH2PosZ();
  const vector3_t h2pos(h2posx, h2posy, h2posz);

  const int h1detid = compton_event.getH1DetId();
  const int h2detid = compton_event.getH2DetId();

  const RealDetectorUnit* h1det = GetDetectorManager()->getDetectorByID(h1detid);

  const double h1PositionResolutionX = h1det->getPixelPitchX() * 0.5;
  const double h1PositionResolutionY = h1det->getPixelPitchY() * 0.5;
  const double h1PositionResolutionZ = h1det->getThickness() * 0.5;
  const vector3_t h1DetDirX = h1det->getDirectionX();
  const vector3_t h1DetDirY = h1det->getDirectionY();
  const vector3_t h1DetDirZ = h1DetDirX.cross(h1DetDirY);

  const RealDetectorUnit* h2det = GetDetectorManager()->getDetectorByID(h2detid);

  const double h2PositionResolutionX = h2det->getPixelPitchX() * 0.5;
  const double h2PositionResolutionY = h2det->getPixelPitchY() * 0.5;
  const double h2PositionResolutionZ = h2det->getThickness() * 0.5;
  const vector3_t h2DetDirX = h2det->getDirectionX();
  const vector3_t h2DetDirY = h2det->getDirectionY();
  const vector3_t h2DetDirZ = h2DetDirX.cross(h2DetDirY);
  
  double cosThetaE = compton_event.CosThetaE();
  if (cosThetaE < -1. || +1. < cosThetaE) {
    return AS_OK;
  }

  double thetaG = SourceDirection.angle(coneAxis) * 180. / TMath::Pi();

  const int TIMES = 1000;
  const double FillWeight = 1./(double)TIMES;
  static TRandom3 randgen;

  for (int t=0; t<TIMES; t++) {
    double h1DeltaLocalX = randgen.Uniform(-h1PositionResolutionX, +h1PositionResolutionX);
    double h1DeltaLocalY = randgen.Uniform(-h1PositionResolutionY, +h1PositionResolutionY);
    double h1DeltaLocalZ = randgen.Uniform(-h1PositionResolutionZ, +h1PositionResolutionZ);
    double h2DeltaLocalX = randgen.Uniform(-h2PositionResolutionX, +h2PositionResolutionX);
    double h2DeltaLocalY = randgen.Uniform(-h2PositionResolutionY, +h2PositionResolutionY);
    double h2DeltaLocalZ = randgen.Uniform(-h2PositionResolutionZ, +h2PositionResolutionZ);

    vector3_t h1pos1 = h1pos + h1DeltaLocalX*h1DetDirX + h1DeltaLocalY*h1DetDirY + h1DeltaLocalZ*h1DetDirZ;
    vector3_t h2pos1 = h2pos + h2DeltaLocalX*h2DetDirX + h2DeltaLocalY*h2DetDirY + h2DeltaLocalZ*h2DetDirZ;

    vector3_t coneAxis1 = h1pos1 - h2pos1;
    double thetaG1 = SourceDirection.angle(coneAxis1) * 180. / TMath::Pi();
    double arm = thetaG1 - thetaG;

    // Filling histograms 
    // All
    hist_arm_All->Fill(arm, FillWeight);
    if (!Evs("EventReconst:CdTeFluor")) { hist_arm_All_nf->Fill(arm, FillWeight); }
    
    for(unsigned int i=0; i<hist_vec.size(); i++) {
      if(EventReconstruction_ptr->HitPatternFlag(i)) {
	hist_vec[i]->Fill(arm, FillWeight);
	if(!Evs("EventReconst:CdTeFluor")) { hist_nf_vec[i]->Fill(arm, FillWeight); }
      }
    }
  }
  
  return AS_OK;
}
