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

#include "CalcARM_PS.hh"

#include "TDirectory.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TwoHitComptonEvent.hh"

using namespace comptonsoft;
using namespace anl;

CalcARM_PS::CalcARM_PS()
  : SourcePos(0.0, 0.0, 0.0)
{
}


ANLStatus CalcARM_PS::mod_startup()
{
  register_parameter(&SourcePos, "Source position", cm, "cm");
  
  return AS_OK;
}


ANLStatus CalcARM_PS::mod_ana()
{
  ComptonEvent& compton_event = EventReconstruction_ptr->GetComptonEvent();
  compton_event.setSourcePosition(SourcePos.X(),
				  SourcePos.Y(),
				  SourcePos.Z());
  double cosThetaE = compton_event.CosThetaE();

  if (cosThetaE < -1. || +1. < cosThetaE) {
    return AS_OK;
  }

  double arm = compton_event.DeltaTheta() * 180. / TMath::Pi();

  // Filling histograms 
  // All
  hist_arm_All->Fill(arm);
  if (!Evs("EventReconst:CdTeFluor")) { hist_arm_All_nf->Fill(arm); }

  for(size_t i=0; i<hist_vec.size(); i++) {
    if(EventReconstruction_ptr->HitPatternFlag(i)) {
      hist_vec[i]->Fill(arm);
      if(!Evs("EventReconst:CdTeFluor")) { hist_nf_vec[i]->Fill(arm); }
    }
  }

  return AS_OK;
}



