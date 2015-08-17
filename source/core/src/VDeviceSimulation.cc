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

#include "VDeviceSimulation.hh"

#include "CLHEP/Random/RandGauss.h"
#include "AstroUnits.hh"
#include "CSTypes.hh"
#include "DetectorHit.hh"
#include "FlagDefinition.hh"

namespace comptonsoft {

VDeviceSimulation::VDeviceSimulation()
  : QuenchingFactor_{1.0, 0.0, 0.0},
    TimeResolutionFast_(100.0*ns),
    TimeResolutionSlow_(2000.0*ns)
{
}

VDeviceSimulation::~VDeviceSimulation() = default;

void VDeviceSimulation::initializeEvent()
{
  RawHits_.clear();
  SimulatedHits_.clear();
}

void VDeviceSimulation::initializeTables()
{
  const int TableSize = SizeOfTable();

  PixelIDVector_.resize(TableSize);
  for (int i=0; i<TableSize; i++) {
    PixelIDVector_[i] = TableIndexToPixelID(i);
  }

  BrokenChannelVector_.assign(TableSize, 0);
  NoiseParamVector_.assign(TableSize, NoiseParam_t{0.0, 0.0, 0.0});
  ThresholdVector_.assign(TableSize, 0.0);
}

void VDeviceSimulation::makeDetectorHits()
{
  simulateDetectorHits();
  for (auto& hit: SimulatedHits_) {
    insertDetectorHit(hit);
  }
}

void VDeviceSimulation::makeRawDetectorHits()
{
  for (auto& hit: RawHits_) {
    insertDetectorHit(hit);
  }
}

void VDeviceSimulation::simulateDetectorHits()
{
  simulatePulseHeights();
  removeHitsOutOfPixelRange();
  mergeSimulatedHits();
  
  auto it = SimulatedHits_.begin();
  while (it != SimulatedHits_.end()) {
    (*it)->setEPI( calculateEPI((*it)->EnergyCharge(), (*it)->Pixel()) );
    if ( getBrokenChannel((*it)->Pixel()) ) {
      it = SimulatedHits_.erase(it);
      continue;
    }
    
    const double threshold = getThreshold((*it)->Pixel());
    if ( (*it)->EPI() < threshold ) {
      it = SimulatedHits_.erase(it);
      continue;
    }

    ++it;
  }
}

void VDeviceSimulation::removeHitsOutOfPixelRange()
{
  auto it = SimulatedHits_.begin();
  while (it != SimulatedHits_.end()) {
    if ( checkRange((*it)->Pixel()) ) {
      ++it;
    }
    else {
      it = SimulatedHits_.erase(it);
    }
  }
}

void VDeviceSimulation::mergeSimulatedHits()
{
  for (auto it1=SimulatedHits_.begin(); it1!=SimulatedHits_.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    while ( it2 != SimulatedHits_.end() ) {
      if ( (*it1)->isInSamePixel(**it2) ) {
        (*it1)->merge(**it2);
        it2 = SimulatedHits_.erase(it2);
      }
      else {
        ++it2;
      }
    }
  }
}

void VDeviceSimulation::mergeSimulatedHitsIfCoincident(double time_width)
{
  for (auto it1 = SimulatedHits_.begin(); it1!=SimulatedHits_.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    while ( it2 != SimulatedHits_.end() ) {
      double hit1Time = (*it1)->RealTime();
      double hit2Time = (*it2)->RealTime();
      if ( (*it2)->isInSamePixel(**it1) && std::abs(hit1Time-hit2Time)<=time_width ) {
        (*it1)->merge(**it2);
        it2 = SimulatedHits_.erase(it2);
      }
      else {
        ++it2;
      }
    }
  }
}

double VDeviceSimulation::
calculateEnergyCharge(const PixelID& ,
                      double energyDeposit,
                      double , double , double ) const
{
  return energyDeposit;
}

double VDeviceSimulation::
calculateEPI(double energyCharge, const PixelID& pixel) const
{
  // noise^2 = param0^2 + (param1*x^(1/2))^2 + (param2*x)^2
  // x: energy in keV
  
  const double x = energyCharge/keV; // keV
  const NoiseParam_t params = getNoiseParam(pixel);
  const double noiseA1 = std::get<0>(params); // keV
  const double noiseB2 = std::get<1>(params)*std::get<1>(params)*x; // keV2
  const double noiseC1 = std::get<2>(params) * x; // keV
  
  const double sigma2 = noiseA1*noiseA1 + noiseB2 + noiseC1*noiseC1; // keV2
  const double sigma = std::sqrt(sigma2) * keV;
  const double ePI = CLHEP::RandGauss::shoot(energyCharge, sigma);
  return ePI;
}

void VDeviceSimulation::fillPixel(DetectorHit_sptr hit) const
{
  const double localposx = hit->LocalPositionX();
  const double localposy = hit->LocalPositionY();
  const PixelID pixel = findPixel(localposx, localposy);
  hit->setPixel(pixel);
}

void VDeviceSimulation::initializeTimingProcess()
{
  simulatePulseHeights();
  removeHitsOutOfPixelRange();

  const double SMALL_TIME = 1.0e-15 * second;
  mergeSimulatedHitsIfCoincident(SMALL_TIME);
  sortSimulatedHitsInTimeOrder();
  mergeSimulatedHitsIfCoincident(TimeResolutionFast_);
}

void VDeviceSimulation::sortSimulatedHitsInTimeOrder()
{
  typedef DetectorHit_sptr HitType;
  SimulatedHits_.sort([](HitType h1, HitType h2)-> bool {
      return h1->RealTime() < h2->RealTime();
    });
}

double VDeviceSimulation::FirstEventTime() const
{
  if (SimulatedHits_.empty()) { return 0.0; }
  return SimulatedHits_.front()->RealTime();
}

void VDeviceSimulation::makeDetectorHitsAtTime(double time_start, int time_group)
{
  const double time_end = time_start + TimeResolutionSlow_;

  auto it1 = SimulatedHits_.begin();
  auto itStart = SimulatedHits_.begin();
  auto itEnd = SimulatedHits_.end();

  while (it1 != SimulatedHits_.end()) {
    const double hitTime = (*it1)->RealTime();
    if (hitTime < time_start) {
      ++it1;
      continue;
    }
    else {
      itStart = it1;
      break;
    }
  }

  while (it1 != SimulatedHits_.end()) {
    const double hitTime = (*it1)->RealTime();
    if (hitTime <= time_end) {
      (*it1)->setTimeGroup(time_group);
      ++it1;
      continue;
    }
    else {
      itEnd = it1;
      break;
    }
  }

  for (it1 = itStart; it1 != itEnd; ++it1) {
    auto it2 = it1;
    ++it2;
    while ( it2 != itEnd ) {
      if ( (*it2)->isInSamePixel(**it1) ) {
        **it1 += **it2;
        it2 = SimulatedHits_.erase(it2);
      }
      else {
        ++it2;
      }
    }
  }
  
  it1 = itStart;
  while (it1 != itEnd) {
    (*it1)->setTimeGroup(time_group);
    (*it1)->setEPI( calculateEPI((*it1)->EnergyCharge(), (*it1)->Pixel()) );
    
    if ( getBrokenChannel((*it1)->Pixel()) ) {
      it1 = SimulatedHits_.erase(it1);
      continue;
    }
    
    double threshold = getThreshold((*it1)->Pixel());
    if ( (*it1)->EPI() < threshold ) {
      it1 = SimulatedHits_.erase(it1);
      continue;
    }
    
    insertDetectorHit(*it1);
    it1 = SimulatedHits_.erase(it1);
  }
}

} /* namespace comptonsoft */
