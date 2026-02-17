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

#include "SimDetectorUnitLArTPCPixel.hh"

#include <algorithm>
#include <boost/format.hpp>
#include <cmath>
#include <iostream>
#include <memory>

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

SimDetectorUnitLArTPCPixel::SimDetectorUnitLArTPCPixel() {
  setNumPixelsInWPCalculation(20.0);
};

SimDetectorUnitLArTPCPixel::~SimDetectorUnitLArTPCPixel() = default;
void SimDetectorUnitLArTPCPixel::initializeEvent() {
  RealDetectorUnitLArTPCPixel::initializeEvent();
  DeviceSimulation::initializeEvent();
}

double SimDetectorUnitLArTPCPixel::ChargeCollectionEfficiency(const VoxelID &voxel, double x, double y, double z) const {
  const vector3_t pixelCenter = LocalPosition(voxel);
  const double xInPixel = x - pixelCenter.x();
  const double yInPixel = y - pixelCenter.y();

  const int ix = CCEMap_->GetXaxis()->FindBin(xInPixel / unit::cm);
  const int iy = CCEMap_->GetYaxis()->FindBin(yInPixel / unit::cm);
  const int iz = CCEMap_->GetZaxis()->FindBin(z / unit::cm);

  return CCEMap_->GetBinContent(ix, iy, iz);
}

double SimDetectorUnitLArTPCPixel::WeightingPotential(const VoxelID &voxel, double x, double y, double z) {
  const vector3_t pixelCenter = LocalPosition(voxel);
  const double xInPixel = x - pixelCenter.x();
  const double yInPixel = y - pixelCenter.y();

  const int ix = WPMap_->GetXaxis()->FindBin(xInPixel / unit::cm);
  const int iy = WPMap_->GetYaxis()->FindBin(yInPixel / unit::cm);
  const int iz = WPMap_->GetZaxis()->FindBin(z / unit::cm);

  return WPMap_->GetBinContent(ix, iy, iz);
}

void SimDetectorUnitLArTPCPixel::buildWPMap() {
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

void SimDetectorUnitLArTPCPixel::buildWPMap(int nx, int ny, int nz, double pixel_factor) {
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
  WPMap_ = new TH3D(histname.c_str(), histname.c_str(),
                    nx, -0.5 * MapSizeX / unit::cm, +0.5 * MapSizeX / unit::cm,
                    ny, -0.5 * MapSizeY / unit::cm, +0.5 * MapSizeY / unit::cm,
                    nz, -0.5 * MapSizeZ / unit::cm, +0.5 * MapSizeZ / unit::cm);

  auto wpModel = std::make_unique<WeightingPotentialPixel>();
  wpModel->setGeometry(getPixelPitchX(), getPixelPitchY(), getThickness(),
                       NumPixelsInWPCalculation());
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

void SimDetectorUnitLArTPCPixel::buildCCEMap() {
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

void SimDetectorUnitLArTPCPixel::buildCCEMap(int nx, int ny, int nz, double pixel_factor) {
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
  CCEMap_ = new TH3D(histname.c_str(), histname.c_str(),
                     nx, -0.5 * MapSizeX / unit::cm, +0.5 * MapSizeX / unit::cm,
                     ny, -0.5 * MapSizeY / unit::cm, +0.5 * MapSizeY / unit::cm,
                     nz, -0.5 * MapSizeZ / unit::cm, +0.5 * MapSizeZ / unit::cm);

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

void SimDetectorUnitLArTPCPixel::checkBranchingGamma() {
  const int N = NumberOfRawHits();
  uint64_t flag = 0;
  for (int i = 0; i < N; i++) {
    DetectorHit_sptr rawhit = getRawHit(i);
    if (rawhit->isProcess(process::Bremsstrahlung)) {
      flag |= flag::BranchingGammaIncluded;
    }
    if ((rawhit->isProcess(process::PhotoelectricAbsorption) || rawhit->isProcess(process::ComptonScattering)) && rawhit->TrackID() != 1) {
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

void SimDetectorUnitLArTPCPixel::simulatePulseHeights() {
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
      const double energyCharge = calculateEnergyCharge(pixel,
                                                        hit->EnergyCharge(),
                                                        localposx,
                                                        localposy,
                                                        localposz);
      hit->setEnergyCharge(energyCharge);
      insertSimulatedHit(hit);
    }
    else {
      DetectorHit_sptr hit = generateHit(*rawhit, pixel);

      const int numDivision = DiffusionDivisionNumber();
      const double localposx_hit = hit->LocalPositionX();
      const double localposy_hit = hit->LocalPositionY();
      const double localposz_hit = hit->LocalPositionZ();
      const double edep_hit = hit->EnergyDeposit();
      const double edepDivision = edep_hit / numDivision;
      const double echargeDivision = hit->EnergyCharge() / numDivision;

      std::vector<DetectorHit_sptr> diffusionHits;
      double diffusionSigma = 0.0;
      if (isAnodeReadout()) {
        diffusionSigma = DiffusionSigmaAnode(localposz_hit);
      }
      else if (isCathodeReadout()) {
        diffusionSigma = DiffusionSigmaCathode(localposz_hit);
      }
      for (int l = 0; l < numDivision; l++) {
        const double dx = gRandom->Gaus(0.0, diffusionSigma);
        const double dy = gRandom->Gaus(0.0, diffusionSigma);
        const PixelID pixelDiff = findPixel(localposx_hit + dx, localposy_hit + dy);

        const double energyChargeDivision = calculateEnergyCharge(pixelDiff,
                                                                  echargeDivision,
                                                                  localposx_hit,
                                                                  localposy_hit,
                                                                  localposz_hit); // even if diffusion exists, dz is not included in CCE calculation
        auto itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                             [&](const DetectorHit_sptr &hit) {
                               return (hit->Pixel() == pixelDiff);
                             });
        if (itHit != diffusionHits.end()) {
          DetectorHit_sptr &hit = *itHit;
          hit->setEnergyDeposit(hit->EnergyDeposit() + edepDivision);
          hit->setEnergyCharge(hit->EnergyCharge() + energyChargeDivision);
        }
        else {
          DetectorHit_sptr hitDivision(new DetectorHit(*hit));
          hitDivision->setEnergyDeposit(edepDivision);
          hitDivision->setEnergyCharge(energyChargeDivision);
          hitDivision->setPixel(pixelDiff);
          diffusionHits.push_back(hitDivision);
        }
      }

      for (auto itHit = diffusionHits.begin(); itHit != diffusionHits.end(); itHit++) {
        DetectorHit_sptr &hit = *itHit;
        insertSimulatedHit(hit);
      }
    }
  }
}

DetectorHit_sptr SimDetectorUnitLArTPCPixel::generateHit(const DetectorHit &rawhit,
                                                         const PixelID &pixel) {
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
  applyRecombination(hit);

  return hit;
}
void SimDetectorUnitLArTPCPixel::mergeHits(std::list<DetectorHit_sptr> &hits) {
  for (auto it1 = hits.begin(); it1 != hits.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    while (it2 != hits.end()) {
      if ((*it1)->isInSamePixel(**it2)) {
        (*it1)->merge(**it2);
        if (abs((*it1)->LocalPositionZ() - (*it2)->LocalPositionZ()) > DistanceThreshold()) {
          (*it1)->addFlags(flag::DistantClustered);
        }
        it2 = hits.erase(it2);
      }
      else {
        ++it2;
      }
    }
  }
}
} /* namespace comptonsoft */
