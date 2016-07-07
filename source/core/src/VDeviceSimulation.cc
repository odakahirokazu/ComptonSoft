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

#include <algorithm>
#include <iterator>
#include <cmath>
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
  simulatePulseHeights();
  removeHitsOutOfPixelRange(SimulatedHits_);
  mergeHits(SimulatedHits_);
  removeHitsAtBrokenChannels(SimulatedHits_);

  for (auto& hit: SimulatedHits_) {
    hit->setEPI( calculateEPI(hit->EnergyCharge(), hit->Pixel()) );
  }
  removeHitsBelowThresholds(SimulatedHits_);
  
  for (auto& hit: SimulatedHits_) {
    insertDetectorHit(hit);
  }
  SimulatedHits_.clear();
}

void VDeviceSimulation::makeRawDetectorHits()
{
  for (auto& hit: RawHits_) {
    insertDetectorHit(hit);
  }
}

void VDeviceSimulation::removeHitsOutOfPixelRange(std::list<DetectorHit_sptr>& hits)
{
  hits.remove_if([this](DetectorHit_sptr hit) {
      return !checkRange(hit->Pixel());
    });
}

void VDeviceSimulation::removeHitsAtBrokenChannels(std::list<DetectorHit_sptr>& hits)
{
  hits.remove_if([this](DetectorHit_sptr hit) {
      return getBrokenChannel(hit->Pixel());
    });
}

void VDeviceSimulation::removeHitsBelowThresholds(std::list<DetectorHit_sptr>& hits)
{
  hits.remove_if([this](DetectorHit_sptr hit) {
      const double threshold = getThreshold(hit->Pixel());
      return (hit->EPI() < threshold);
    });
}

void VDeviceSimulation::mergeHits(std::list<DetectorHit_sptr>& hits)
{
  for (auto it1=hits.begin(); it1!=hits.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    while ( it2 != hits.end() ) {
      if ( (*it1)->isInSamePixel(**it2) ) {
        (*it1)->merge(**it2);
        it2 = hits.erase(it2);
      }
      else {
        ++it2;
      }
    }
  }
}

void VDeviceSimulation::mergeHitsIfCoincident(double time_width,
                                              std::list<DetectorHit_sptr>& hits)
{
  for (auto it1=hits.begin(); it1!=hits.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    while ( it2 != hits.end() ) {
      const double hit1Time = (*it1)->RealTime();
      const double hit2Time = (*it2)->RealTime();
      if ( (*it1)->isInSamePixel(**it2) && std::abs(hit1Time-hit2Time)<=time_width ) {
        (*it1)->merge(**it2);
        it2 = hits.erase(it2);
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

void VDeviceSimulation::prepareForTimingProcess()
{
  simulatePulseHeights();
  removeHitsOutOfPixelRange(SimulatedHits_);

  const double SMALL_TIME = 1.0e-15 * second;
  mergeHitsIfCoincident(SMALL_TIME, SimulatedHits_);
  sortHitsInTimeOrder(SimulatedHits_);
  mergeHitsIfCoincident(TimeResolutionFast_, SimulatedHits_);
}

void VDeviceSimulation::sortHitsInTimeOrder(std::list<DetectorHit_sptr>& hits)
{
  using HitType = DetectorHit_sptr;
  hits.sort([](HitType h1, HitType h2)-> bool {
      return h1->RealTime() < h2->RealTime();
    });
}

bool VDeviceSimulation::isSelfTriggered() const
{
  // tentative implementation
  
  bool triggered = false;
  if (!SimulatedHits_.empty()) {
    triggered = true;
  }
  return triggered;
}

double VDeviceSimulation::FirstEventTime() const
{
  if (SimulatedHits_.empty()) { return 0.0; }
  return SimulatedHits_.front()->RealTime();
}

void VDeviceSimulation::makeDetectorHitsAtTime(double time_start, int time_group)
{
  using HitType = DetectorHit_sptr;
  
  const double time_end = time_start + TimeResolutionSlow_;
  const auto itStart = std::find_if(std::begin(SimulatedHits_),
                                    std::end(SimulatedHits_),
                                    [=](HitType hit)-> bool {
                                      const double hitTime = hit->RealTime();
                                      return (hitTime >= time_start);
                                    });
  const auto itEnd = std::find_if(std::begin(SimulatedHits_),
                                  std::end(SimulatedHits_),
                                  [=](HitType hit)-> bool {
                                    const double hitTime = hit->RealTime();
                                    return (hitTime > time_end);
                                  });

  // move hits in this time group to a new list
  std::list<HitType> hits(itStart, itEnd);
  SimulatedHits_.erase(itStart, itEnd);

  mergeHits(hits);
  removeHitsAtBrokenChannels(hits);

  for (auto& hit: hits) {
    hit->setTimeGroup(time_group);
    hit->setEPI( calculateEPI(hit->EnergyCharge(), hit->Pixel()) );
  }
  removeHitsBelowThresholds(hits);
  
  for (auto& hit: hits) {
    insertDetectorHit(hit);
  }
}

} /* namespace comptonsoft */
