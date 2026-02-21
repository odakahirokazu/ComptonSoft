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

#include "RealDetectorUnitLArTPCPixel.hh"
#include "DetectorHit.hh"
#include <algorithm>
#include <iterator>
#include <list>
#include <utility>

namespace comptonsoft {

RealDetectorUnitLArTPCPixel::RealDetectorUnitLArTPCPixel()
    : readoutElectrode_(ElectrodeSide::Undefined) {
}

RealDetectorUnitLArTPCPixel::~RealDetectorUnitLArTPCPixel() = default;


void RealDetectorUnitLArTPCPixel::printDetectorParameters(std::ostream &os) const {
  VRealDetectorUnit::printDetectorParameters(os);
  os << "  Recombination correction: " << isRecombinationCorrectionEnabled()<< "\n";
  os << "  Wion: " << Wion_/CLHEP::eV << " eV" << "\n";
  os << "  Wexc: " << Wexc_/CLHEP::eV << " eV" << "\n";
}

bool RealDetectorUnitLArTPCPixel::setReconstructionDetails(int mode) {
  if (mode == 1 || mode == 4) {
    // basic reconstruction with clustering
    // mode 4 is using threshold-based clustering
    setClusteringOn(true);
  }
  else if (mode == 2) {
    // basic reconstruction without clustering
    setClusteringOn(false);
  }
  else if (mode == 3) {
    // basic reconstruction with clustering (contact: false)
    setClusteringOn(true, false);
  }
  else {
    std::cout << "Unknown reconstruction mode is given." << std::endl;
    return false;
  }
  return true;
}

void RealDetectorUnitLArTPCPixel::reconstruct(const DetectorHitVector &hitSignals,
                                              DetectorHitVector &hitsReconstructed) {
  std::transform(hitSignals.begin(), hitSignals.end(),
                 std::back_inserter(hitsReconstructed),
                 [](const DetectorHit_sptr &hit) {
                   auto hit2 = hit->clone();
                   hit2->setEnergy(hit->EPI());
                   hit2->setEnergyError(hit->EPIError());
                   return hit2;
                 });
  determinePosition(hitsReconstructed);
  if (isClusteringOn()) {
    if (ReconstructionMode() == 4) {
      clusterByThreshold(hitsReconstructed);
    }
    else {
      cluster(hitsReconstructed);
    }
  }
  if (isRecombinationCorrectionEnabled()) {
    applyRecombinationCorrection(hitsReconstructed);
  }
}

void RealDetectorUnitLArTPCPixel::determinePosition(DetectorHitVector &hits) const {
  for (auto &hit: hits) {
    const PixelID pixel = hit->Pixel();
    if (hit->DepthSensingMode() == 1) {
      hit->setPosition(PositionWithDepth(pixel, hit->LocalPositionZ()));
      hit->setLocalPosition(LocalPositionWithDepth(pixel, hit->LocalPositionZ()));
    }
    else {
      hit->setPosition(Position(pixel));
      hit->setLocalPosition(LocalPosition(pixel));
    }
    hit->setPositionError(PositionError(hit->LocalPositionError()));
  }
}

void RealDetectorUnitLArTPCPixel::applyRecombinationCorrection(DetectorHitVector &hits) {
  const auto wexc = Wexc();
  const auto wion = Wion();
  for (auto &hit: hits) {
    const auto epi = hit->EPI();
    const auto epi_error = hit->EPIError();
    const auto photon_count = hit->PhotonCount();
    const auto photon_count_error = hit->PhotonCountError();
    const double corrected_epi = (epi / wion + photon_count) * wexc;
    const double corrected_epi_error = wexc * std::sqrt(std::pow(epi_error / wion, 2) + std::pow(photon_count_error, 2)); // propagation of uncertainty
    hit->setEPI(corrected_epi);
    hit->setEPIError(corrected_epi_error);
  }
}

} /* namespace comptonsoft */
