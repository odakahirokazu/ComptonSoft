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

#include "SimDetectorUnitLite.hh"

#include <iostream>
#include "HXISGDFlagDef.hh"
#include "DetectorHit.hh"

namespace comptonsoft {


SimDetectorUnitLite::SimDetectorUnitLite()
{
  registerDetectorUnit( static_cast<RealDetectorUnit*>(this) );
}


SimDetectorUnitLite::~SimDetectorUnitLite()
{
}


void SimDetectorUnitLite::setThresholdEnergyCathode(double val)
{
  setThresholdEnergy(val);
}


void SimDetectorUnitLite::setThresholdEnergyAnode(double val)
{
  setThresholdEnergy(val);
}


double SimDetectorUnitLite::ThresholdEnergyCathode()
{
  return ThresholdEnergyX();
}


double SimDetectorUnitLite::ThresholdEnergyAnode()
{
  return ThresholdEnergyX();
}


void SimDetectorUnitLite::simulatePulseHeights()
{
  std::vector<DetectorHit_sptr>::iterator it;
  std::vector<DetectorHit_sptr>::iterator itEnd = m_RawHits.end();
  
  for (it = m_RawHits.begin(); it != itEnd; it++) {
    DetectorHit_sptr rawhit = *it;
    double edep = rawhit->getEdep();
    double localposx = rawhit->getLocalPosX();
    double localposy = rawhit->getLocalPosY();
    
    if (edep == 0.0) {
      rawhit->addFlag(PRIORITY_SIDE);
      fillStrip(rawhit);
      insertSimulatedHit(rawhit);
      continue;
    }

    StripPair sp = StripXYByLocalPosition(localposx, localposy);
    DetectorHit_sptr hit = generateHit(*rawhit, sp);
    insertSimulatedHit(hit);
  }
}


DetectorHit_sptr SimDetectorUnitLite::generateHit(const DetectorHit& rawhit,
                                                  const StripPair& sp)
{
  DetectorHit_sptr hit(new DetectorHit(rawhit));
  hit->addFlag(PRIORITY_SIDE);
  
  if (RangeCheck(sp)) {
    hit->setStrip(sp);
  }
  else {
    hit->setStrip(comptonsoft::GUARD_RING, comptonsoft::GUARD_RING);
    return hit;
  }

  double edep = hit->getEdep();
  double localposx = hit->getLocalPosX();
  double localposy = hit->getLocalPosY();
  double localposz = hit->getLocalPosZ();

  if (hit->isProcess(comptonsoft::QUENCHING)) {
    edep *= QuenchingFactor();
  }

  double pha = calculatePHA(sp, edep, localposx, localposy, localposz);
  hit->setPHA(pha);

  return hit;
}

}
