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
#include "MultiChannelData.hh"
#include "DetectorHit.hh"
#include "FlagDefinition.hh"

namespace comptonsoft {

SimDetectorUnitLite::SimDetectorUnitLite() = default;

SimDetectorUnitLite::~SimDetectorUnitLite() = default;

void SimDetectorUnitLite::initializeEvent()
{
  RealDetectorUnit2DPad::initializeEvent();
  BasicDeviceSimulation::initializeEvent();
}

void SimDetectorUnitLite::simulatePulseHeights()
{
  const int N = NumRawHits();
  for (int i=0; i<N; i++) {
    DetectorHit_sptr rawhit = getRawHit(i);
    double edep = rawhit->EnergyDeposit();
    double localposx = rawhit->LocalPositionX();
    double localposy = rawhit->LocalPositionY();
    
    if (edep == 0.0) {
      rawhit->addFlags(flag::PrioritySide);
      fillPixel(rawhit);
      insertSimulatedHit(rawhit);
      continue;
    }

    PixelID pixel = findPixel(localposx, localposy);
    DetectorHit_sptr hit = generateHit(*rawhit, pixel);
    insertSimulatedHit(hit);
  }
}

DetectorHit_sptr SimDetectorUnitLite::generateHit(const DetectorHit& rawhit,
                                                  const PixelID& pixel)
{
  DetectorHit_sptr hit(new DetectorHit(rawhit));
  hit->addFlags(flag::PrioritySide);
  
  if (checkRange(pixel)) {
    hit->setPixel(pixel);
  }
  else {
    hit->setPixel(PixelID::OnGuardRing, PixelID::OnGuardRing);
    return hit;
  }

  double edep = hit->EnergyDeposit();
  const double localposx = hit->LocalPositionX();
  const double localposy = hit->LocalPositionY();
  const double localposz = hit->LocalPositionZ();

  if (hit->isProcess(process::Quenching)) {
    edep *= QuenchingFactor();
  }

  const double pha = calculatePHA(pixel, edep, localposx, localposy, localposz);
  hit->setPHA(pha);

  return hit;
}

} /* namespace comptonsoft */
