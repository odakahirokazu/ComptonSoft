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
#include "DetectorHit.hh"
#include "FlagDefinition.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

VDeviceSimulation::VDeviceSimulation()
  : DepthSensingMode_(0),
    DepthResolution_(0.0),
    QuenchingFactor_{1.0, 0.0, 0.0},
    TimeResolutionFast_(100.0*unit::ns),
    TimeResolutionSlow_(1000.0*unit::ns),
    pedestalEnabled_(false)
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

  ChannelDisabledVector_.assign(TableSize, 0);
  NoiseParam0Vector_.assign(TableSize, 0.0);
  NoiseParam1Vector_.assign(TableSize, 0.0);
  NoiseParam2Vector_.assign(TableSize, 0.0);
  ThresholdVector_.assign(TableSize, 0.0);
  TriggerDiscriminationCenterVector_.assign(TableSize, 0.0);
  TriggerDiscriminationSigmaVector_.assign(TableSize, 0.0);
}

void VDeviceSimulation::makeDetectorHits()
{
  simulatePulseHeights();
  removeHitsOutOfPixelRange(SimulatedHits_);
  mergeHits(SimulatedHits_);

  if (isPedestalEnabled()) {
    constexpr int time_group = 0;
    constexpr double time_of_signal = std::numeric_limits<double>::max();
    std::list<DetectorHit_sptr> pedestalHits = generatePedestalSignals(time_group, time_of_signal);
    std::move(pedestalHits.begin(), pedestalHits.end(), std::back_inserter(SimulatedHits_));
    mergeHits(SimulatedHits_);
  }

  removeHitsAtChannelsDisabled(SimulatedHits_);

  for (auto& hit: SimulatedHits_) {
    makeEPI(hit);
  }
  removeHitsBelowThresholds(SimulatedHits_);
  
  for (auto& hit: SimulatedHits_) {
    hit->setTimeGroup(0);
    hit->setSelfTriggered(true);
    hit->setTriggered(true);
    hit->setSelfTriggeredTime(hit->RealTime());
    hit->setTriggeredTime(hit->RealTime());
    assignLocalDepth(hit);
    assignLocalPositionError(hit);
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

void VDeviceSimulation::removeHitsAtChannelsDisabled(std::list<DetectorHit_sptr>& hits)
{
  hits.remove_if([this](DetectorHit_sptr hit) {
      return (getChannelDisabled(hit->Pixel()) == channel_status::readout_disable);
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

void VDeviceSimulation::performTriggerDiscrimination()
{
  using HitType = DetectorHit_sptr;
  for (HitType& hit: SimulatedHits_) {
    if (checkTriggerDiscrimination(hit->EnergyCharge(), hit->Pixel())) {
      hit->setSelfTriggered(true);
      hit->setSelfTriggeredTime(hit->RealTime());
    }
  }
}

void VDeviceSimulation::applyQuenching(DetectorHit_sptr hit) const
{
  if (hit->isProcess(process::NucleusHit)) {
    const double edep = hit->EnergyDeposit();
    const double edep_new = edep * QuenchingFactor();
    hit->setEnergyDeposit(edep_new);
    hit->addFlags(flag::EnergyDepositQuenched);
  }
}

double VDeviceSimulation::
calculateEnergyCharge(const PixelID& ,
                      double energyDeposit,
                      double , double , double ) const
{
  return energyDeposit;
}

std::tuple<double, double> VDeviceSimulation::
calculateEPI(double energyCharge, const PixelID& pixel) const
{
  // y^2 = param0^2 + (param1*x^(1/2))^2 + (param2*x)^2
  // x: energy in keV
  // y: noise value in keV
  
  const double x = energyCharge/unit::keV; // in keV
  const double param0 = getNoiseParam0(pixel);
  const double param1 = getNoiseParam1(pixel);
  const double param2 = getNoiseParam2(pixel);
  const double y2 = param0*param0 + param1*param1*x + param2*param2*x*x; // in keV2
  const double sigma = std::sqrt(y2) * unit::keV;
  const double ePI = CLHEP::RandGauss::shoot(energyCharge, sigma);
  return std::make_tuple(ePI, sigma);
}

void VDeviceSimulation::makeEPI(DetectorHit_sptr hit)
{
  double ePI(0.0), ePIError(0.0);
  std::tie(ePI, ePIError) = calculateEPI(hit->EnergyCharge(), hit->Pixel());
  hit->setEPI(ePI);
  hit->setEPIError(ePIError);
}

bool VDeviceSimulation::
checkTriggerDiscrimination(double energyCharge, const PixelID& pixel) const
{
  const int channelStatus = getChannelDisabled(pixel);
  if (channelStatus==channel_status::readout_disable ||
      channelStatus==channel_status::trigger_disable) {
    return false;
  }
  const double sigma = getTriggerDiscriminationSigma(pixel);
  const double energyForTrigger = CLHEP::RandGauss::shoot(energyCharge, sigma);
  const double threshold = getTriggerDiscriminationCenter(pixel);
  return (energyForTrigger >= threshold);
}

void VDeviceSimulation::fillVoxel(DetectorHit_sptr hit) const
{
  const double localposx = hit->LocalPositionX();
  const double localposy = hit->LocalPositionY();
  const double localposz = hit->LocalPositionZ();
  const VoxelID voxel = findVoxel(localposx, localposy, localposz);
  hit->setVoxel(voxel);
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

  const double SMALL_TIME = 1.0e-15 * unit::second;
  mergeHitsIfCoincident(SMALL_TIME, SimulatedHits_);
  sortHitsInTimeOrder(SimulatedHits_);
  mergeHitsIfCoincident(TimeResolutionFast_, SimulatedHits_);
  performTriggerDiscrimination();
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
  using HitType = DetectorHit_sptr;
  bool triggered = false;
  const auto it = std::find_if(std::begin(SimulatedHits_),
                               std::end(SimulatedHits_),
                               [](HitType hit)-> bool {
                                 return hit->SelfTriggered();
                               });
  if (it != std::end(SimulatedHits_)) { triggered = true; }
  return triggered;
}

double VDeviceSimulation::FirstTriggerTime() const
{
  using HitType = DetectorHit_sptr;

  double triggeredTime = std::numeric_limits<double>::max();
  const auto it = std::find_if(std::begin(SimulatedHits_),
                               std::end(SimulatedHits_),
                               [](HitType hit)-> bool {
                                 return hit->SelfTriggered();
                               });
  if (it != std::end(SimulatedHits_)) {
    triggeredTime = (*it)->SelfTriggeredTime();
  }
  return triggeredTime;
}

void VDeviceSimulation::makeDetectorHitsAtTime(double time_triggered, int time_group)
{
  using HitType = DetectorHit_sptr;
  
  const double time_start = time_triggered - 0.5*TimeResolutionSlow_;
  const double time_end   = time_triggered + 0.5*TimeResolutionSlow_;
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

  std::list<HitType> hits(itStart, itEnd);
  
  // erase all hits before time_end
  SimulatedHits_.erase(std::begin(SimulatedHits_), itEnd);

  mergeHits(hits);

  if (isPedestalEnabled()) {
    std::list<HitType> pedestalHits = generatePedestalSignals(time_group, time_end);
    std::move(pedestalHits.begin(), pedestalHits.end(), std::back_inserter(hits));
    mergeHits(hits);
  }
  
  removeHitsAtChannelsDisabled(hits);

  for (auto& hit: hits) {
    makeEPI(hit);
  }
  removeHitsBelowThresholds(hits);
  
  for (auto& hit: hits) {
    hit->setTimeGroup(time_group);
    hit->setTriggered(true);
    hit->setTriggeredTime(time_triggered);
    assignLocalDepth(hit);
    assignLocalPositionError(hit);
    insertDetectorHit(hit);
  }
}

std::list<DetectorHit_sptr> VDeviceSimulation::
generatePedestalSignals(int time_group, double time_of_signal) const
{
  std::list<DetectorHit_sptr> hits;
  const int NumChannels = SizeOfTable();
  for (int i=0; i<NumChannels; i++) {
    const PixelID pixel = TableIndexToPixelID(i);
    DetectorHit_sptr hit(new DetectorHit);
    hit->setInstrumentID(getInstrumentID());
    hit->setDetectorID(getDetectorID());
    hit->setTimeGroup(time_group);
    hit->setPixel(pixel);
    hit->setRealTime(time_of_signal);
    hits.push_back(hit);
  }

  return hits;
}

void VDeviceSimulation::assignLocalDepth(DetectorHit_sptr hit) const
{
  const double localposx = hit->LocalPositionX();
  const double localposy = hit->LocalPositionY();
  const double localposz = hit->LocalPositionZ();

  if (DepthSensingMode()==1) {
    double zMeasured = CLHEP::RandGauss::shoot(localposz, DepthResolution());
    if (zMeasured < -0.5*getThickness()) {
      zMeasured = -0.5*getThickness();
    }
    else if (zMeasured > +0.5*getThickness()) {
      zMeasured = +0.5*getThickness();
    }

    hit->setDepthSensingMode(DepthSensingMode());
    hit->setLocalPosition(localposx, localposy, zMeasured);
  }
}

void VDeviceSimulation::assignLocalPositionError(DetectorHit_sptr hit) const
{
  const double conversionToSigma = 1.0/std::sqrt(12.0);
  const double dx = getVoxelPitchX()*conversionToSigma;
  const double dy = getVoxelPitchY()*conversionToSigma;
  const double dz = (DepthSensingMode()==1) ? DepthResolution() : (getVoxelPitchZ()*conversionToSigma);
  hit->setLocalPositionError(dx, dy, dz);
}

} /* namespace comptonsoft */
