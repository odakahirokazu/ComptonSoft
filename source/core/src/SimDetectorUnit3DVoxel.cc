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

#include "SimDetectorUnit3DVoxel.hh"

#include <iostream>
#include <algorithm>
#include <memory>
#include <cmath>
#include <boost/format.hpp>

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"
#include "TRandom3.h"
#include "TH3.h"

#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"
#include "WeightingPotentialPixel.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

SimDetectorUnit3DVoxel::SimDetectorUnit3DVoxel()
{
}

SimDetectorUnit3DVoxel::~SimDetectorUnit3DVoxel() = default;

void SimDetectorUnit3DVoxel::initializeEvent()
{
  RealDetectorUnit3DVoxel::initializeEvent();
  DeviceSimulation::initializeEvent();
}

void SimDetectorUnit3DVoxel::simulatePulseHeights()
{
  const int N = NumberOfRawHits();
  for (int i=0; i<N; i++) {
    DetectorHit_sptr rawhit = getRawHit(i);
    const double edep = rawhit->EnergyDeposit();
    const double localposx = rawhit->LocalPositionX();
    const double localposy = rawhit->LocalPositionY();
    const double localposz = rawhit->LocalPositionZ();

    if (edep == 0.0) {
      rawhit->addFlags(flag::PrioritySide);
      if (isAnodeReadout()) {
        rawhit->addFlags(flag::AnodeSide);
      }
      else if (isCathodeReadout()) {
        rawhit->addFlags(flag::CathodeSide);
      }
      
      fillVoxel(rawhit);
      insertSimulatedHit(rawhit);
      continue;
    }

    const VoxelID voxel = findVoxel(localposx, localposy, localposz);
    
    if (DiffusionMode() == 0) {
      DetectorHit_sptr hit = generateHit(*rawhit, voxel);
      insertSimulatedHit(hit);
    }
    else {
      DetectorHit_sptr hit = generateHit(*rawhit, voxel);
      const double energyCharge = hit->EnergyCharge();

      const int numDivision = DiffusionDivisionNumber();
      const double edepDivision = edep/numDivision;
      const double energyChargeDivision = energyCharge/numDivision;
      
      double diffusionSigma = 0.0;
      if (isAnodeReadout()) {
        diffusionSigma = DiffusionSigmaAnode(localposz);
      }
      else if (isCathodeReadout()) {
        diffusionSigma = DiffusionSigmaCathode(localposz);
      }

      std::vector<DetectorHit_sptr> diffusionHits;
      for (int l=0; l<numDivision; l++) {
        const double dx = gRandom->Gaus(0.0, diffusionSigma);
        const double dy = gRandom->Gaus(0.0, diffusionSigma);
        const double dz = gRandom->Gaus(0.0, diffusionSigma);
        const VoxelID voxelDiff = findVoxel(localposx+dx, localposy+dy, localposz+dz);

        auto itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                             [&](const DetectorHit_sptr& hit) {
                               return (hit->Voxel()==voxelDiff);
                             });
        if (itHit != diffusionHits.end()) {
          DetectorHit_sptr& hit = *itHit;
          hit->setEnergyDeposit( hit->EnergyDeposit() + edepDivision );
          hit->setEnergyCharge( hit->EnergyCharge() + energyChargeDivision );
        }
        else {
          DetectorHit_sptr hitDivision(new DetectorHit(*hit));
          hitDivision->setEnergyDeposit(edepDivision);
          hitDivision->setEnergyCharge(energyChargeDivision);
          hitDivision->setVoxel(voxelDiff);
          diffusionHits.push_back(hitDivision);
        }
      }

      for (auto itHit=diffusionHits.begin(); itHit!=diffusionHits.end(); itHit++) {
        DetectorHit_sptr& hit = *itHit;
        insertSimulatedHit(hit);
      }
    }
  }
}

DetectorHit_sptr SimDetectorUnit3DVoxel::generateHit(const DetectorHit& rawhit,
                                                     const VoxelID& voxel)
{
  DetectorHit_sptr hit(new DetectorHit(rawhit));
  hit->addFlags(flag::PrioritySide);
  if (isAnodeReadout()) {
    hit->addFlags(flag::AnodeSide);
  }
  else if (isCathodeReadout()) {
    hit->addFlags(flag::CathodeSide);
  }
    
  if (checkRange(voxel)) {
    hit->setVoxel(voxel);
  }
  else {
    hit->setVoxel(VoxelID::OnMergin, VoxelID::OnMergin, VoxelID::OnMergin);
    return hit;
  }

  applyQuenching(hit);

  const double edep = hit->EnergyDeposit();
  const double localposx = hit->LocalPositionX();
  const double localposy = hit->LocalPositionY();
  const double localposz = hit->LocalPositionZ();
  const double energyCharge = calculateEnergyCharge(voxel,
                                                    edep,
                                                    localposx,
                                                    localposy,
                                                    localposz);
  hit->setEnergyCharge(energyCharge);

  return hit;
}

void SimDetectorUnit3DVoxel::printSimulationParameters(std::ostream& os) const
{
  os << "<SimDetectorUnit3DVoxel>" << '\n'
     << "Upside anode   : " << isUpSideAnode() << '\n'
     << "Upside readout : " << isUpSideReadout() << '\n'
     << std::endl;
  DeviceSimulation::printSimulationParameters(os);
}

} /* namespace comptonsoft */
