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

#include "RealDetectorUnitLArTPC.hh"
#include <list>
#include <algorithm>
#include <iterator>
#include <utility>
#include "DetectorHit.hh"

namespace comptonsoft {

RealDetectorUnitLArTPC::RealDetectorUnitLArTPC()
    : readoutElectrode_(ElectrodeSide::Undefined){
}

RealDetectorUnitLArTPC::~RealDetectorUnitLArTPC() = default;

void RealDetectorUnitLArTPC::printDetectorParameters(std::ostream &os) const {
  VRealDetectorUnit::printDetectorParameters(os);
  os << "  Recombination correction: " << isRecombinationCorrectionEnabled()<< "\n";
  os << "  Wion: " << Wion_/CLHEP::eV << " eV" << "\n";
  os << "  Wexc: " << Wexc_/CLHEP::eV << " eV" << "\n";
}

bool RealDetectorUnitLArTPC::setReconstructionDetails(int mode) {
  if (mode == 1 || mode == 4) {
    // basic reconstruction with clustering
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

void RealDetectorUnitLArTPC::reconstruct(const DetectorHitVector& hitSignals,
                                          DetectorHitVector& hitsReconstructed)
{
  std::transform(hitSignals.begin(), hitSignals.end(),
                 std::back_inserter(hitsReconstructed),
                 [](const DetectorHit_sptr& hit) {
                   auto hit2 = hit->clone();
                   hit2->setEnergy(hit->EPI());
                   hit2->setEnergyError(hit->EPIError());
                   hit2->setMultiplicity(1);
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
  correctPhotonDetectionEfficiency(hitsReconstructed);
  if (isRecombinationCorrectionEnabled()) {
    correctRecombination(hitsReconstructed);
  }
}

void RealDetectorUnitLArTPC::determinePosition(DetectorHitVector& hits) const
{
  for (auto& hit: hits) {
    const VoxelID voxel = hit->Voxel();
    hit->setPosition(Position(voxel));
    hit->setLocalPosition(LocalPosition(voxel));
    hit->setPositionError(PositionError(hit->LocalPositionError()));
  }
}

void RealDetectorUnitLArTPC::correctRecombination(DetectorHitVector &hits) const {
  for (auto &hit: hits) {
    applyRecombinationCorrection(hit);
  }
}

void RealDetectorUnitLArTPC::applyRecombinationCorrection(DetectorHit_sptr &hit) const {
  const auto wexc = Wexc();
  const auto wion = Wion();
  const auto epi = hit->EPI();
  const auto epi_error = hit->EPIError();
  const auto photon_count = hit->PhotonCount();
  const auto photon_count_error = hit->PhotonCountError();
  const double corrected_epi = (epi / wion + photon_count) * wexc;
  const double corrected_epi_error = wexc * std::sqrt(std::pow(epi_error / wion, 2) + std::pow(photon_count_error, 2)); // propagation of uncertainty
  hit->setEPI(corrected_epi);
  hit->setEPIError(corrected_epi_error);
  hit->setEnergy(corrected_epi);
  hit->setEnergyError(corrected_epi_error);
}

void RealDetectorUnitLArTPC::correctPhotonDetectionEfficiency(DetectorHitVector &hits) const {
  for (auto &hit: hits) {
    const auto photon_count = hit->PhotonCount();
    const auto photon_count_error = hit->PhotonCountError();
    const double corrected_photon_count = photon_count * photonDetectionEfficiencyInv_;
    const double corrected_photon_count_error = photon_count_error * photonDetectionEfficiencyInv_; // propagation of uncertainty
    hit->setPhotonCount(corrected_photon_count);
    hit->setPhotonCountError(corrected_photon_count_error);
  }
}

} /* namespace comptonsoft */
