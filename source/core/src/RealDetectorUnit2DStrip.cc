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

#include "RealDetectorUnit2DStrip.hh"
#include <algorithm>
#include <iterator>
#include "DetectorHit.hh"
#include "FlagDefinition.hh"

namespace comptonsoft
{

RealDetectorUnit2DStrip::RealDetectorUnit2DStrip()
  : prioritySide_(ElectrodeSide::Undefined),
    xStripSideElectrode_(ElectrodeSide::Undefined),
    hitSelectionMethod_(HitSelectionMethod_t::OneByOne),
    energyConsistencyRequired_(false),
    lowerEnergyConsistencyCheckFunctionC0_(0.0),
    lowerEnergyConsistencyCheckFunctionC1_(0.9),
    upperEnergyConsistencyCheckFunctionC0_(0.0),
    upperEnergyConsistencyCheckFunctionC1_(1.1)
{
}

RealDetectorUnit2DStrip::~RealDetectorUnit2DStrip() = default;

void RealDetectorUnit2DStrip::initializeEvent()
{
  VRealDetectorUnit::initializeEvent();
  cathodeSideHits_.clear();
  anodeSideHits_.clear();
}

bool RealDetectorUnit2DStrip::setReconstructionDetails(int mode)
{
  if (mode == 1) {
    hitSelectionMethod_ = HitSelectionMethod_t::OneByOne;
    energyConsistencyRequired_ = false;
    setClusteringOn(true);
  }
  else if (mode == 2) {
    hitSelectionMethod_ = HitSelectionMethod_t::OneByOne;
    energyConsistencyRequired_ = false;
    setClusteringOn(false);
  }
  else if (mode == 11) {
    hitSelectionMethod_ = HitSelectionMethod_t::OneByOne;
    energyConsistencyRequired_ = true;
    setClusteringOn(true);
  }
  else if (mode == 12) {
    hitSelectionMethod_ = HitSelectionMethod_t::OneByOne;
    energyConsistencyRequired_ = true;
    setClusteringOn(false);
  }
  else if (mode == 21) {
    hitSelectionMethod_ = HitSelectionMethod_t::MaxPulseHeight;
    energyConsistencyRequired_ = false;
    setClusteringOn(true);
  }
  else if (mode == 22) {
    hitSelectionMethod_ = HitSelectionMethod_t::MaxPulseHeight;
    energyConsistencyRequired_ = false;
    setClusteringOn(false);
  }
  else if (mode == 31) {
    hitSelectionMethod_ = HitSelectionMethod_t::MaxPulseHeight;
    energyConsistencyRequired_ = true;
    setClusteringOn(true);
  }
  else if (mode == 32) {
    hitSelectionMethod_ = HitSelectionMethod_t::MaxPulseHeight;
    energyConsistencyRequired_ = true;
    setClusteringOn(false);
  }
  else {
    std::cout << "Unknown reconstruction mode is given." << std::endl;
    return false;
  }
  return true;
}

void RealDetectorUnit2DStrip::
reconstruct(const DetectorHitVector& hitSignals,
            DetectorHitVector& hitReconstructed)
{
  for (auto it=hitSignals.begin(); it!=hitSignals.end(); ++it) {
    if ((*it)->isFlags(flag::AnodeSide)) {
      anodeSideHits_.push_back((*it)->clone());
    }
    else if ((*it)->isFlags(flag::CathodeSide)) {
      cathodeSideHits_.push_back((*it)->clone());
    }
  }

  auto compairStrip = [](const DetectorHit_sptr& h1, const DetectorHit_sptr& h2)-> bool {
    return ((h1->isXStrip() && (h1->Pixel().X() < h2->Pixel().X()))
            || (h1->isYStrip() && (h1->Pixel().Y() < h2->Pixel().Y())));
  };

  std::sort(anodeSideHits_.begin(), anodeSideHits_.end(), compairStrip);
  std::sort(cathodeSideHits_.begin(), cathodeSideHits_.end(), compairStrip);

  if (isClusteringOn()) {
    cluster(anodeSideHits_);
    cluster(cathodeSideHits_);
  }

  reconstructDoubleSides(cathodeSideHits_, anodeSideHits_, hitReconstructed);
}

void RealDetectorUnit2DStrip::cluster1Dimension(DetectorHitVector& hits) const
{
  DetectorHitVector clusteredHits;
  for (auto it=hits.begin(); it!=hits.end(); ++it) {
    if (!clusteredHits.empty()) {
      if (clusteredHits.back()->isAdjacent(**it)) {
        clusteredHits.back()->mergeAdjacentSignal(**it, DetectorHit::MergedPosition::CopyRight);
      }
      else {
        DetectorHit_sptr hit(new DetectorHit(**it));
        clusteredHits.push_back(hit);
      }
    }
  }
  hits = std::move(clusteredHits);
}

DetectorHit_sptr RealDetectorUnit2DStrip::
selectMaxEPI(const DetectorHitVector& hits) const
{
  auto it = std::max_element(hits.begin(), hits.end(),
                             [](DetectorHit_sptr h1, DetectorHit_sptr h2)-> bool {
                               return h1->EPI() < h2->EPI();
                             });
  return *it;
}

void RealDetectorUnit2DStrip::
reconstructDoubleSides(const DetectorHitVector& hitsCathode,
                       const DetectorHitVector& hitsAnode,
                       DetectorHitVector& hitReconstructed)
{
  if (HitSelectionMethod()==HitSelectionMethod_t::MaxPulseHeight) {
    reconstructWithMaxPulseHeightMethod(hitsCathode, hitsAnode, hitReconstructed);
  }
  else if (HitSelectionMethod()==HitSelectionMethod_t::OneByOne) {
    reconstructWithOneByOneMethod(hitsCathode, hitsAnode, hitReconstructed);
  }
}

void RealDetectorUnit2DStrip::
reconstructWithMaxPulseHeightMethod(const DetectorHitVector& hitsCathode,
                                    const DetectorHitVector& hitsAnode,
                                    DetectorHitVector& hitReconstructed)
{
  if (hitsCathode.size()==0 || hitsAnode.size()==0) {
    return;
  }
  
  DetectorHit_sptr hitCathode = selectMaxEPI(hitsCathode);
  DetectorHit_sptr hitAnode = selectMaxEPI(hitsAnode);
  DetectorHit_sptr merged = mergeDoubleSides(hitCathode, hitAnode);
  
  if (isEnergyConsistencyRequired()) {
    if (!checkEnergyConsistency(merged->EPI(), hitCathode->EPI())) {
      return;
    }
    if (!checkEnergyConsistency(merged->EPI(), hitAnode->EPI())) {
      return;
    }
  }
  
  merged->setEnergy(merged->EPI());
  hitReconstructed.push_back(merged);
}

void RealDetectorUnit2DStrip::
reconstructWithOneByOneMethod(const DetectorHitVector& hitsCathode,
                              const DetectorHitVector& hitsAnode,
                              DetectorHitVector& hitReconstructed)
{
  if (hitsCathode.size()==1 && hitsAnode.size()==1) {
    DetectorHit_sptr hitCathode = hitsCathode[0];
    DetectorHit_sptr hitAnode = hitsAnode[0];
    DetectorHit_sptr merged = mergeDoubleSides(hitCathode, hitAnode);

    if (isEnergyConsistencyRequired()) {
      if (!checkEnergyConsistency(merged->EPI(), hitCathode->EPI())) {
        return;
      }
      if (!checkEnergyConsistency(merged->EPI(), hitAnode->EPI())) {
        return;
      }
    }
    
    merged->setEnergy(merged->EPI());
    hitReconstructed.push_back(merged);
  }
}

bool RealDetectorUnit2DStrip::
checkEnergyConsistency(double energy_x, double energy_y)
{
  const double x = energy_x;
  const double y = energy_y;
  const double fl0 = LowerEnergyConsistencyCheckFunctionC0();
  const double fl1 = LowerEnergyConsistencyCheckFunctionC1();
  const double fu0 = UpperEnergyConsistencyCheckFunctionC0();
  const double fu1 = UpperEnergyConsistencyCheckFunctionC1();
  const double y_lower = fl0 + fl1*x;
  const double y_upper = fu0 + fu1*x;
  if (y_lower<= y && y<=y_upper) {
    return true;
  }
  return false;
}

DetectorHit_sptr RealDetectorUnit2DStrip::
mergeDoubleSides(const DetectorHit_sptr hitCathode,
                 const DetectorHit_sptr hitAnode) const
{
  PixelID spCathode = hitCathode->Pixel();
  PixelID spAnode = hitAnode->Pixel();
  PixelID pixel = spCathode.combine(spAnode);
  vector3_t position = Position(pixel);
  vector3_t localPosition = LocalPosition(pixel);
  DetectorHit_sptr hit(new DetectorHit((PrioritySide()==ElectrodeSide::Anode) ?
                                       (*hitAnode) : 
                                       (*hitCathode)));
  hit->setPosition(position);
  hit->setLocalPosition(localPosition);
  hit->setPixel(pixel);
  return hit;
}

} /* namespace comptonsoft */
