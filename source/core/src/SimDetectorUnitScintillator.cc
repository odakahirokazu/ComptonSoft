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

#include "SimDetectorUnitScintillator.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"

namespace comptonsoft {

SimDetectorUnitScintillator::SimDetectorUnitScintillator() = default;

SimDetectorUnitScintillator::~SimDetectorUnitScintillator() = default;

void SimDetectorUnitScintillator::initializeEvent()
{
  RealDetectorUnitScintillator::initializeEvent();
  DeviceSimulation::initializeEvent();
}

void SimDetectorUnitScintillator::simulatePulseHeights()
{
  const int N = NumberOfRawHits();
  for (int i=0; i<N; i++) {
    DetectorHit_sptr rawhit = getRawHit(i);
    double edep = rawhit->EnergyDeposit();
    if (rawhit->isProcess(process::NucleusHit)) {
      edep *= QuenchingFactor();
    }
    
    DetectorHit_sptr hit(new DetectorHit(*rawhit));
    hit->setPixel(0, 0);
    hit->setEnergyCharge(edep);
    
    insertSimulatedHit(hit);
  }
}

} /* namespace comptonsoft */
