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
#include <string>
#include <vector>
#include "DetectorHit.hh"
#include "TFile.h"
#include "TTree.h"
#include "TSpline.h"
#include "TF1.h"
#include "GainFunctionTF1.hh"
#include "GainFunctionSpline.hh"
namespace comptonsoft {

RealDetectorUnitLArTPC::RealDetectorUnitLArTPC()
    : readoutElectrode_(ElectrodeSide::Undefined){
}

RealDetectorUnitLArTPC::~RealDetectorUnitLArTPC() {
  if (recombinationCorrectionFile_) {
    recombinationCorrectionFile_->Close();
    delete recombinationCorrectionFile_;
    recombinationCorrectionFile_ = nullptr;
  }
}

void RealDetectorUnitLArTPC::printDetectorParameters(std::ostream &os) const {
  VRealDetectorUnit::printDetectorParameters(os);
  os << "  Recombination correction: " << recombinationCorrectionMode()<< "\n";
  if (recombinationCorrectionMode() == 2 || recombinationCorrectionMode() == 3) {
    os << "  Recombination correction File: " << (recombinationCorrectionFile_ ? recombinationCorrectionFile_->GetName() : "None") << "\n";
    os << "  Number of zbins: " << (recombinationCorrectionZMapping_ ? recombinationCorrectionZMapping_->GetNbinsX() : 0) << "\n";
  }
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
  if (recombinationCorrectionMode() > 0) {
    applyRecombinationCorrection(hitsReconstructed);
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

std::tuple<double, double> RealDetectorUnitLArTPC::applyRecombinationCorrectionWithLight(const DetectorHit_sptr &hit) const {
  const auto wexc = Wexc();
  const auto wion = Wion();
  const auto epi = hit->EPI();
  const auto epi_error = hit->EPIError();
  const auto photon_count = hit->PhotonCount();
  const auto photon_count_error = hit->PhotonCountError();
  const double corrected_epi = (epi / wion + photon_count) * wexc;
  const double corrected_epi_error = wexc * std::sqrt(std::pow(epi_error / wion, 2) + std::pow(photon_count_error, 2)); // propagation of uncertainty
  return std::make_tuple(corrected_epi, corrected_epi_error);
}

std::tuple<double, double> RealDetectorUnitLArTPC::applyRecombinationCorrectionWithCorrectionFile(const DetectorHit_sptr &hit) const {
  if (!recombinationCorrectionZMapping_ || recombinationCorrectionFunction_.empty()) {
    throw std::runtime_error("RealDetectorUnitLArTPC::applyRecombinationCorrectionWithCorrectionFile: Recombination correction function is not set.");
  }
  const auto z = hit->PositionZ() / CLHEP::cm;
  const int bin = recombinationCorrectionZMapping_->FindBin(z);
  if (bin < 1 || bin > recombinationCorrectionZMapping_->GetNbinsX()) {
    return std::make_tuple(0, 0); // Forced to return zero if the hit is out of the z range of the correction file
  }
  const int index = static_cast<int>(recombinationCorrectionZMapping_->GetBinContent(bin));
  if (index < 0 || index >= static_cast<int>(recombinationCorrectionFunction_.size())) {
    throw std::runtime_error("RealDetectorUnitLArTPC::applyRecombinationCorrectionWithCorrectionFile: Correction function index is out of range: " + std::to_string(index));
  }
  const auto correction_func = recombinationCorrectionFunction_[index];
  const auto epi = hit->EPI() / CLHEP::keV;
  const auto epi_error = hit->EPIError() / CLHEP::keV;
  const double factor = correction_func->eval(epi) / epi;
  const double corrected_epi = epi * factor * CLHEP::keV;
  const double corrected_epi_error = epi_error * factor * CLHEP::keV; // propagation of uncertainty
  return std::tuple<double, double>(corrected_epi, corrected_epi_error);
}

void RealDetectorUnitLArTPC::applyRecombinationCorrection(DetectorHitVector &hits) const {
 double corrected_epi, corrected_epi_error;
  if (recombinationCorrectionMode() == 1) {
    for (auto &hit: hits) {
      std::tie(corrected_epi, corrected_epi_error) = applyRecombinationCorrectionWithLight(hit);
      hit->setEPI(corrected_epi);
      hit->setEPIError(corrected_epi_error);
    }
  }
  else if (recombinationCorrectionMode() == 2) {
    for (auto &hit: hits) {
      std::tie(corrected_epi, corrected_epi_error) = applyRecombinationCorrectionWithCorrectionFile(hit);
      hit->setEPI(corrected_epi);
      hit->setEPIError(corrected_epi_error);
    } 
  }
  else if (recombinationCorrectionMode() == 3) {
    if (hits.size() == 1) {
      std::tie(corrected_epi, corrected_epi_error) = applyRecombinationCorrectionWithLight(hits[0]);
      hits[0]->setEPI(corrected_epi);
      hits[0]->setEPIError(corrected_epi_error);
    }
    else {
      // If there are multiple hits, apply correction with light only to avoid the complexity of handling multiple hits with the correction file.
      for (auto &hit: hits) {
        std::tie(corrected_epi, corrected_epi_error) = applyRecombinationCorrectionWithCorrectionFile(hit);
        hit->setEPI(corrected_epi);
        hit->setEPIError(corrected_epi_error);
      }
    }
  }
  else {
    // No correction
    return;
  }
}

void RealDetectorUnitLArTPC::setRecombinationCorrectionFile(const std::string &filename, const std::string &meta_tree_name) {
  if (filename.empty()) {
    return;
  }
  recombinationCorrectionFile_= TFile::Open(filename.c_str(), "READ");
  if (!recombinationCorrectionFile_ || recombinationCorrectionFile_->IsZombie()) {
    throw std::runtime_error("RealDetectorUnitLArTPC::setRecombinationCorrectionFile: Cannot open file: " + filename);
  }
  double bin_z_min;
  double bin_z_max;
  std::string object_name;
  TTree *meta_tree = dynamic_cast<TTree *>(recombinationCorrectionFile_->Get(meta_tree_name.c_str()));
  if (!meta_tree) {
    throw std::runtime_error("RealDetectorUnitLArTPC::setRecombinationCorrectionFile: Cannot find meta tree: " + meta_tree_name);
  }
  std::vector<double> bins_z;
  std::vector<std::string> object_name_vec;
  meta_tree->SetBranchAddress("bin_z_min", &bin_z_min);
  meta_tree->SetBranchAddress("bin_z_max", &bin_z_max);
  meta_tree->SetBranchAddress("object_name", &object_name);
  bins_z.reserve(meta_tree->GetEntries());
  object_name_vec.reserve(meta_tree->GetEntries());
  for (int i = 0; i < meta_tree->GetEntries(); ++i) {
    meta_tree->GetEntry(i);
    bins_z.push_back(bin_z_min);
    object_name_vec.push_back(object_name);
  }
  bins_z.push_back(bin_z_max);
  recombinationCorrectionZMapping_ = new TH1I("recombinationCorrectionZMapping", "Recombination Correction Z Mapping", bins_z.size(), bins_z.data());
  for (int i = 0; i < static_cast<int>(object_name_vec.size()); ++i) {
    recombinationCorrectionZMapping_->SetBinContent(i+1, i);
    auto func = recombinationCorrectionFile_->Get(object_name_vec[i].c_str());
    if (!func) {
      throw std::runtime_error("RealDetectorUnitLArTPC::setRecombinationCorrectionFile: Cannot find function: " + object_name_vec[i]);
    }
    if (func->InheritsFrom(TSpline::Class())) {
      auto gain_func = new GainFunctionSpline();
      gain_func->set(dynamic_cast<TSpline *>(func));
      recombinationCorrectionFunction_.push_back(gain_func);
    }
    else if (func->InheritsFrom(TF1::Class())) {
      auto gain_func = new GainFunctionTF1();
      gain_func->set(dynamic_cast<TF1 *>(func));
      recombinationCorrectionFunction_.push_back(gain_func);
    }
    else {
      throw std::runtime_error("RealDetectorUnitLArTPC::setRecombinationCorrectionFile: Object is neither TSpline nor TF1: " + object_name_vec[i]);
    }
  }
  delete meta_tree;
  meta_tree = nullptr;
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
