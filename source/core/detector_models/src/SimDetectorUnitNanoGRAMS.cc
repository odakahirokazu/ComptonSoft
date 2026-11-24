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

#include "SimDetectorUnitNanoGRAMS.hh"

#include <algorithm>
#include <array>
#include <boost/format.hpp>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "TH3.h"
#include "TRandom3.h"

#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "FlagDefinition.hh"

#include "WeightingPotentialPixel.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

SimDetectorUnitNanoGRAMS::SimDetectorUnitNanoGRAMS()
{
  setNumPixelsInWPCalculation(20.0);
};

SimDetectorUnitNanoGRAMS::~SimDetectorUnitNanoGRAMS() = default;

void SimDetectorUnitNanoGRAMS::initializeEvent()
{
  RealDetectorUnitNanoGRAMS::initializeEvent();
  LArTPCDeviceSimulation::initializeEvent();
  eventStartTime_ = std::numeric_limits<double>::quiet_NaN();
}

double SimDetectorUnitNanoGRAMS::arrivalTime(const DetectorHit& hit) const
{
  return hit.RealTime() + driftTimeFromDepth(hit.LocalPositionZ());
}

int SimDetectorUnitNanoGRAMS::FECOfPixel(const PixelID& pixel) const
{
  if (getChannelMap() == nullptr) {
    throw std::runtime_error("SimDetectorUnitNanoGRAMS: a channel map is required to find the FEC of a pixel.");
  }
  const int fec = PixelToChanelID(pixel).Section();
  return (0 <= fec && fec < NumFECs) ? fec : -1;
}

void SimDetectorUnitNanoGRAMS::mergeHitsIfArrivalCoincident(double time_width, std::list<DetectorHit_sptr>& hits)
{
  for (auto it1 = hits.begin(); it1 != hits.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    while (it2 != hits.end()) {
      if ((*it1)->isInSamePixel(**it2) && std::abs(arrivalTime(**it1) - arrivalTime(**it2)) <= time_width) {
        (*it1)->merge(**it2);
        it2 = hits.erase(it2);
      }
      else {
        ++it2;
      }
    }
  }
}

void SimDetectorUnitNanoGRAMS::prepareForTimingProcess()
{
  double startTime = std::numeric_limits<double>::infinity();
  for (const DetectorHit_sptr& rawhit : getRawHits()) {
    if (rawhit->EnergyDeposit() > 0.0) {
      startTime = std::min(startTime, rawhit->RealTime());
    }
  }
  eventStartTime_ = std::isfinite(startTime) ? startTime : std::numeric_limits<double>::quiet_NaN();

  simulatePulseHeights();
  // 光は電荷の読み出し窓に関係なくイベント全体で測るので、窓で選ぶ前にイベント全体の光子数を全ヒットに入れる
  // (makeDetectorHits と同じ順序)
  makePhotonCount();
  std::list<DetectorHit_sptr>& hits = getSimulatedHits();
  removeHitsOutOfPixelRange(hits);

  const auto earlierArrival = [this](const DetectorHit_sptr& hit1, const DetectorHit_sptr& hit2) {
    return arrivalTime(*hit1) < arrivalTime(*hit2);
  };
  const double SMALL_TIME = 1.0e-15 * unit::second;
  mergeHitsIfArrivalCoincident(SMALL_TIME, hits);
  hits.sort(earlierArrival);
  mergeHitsIfArrivalCoincident(TimingResolutionForTrigger(), hits);
  hits.sort(earlierArrival);

  for (DetectorHit_sptr& hit : hits) {
    if (checkTriggerDiscrimination(hit->EnergyCharge(), hit->Pixel())) {
      hit->setSelfTriggered(true);
      hit->setSelfTriggeredTime(arrivalTime(*hit));
    }
  }
}

void SimDetectorUnitNanoGRAMS::makeDetectorHitsAtTime(double /* time_triggered */, int time_group)
{
  std::list<DetectorHit_sptr>& simulatedHits = getSimulatedHits();

  std::array<double, NumFECs> fecTriggerTime;
  fecTriggerTime.fill(std::numeric_limits<double>::quiet_NaN());
  for (const DetectorHit_sptr& hit : simulatedHits) {
    if (!hit->SelfTriggered()) {
      continue;
    }
    const int fec = FECOfPixel(hit->Pixel());
    if (fec >= 0 && std::isnan(fecTriggerTime[fec])) {
      fecTriggerTime[fec] = hit->SelfTriggeredTime();
    }
  }

  const double halfWindow = 0.5 * TimingResolutionForEnergyMeasurement();
  std::array<std::list<DetectorHit_sptr>, NumFECs> fecHits;
  for (const DetectorHit_sptr& hit : simulatedHits) {
    const int fec = FECOfPixel(hit->Pixel());
    if (fec < 0 || std::isnan(fecTriggerTime[fec])) {
      continue;
    }
    const double hitArrivalTime = arrivalTime(*hit);
    if (fecTriggerTime[fec] - halfWindow <= hitArrivalTime && hitArrivalTime <= fecTriggerTime[fec] + halfWindow) {
      fecHits[fec].push_back(hit);
    }
  }
  simulatedHits.clear();

  if (isPedestalEnabled()) {
    std::list<DetectorHit_sptr> pedestalHits = generatePedestalSignals(time_group, 0.0);
    for (DetectorHit_sptr& hit : pedestalHits) {
      const int fec = FECOfPixel(hit->Pixel());
      if (fec < 0 || std::isnan(fecTriggerTime[fec])) {
        continue;
      }
      hit->setRealTime(fecTriggerTime[fec] + halfWindow);
      fecHits[fec].push_back(hit);
    }
  }

  for (int fec = 0; fec < NumFECs; fec++) {
    std::list<DetectorHit_sptr>& hits = fecHits[fec];
    if (hits.empty()) {
      continue;
    }
    mergeHits(hits);
    removeHitsAtChannelsDisabled(hits);
    for (DetectorHit_sptr& hit : hits) {
      makeEPI(hit);
    }
    removeHitsBelowThresholds(hits);

    for (DetectorHit_sptr& hit : hits) {
      hit->setTimeGroup(time_group);
      hit->setTriggered(true);
      hit->setTriggeredTime(fecTriggerTime[fec]);
      assignLocalDepth(hit);
      assignLocalPositionError(hit);
      insertDetectorHit(hit);
    }
  }
}

double SimDetectorUnitNanoGRAMS::driftTimeFromDepth(double z) const
{
  if (!(DriftVelocity() > 0.0)) {
    throw std::runtime_error("SimDetectorUnitNanoGRAMS: drift velocity must be set (> 0) to calculate the drift time.");
  }
  return (getSizeZ() * 0.5 - z) / DriftVelocity();
}

void SimDetectorUnitNanoGRAMS::reconstruct(const DetectorHitVector& hitSignals, DetectorHitVector& hitsReconstructed)
{
  const bool useTriggerTime = std::isfinite(eventStartTime_);
  std::array<double, NumFECs> maxZ;
  maxZ.fill(-std::numeric_limits<double>::infinity());
  for (const DetectorHit_sptr& hit : hitSignals) {
    hit->setEPIForSelection(hit->EPI());
    const int fec = hit->DetectorSection();
    if (fec < 0 || fec >= NumFECs) {
      continue;
    }
    if (useTriggerTime) {
      if (hit->Triggered()) {
        setDriftTime(fec, hit->TriggeredTime() - eventStartTime_);
      }
      continue;
    }
    const double localZ = hit->LocalPositionZ();
    if (maxZ[fec] < localZ) {
      maxZ[fec] = localZ;
      setDriftTime(fec, driftTimeFromDepth(localZ));
    }
  }
  RealDetectorUnitNanoGRAMS::reconstruct(hitSignals, hitsReconstructed);
}

void SimDetectorUnitNanoGRAMS::determinePosition(DetectorHitVector& hits) const
{
  if (std::isfinite(eventStartTime_)) {
    RealDetectorUnitNanoGRAMS::determinePosition(hits);
  }
  else {
    RealDetectorUnitLArTPCPixel::determinePosition(hits);
  }
}

double SimDetectorUnitNanoGRAMS::ChargeCollectionEfficiency(const VoxelID& voxel, double x, double y, double z) const
{
  const vector3_t pixelCenter = LocalPosition(voxel);
  const double xInPixel = x - pixelCenter.x();
  const double yInPixel = y - pixelCenter.y();

  const int ix = CCEMap_->GetXaxis()->FindBin(xInPixel / unit::cm);
  const int iy = CCEMap_->GetYaxis()->FindBin(yInPixel / unit::cm);
  const int iz = CCEMap_->GetZaxis()->FindBin(z / unit::cm);

  return CCEMap_->GetBinContent(ix, iy, iz);
}

double SimDetectorUnitNanoGRAMS::WeightingPotential(const VoxelID& voxel, double x, double y, double z)
{
  const vector3_t pixelCenter = LocalPosition(voxel);
  const double xInPixel = x - pixelCenter.x();
  const double yInPixel = y - pixelCenter.y();

  const int ix = WPMap_->GetXaxis()->FindBin(xInPixel / unit::cm);
  const int iy = WPMap_->GetYaxis()->FindBin(yInPixel / unit::cm);
  const int iz = WPMap_->GetZaxis()->FindBin(z / unit::cm);

  return WPMap_->GetBinContent(ix, iy, iz);
}

void SimDetectorUnitNanoGRAMS::buildWPMap()
{
  if (ChargeCollectionMode() == 2) {
    buildWPMap(31, 31, 100, 1.0);
  }
  else if (ChargeCollectionMode() == 3) {
    buildWPMap(93, 93, 100, 3.0);
  }
  else {
    std::cout << "Error : buildWPMap() " << std::endl;
    return;
  }
}

void SimDetectorUnitNanoGRAMS::buildWPMap(int nx, int ny, int nz, double pixel_factor)
{
  if (nx < 1 || ny < 1 || nz <= 1) {
    std::cout << "Error : buildWPMap " << std::endl;
    return;
  }

  const double MapSizeX = getPixelPitchX() * pixel_factor;
  const double MapSizeY = getPixelPitchY() * pixel_factor;
  const double MapSizeZ = getThickness() * static_cast<double>(nz) / static_cast<double>(nz - 1);

  if (ChargeCollectionMode() == 2) {
    if (MapSizeX < getPixelPitchX() * 0.999 || MapSizeY < getPixelPitchY() * 0.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 2.";
    }
  }
  else if (ChargeCollectionMode() == 3) {
    if (MapSizeX < getPixelPitchX() * 2.999 || MapSizeY < getPixelPitchY() * 2.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 3.";
    }
  }

  const std::string histname = (boost::format("wp_%04d") % getID()).str();
  WPMap_ = new TH3D(histname.c_str(), histname.c_str(), nx, -0.5 * MapSizeX / unit::cm, +0.5 * MapSizeX / unit::cm, ny,
                    -0.5 * MapSizeY / unit::cm, +0.5 * MapSizeY / unit::cm, nz, -0.5 * MapSizeZ / unit::cm,
                    +0.5 * MapSizeZ / unit::cm);

  auto wpModel = std::make_unique<WeightingPotentialPixel>();
  wpModel->setGeometry(getPixelPitchX(), getPixelPitchY(), getThickness(), NumPixelsInWPCalculation());
  wpModel->initializeTable();
  std::cout << "calculating weighing potential..." << std::endl;
  for (int ix = 1; ix <= nx; ix++) {
    std::cout << '*' << std::flush;
    const double x = WPMap_->GetXaxis()->GetBinCenter(ix) * unit::cm;
    for (int iy = 1; iy <= ny; iy++) {
      const double y = WPMap_->GetYaxis()->GetBinCenter(iy) * unit::cm;
      wpModel->setXY(x, y);
      for (int iz = 1; iz <= nz; iz++) {
        const double z = WPMap_->GetZaxis()->GetBinCenter(iz) * unit::cm;
        if (isUpSideReadout()) {
          double wp = 0.;
          if (iz == nz) {
            if (std::abs(x) < 0.5 * getPixelPitchX() && std::abs(y) < 0.5 * getPixelPitchY()) {
              wp = 1.;
            }
            else {
              wp = 0.;
            }
          }
          else if (iz == 1) {
            wp = 0.;
          }
          else {
            wp = wpModel->calculateWeightingPotential(z);
          }
          WPMap_->SetBinContent(ix, iy, iz, wp);
        }
        else {
          double wp = 0.;
          if (iz == 1) {
            if (std::abs(x) < 0.5 * getPixelPitchX() && std::abs(y) < 0.5 * getPixelPitchY()) {
              wp = 1.;
            }
            else {
              wp = 0.;
            }
          }
          else if (iz == nz) {
            wp = 0.;
          }
          else {
            wp = wpModel->calculateWeightingPotential(-z);
          }
          WPMap_->SetBinContent(ix, iy, iz, wp);
        }
      }
    }
  }
  std::cout << std::endl;
}

void SimDetectorUnitNanoGRAMS::buildCCEMap()
{
  if (ChargeCollectionMode() == 2) {
    buildCCEMap(31, 31, 100, 1.0);
  }
  else if (ChargeCollectionMode() == 3) {
    buildCCEMap(93, 93, 100, 3.0);
  }
  else {
    std::cout << "Error : buildCCEMap() " << std::endl;
    return;
  }
}

void SimDetectorUnitNanoGRAMS::buildCCEMap(int nx, int ny, int nz, double pixel_factor)
{
  if (nx < 1 || ny < 1 || nz < 1) {
    std::cout << "Error : buildCCEMap " << std::endl;
    return;
  }

  const double MapSizeX = getPixelPitchX() * pixel_factor;
  const double MapSizeY = getPixelPitchY() * pixel_factor;
  const double MapSizeZ = getThickness();

  if (ChargeCollectionMode() == 2) {
    if (MapSizeX < getPixelPitchX() * 0.999 || MapSizeY < getPixelPitchY() * 0.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 2.";
    }
  }
  else if (ChargeCollectionMode() == 3) {
    if (MapSizeX < getPixelPitchX() * 2.999 || MapSizeY < getPixelPitchY() * 2.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 3.";
    }
  }

  const std::string histname = (boost::format("cce_%04d") % getID()).str();
  CCEMap_ = new TH3D(histname.c_str(), histname.c_str(), nx, -0.5 * MapSizeX / unit::cm, +0.5 * MapSizeX / unit::cm, ny,
                     -0.5 * MapSizeY / unit::cm, +0.5 * MapSizeY / unit::cm, nz, -0.5 * MapSizeZ / unit::cm,
                     +0.5 * MapSizeZ / unit::cm);

  std::cout << "calculating charge collection efficiency..." << std::endl;

  for (int ix = 1; ix <= nx; ix++) {
    std::cout << '*' << std::flush;
    for (int iy = 1; iy <= ny; iy++) {
      if (!isUsingSymmetry() || (ix <= (nx + 1) / 2 && iy <= (ny + 1) / 2)) {
        const double x_in_cm = CCEMap_->GetXaxis()->GetBinCenter(ix);
        const double y_in_cm = CCEMap_->GetYaxis()->GetBinCenter(iy);
        const int binx = WPMap_->GetXaxis()->FindBin(x_in_cm);
        const int biny = WPMap_->GetYaxis()->FindBin(y_in_cm);

        const int numPoints = nz + 1;
        boost::shared_array<double> wp(new double[numPoints]);
        for (int k = 0; k < numPoints; k++) {
          const double z_in_cm = CCEMap_->GetZaxis()->GetBinLowEdge(k + 1);
          const int binz = WPMap_->GetZaxis()->FindBin(z_in_cm);
          wp[k] = WPMap_->GetBinContent(binx, biny, binz);
        }
        setWeightingPotential(isUpSideReadout(), wp, numPoints);

        for (int iz = 1; iz <= nz; iz++) {
          const double z = CCEMap_->GetZaxis()->GetBinCenter(iz) * unit::cm;
          const double cce = calculateCCE(z);
          CCEMap_->SetBinContent(ix, iy, iz, cce);
        }
      }
    }
  }

  if (isUsingSymmetry()) {
    for (int ix = 1; ix <= nx; ix++) {
      for (int iy = 1; iy <= ny; iy++) {
        for (int iz = 1; iz <= nz; iz++) {
          if (ix > (nx + 1) / 2 && iy <= (ny + 1) / 2) {
            const double cce = CCEMap_->GetBinContent(nx - ix + 1, iy, iz);
            CCEMap_->SetBinContent(ix, iy, iz, cce);
          }
          else if (ix <= (nx + 1) / 2 && iy > (ny + 1) / 2) {
            const double cce = CCEMap_->GetBinContent(ix, ny - iy + 1, iz);
            CCEMap_->SetBinContent(ix, iy, iz, cce);
          }
          else if (ix > (ny + 1) / 2 && iy > (ny + 1) / 2) {
            const double cce = CCEMap_->GetBinContent(nx - ix + 1, ny - iy + 1, iz);
            CCEMap_->SetBinContent(ix, iy, iz, cce);
          }
        }
      }
    }
  }

  std::cout << std::endl;
  std::cout << "Charge collection efficiency map is built." << std::endl;
}

void SimDetectorUnitNanoGRAMS::checkBranchingGamma()
{
  const int N = NumberOfRawHits();
  uint64_t flag = 0;
  for (int i = 0; i < N; i++) {
    DetectorHit_sptr rawhit = getRawHit(i);
    if (rawhit->isProcess(process::Bremsstrahlung)) {
      flag |= flag::BranchingGammaIncluded;
    }
    if ((rawhit->isProcess(process::PhotoelectricAbsorption) || rawhit->isProcess(process::ComptonScattering)) &&
        rawhit->TrackID() != 1) {
      rawhit->addFlags(flag::BranchingGammaDeposited);
    }
    if (rawhit->isProcess(process::PhotoelectricAbsorption) && rawhit->TrackID() == 1) {
      rawhit->addFlags(flag::LastHitPhotoAbsorption);
    }
    if (rawhit->isProcess(process::RayleighScattering)) {
      flag |= flag::HasRayleighScattering;
    }
  }
  if (flag) {
    for (int i = 0; i < N; i++) {
      DetectorHit_sptr rawhit = getRawHit(i);
      rawhit->addFlags(flag);
    }
  }
}

void SimDetectorUnitNanoGRAMS::simulatePulseHeights()
{
  checkBranchingGamma();
  const int N = NumberOfRawHits();
  for (int i = 0; i < N; i++) {
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

      fillPixel(rawhit);
      insertSimulatedHit(rawhit);
      continue;
    }

    const PixelID pixel = findPixel(localposx, localposy);

    if (DiffusionMode() == 0) {
      DetectorHit_sptr hit = generateHit(*rawhit, pixel);
      insertSimulatedHit(hit);
    }
    else {
      DetectorHit_sptr hit = generateHit(*rawhit, pixel);
      const double energyCharge = hit->EnergyCharge();

      const int numDivision = DiffusionDivisionNumber();
      const double edep_hit = hit->EnergyDeposit();
      const double edepDivision = edep_hit / numDivision;
      const double energyChargeDivision = energyCharge / numDivision;
      const double photonCountDivision = hit->PhotonCount() / numDivision;

      std::vector<DetectorHit_sptr> diffusionHits;
      double diffusionSigma = 0.0;
      if (DiffusionMode() != 3) {
        if (isAnodeReadout()) {
          diffusionSigma = DiffusionSigmaAnode(localposz);
        }
        else if (isCathodeReadout()) {
          diffusionSigma = DiffusionSigmaCathode(localposz);
        }
      }
      else {
        double longitudinal, transverse;
        diffusionSigma = DiffusionSigmaAnode3D(localposz, longitudinal, transverse); // only use transverse diffusion.
      }
      for (int l = 0; l < numDivision; l++) {
        const double dx = gRandom->Gaus(0.0, diffusionSigma);
        const double dy = gRandom->Gaus(0.0, diffusionSigma);
        const PixelID pixelDiff = findPixel(localposx + dx, localposy + dy);

        auto itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                             [&](const DetectorHit_sptr& hit) { return (hit->Pixel() == pixelDiff); });
        if (itHit != diffusionHits.end()) {
          DetectorHit_sptr& hit = *itHit;
          hit->setEnergyDeposit(hit->EnergyDeposit() + edepDivision);
          hit->setEnergyCharge(hit->EnergyCharge() + energyChargeDivision);
          hit->setPhotonCount(hit->PhotonCount() + photonCountDivision);
        }
        else {
          DetectorHit_sptr hitDivision(new DetectorHit(*hit));
          hitDivision->setEnergyDeposit(edepDivision);
          hitDivision->setEnergyCharge(energyChargeDivision);
          hitDivision->setPhotonCount(photonCountDivision);
          hitDivision->setPixel(pixelDiff);
          diffusionHits.push_back(hitDivision);
        }
      }

      for (auto itHit = diffusionHits.begin(); itHit != diffusionHits.end(); itHit++) {
        DetectorHit_sptr& hit = *itHit;
        insertSimulatedHit(hit);
      }
    }

    // adjacent pad
    if (ChargeCollectionMode() >= 3) {
      DetectorHit_sptr hit;
      PixelID adjacentPixel;

      adjacentPixel.set(pixel.X() + 1, pixel.Y() + 1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      hit->setPhotonCount(0.0);
      insertSimulatedHit(hit);

      adjacentPixel.set(pixel.X() + 1, pixel.Y());
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      hit->setPhotonCount(0.0);
      insertSimulatedHit(hit);

      adjacentPixel.set(pixel.X() + 1, pixel.Y() - 1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      hit->setPhotonCount(0.0);
      insertSimulatedHit(hit);

      adjacentPixel.set(pixel.X(), pixel.Y() + 1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      hit->setPhotonCount(0.0);
      insertSimulatedHit(hit);

      adjacentPixel.set(pixel.X(), pixel.Y() - 1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      hit->setPhotonCount(0.0);
      insertSimulatedHit(hit);

      adjacentPixel.set(pixel.X() - 1, pixel.Y() + 1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      hit->setPhotonCount(0.0);
      insertSimulatedHit(hit);

      adjacentPixel.set(pixel.X() - 1, pixel.Y());
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      hit->setPhotonCount(0.0);
      insertSimulatedHit(hit);

      adjacentPixel.set(pixel.X() - 1, pixel.Y() - 1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      hit->setPhotonCount(0.0);
      insertSimulatedHit(hit);
    }
  }
}

DetectorHit_sptr SimDetectorUnitNanoGRAMS::generateHit(const DetectorHit& rawhit, const PixelID& pixel)
{
  DetectorHit_sptr hit(new DetectorHit(rawhit));
  hit->addFlags(flag::PrioritySide);
  if (isAnodeReadout()) {
    hit->addFlags(flag::AnodeSide);
  }
  else if (isCathodeReadout()) {
    hit->addFlags(flag::CathodeSide);
  }

  if (checkRange(pixel)) {
    hit->setPixel(pixel);
  }
  else {
    hit->setPixel(PixelID::OnMergin, PixelID::OnMergin);
    return hit;
  }

  applyQuenching(hit);
  applyRecombination(hit); // echarge is updated in this function.

  const double localposx = hit->LocalPositionX();
  const double localposy = hit->LocalPositionY();
  const double localposz = hit->LocalPositionZ();
  const double energyCharge = calculateEnergyCharge(pixel, hit->EnergyCharge(), localposx, localposy, localposz);
  hit->setEnergyCharge(energyCharge);

  return hit;
}

void SimDetectorUnitNanoGRAMS::mergeHits(std::list<DetectorHit_sptr>& hits)
{
  for (auto it1 = hits.begin(); it1 != hits.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    while (it2 != hits.end()) {
      if ((*it1)->isInSamePixel(**it2)) {
        (*it1)->merge(**it2);
        it2 = hits.erase(it2);
      }
      else {
        ++it2;
      }
    }
  }
}

} // namespace comptonsoft