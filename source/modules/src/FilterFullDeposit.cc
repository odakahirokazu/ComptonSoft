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

// 2008-12-12 Hirokazu Odaka
// 2011-04-26 Hirokazu Odaka

#include "FilterFullDeposit.hh"
#include "TMath.h"
#include "DetectorHit.hh"
#include "InitialInformation.hh"


using namespace comptonsoft;
using namespace anl;


ANLStatus FilterFullDeposit::mod_init()
{
  EvsDef("PhAbsMode:FullDeposit");

  GetANLModuleNC("CSHitCollection", &hit_collection);
  GetANLModuleIF("InitialInformation", &initial_info);
  
  return AS_OK;
}


ANLStatus FilterFullDeposit::mod_ana()
{
  int numTimeGroup = hit_collection->NumTimeGroup();
  int timeGroup = numTimeGroup - 1;
  const std::vector<DetectorHit_sptr>& hitVec
    = hit_collection->GetHits(timeGroup);

  double energy = 0.0;
  const int num = hitVec.size();
  for (int i=0; i<num; i++) {
    energy += hitVec[i]->getPI();
  }

  // BnkPut("PhAbsMode:TotalEnergy", sizeof(double), &energy);
  
  const double FULL_DEPOSIT_THRESHOLD = 0.03;
  double in_energy = 0.0;
  //  BnkGet("PrimaryGen:Energy", sizeof(double), &usedsize, &in_energy);

  bool full_deposit = false;
  if (TMath::Abs(energy/in_energy - 1.0) < FULL_DEPOSIT_THRESHOLD) {
    EvsSet("PhAbsMode:FullDeposit");
    full_deposit = true;
  }

  if (!full_deposit) {
    return AS_SKIP;
  }

  return AS_OK;
}
