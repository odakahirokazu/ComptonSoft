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

#include "StandardEventReconstructionAlgorithm.hh"
#include <list>
#include <algorithm>
#include <numeric>
#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

StandardEventReconstructionAlgorithm::StandardEventReconstructionAlgorithm()
  : fluorescenceCut_(false)
{
}

StandardEventReconstructionAlgorithm::~StandardEventReconstructionAlgorithm() = default;

void StandardEventReconstructionAlgorithm::initializeEvent()
{
  setFluorescenceCut(false);
}

bool StandardEventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  const int NumHits = hits.size();
  if (NumHits>MaxHits()) {
    return false;
  }

  bool result = false;
  auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);
  eventReconstructed->setTotalEnergyDeposit(total_energy_deposits(hits));

  if (NumHits==2) {
    const bool hit0Si = hits[0]->isFlags(flag::LowZHit);
    const bool hit1Si = hits[1]->isFlags(flag::LowZHit);
    const bool hit0CdTe = hits[0]->isFlags(flag::HighZHit);
    const bool hit1CdTe = hits[1]->isFlags(flag::HighZHit);

    if (hit0Si && hit1Si) {
      result = reconstruct2HitsSameMaterial(hits, *eventReconstructed);
    }
    else if (hit0CdTe && hit1CdTe) {
      result = reconstruct2HitsSameMaterial(hits, *eventReconstructed);
    }
    else if (hit0Si && hit1CdTe) {
      result = reconstruct2HitsSiCdTe(hits, *eventReconstructed);
    }
    else if (hit0CdTe && hit1Si) {
      std::vector<DetectorHit_sptr> hitsOrdered(2);
      hitsOrdered[0] = hits[1];
      hitsOrdered[1] = hits[0];
      result = reconstruct2HitsSiCdTe(hitsOrdered, *eventReconstructed);
    }
  }
  else if (NumHits==3) {
    const bool hit0Si = hits[0]->isFlags(flag::LowZHit);
    const bool hit1Si = hits[1]->isFlags(flag::LowZHit);
    const bool hit2Si = hits[2]->isFlags(flag::LowZHit);
    const bool hit0CdTe = hits[0]->isFlags(flag::HighZHit);
    const bool hit1CdTe = hits[1]->isFlags(flag::HighZHit);
    const bool hit2CdTe = hits[2]->isFlags(flag::HighZHit);

    if (hit0Si && hit1Si && hit2CdTe) {
      result = reconstruct3HitsSiSiCdTe(hits, *eventReconstructed);
    }
    else if (hit0Si && hit1CdTe && hit2Si) {
      std::vector<DetectorHit_sptr> hitsOrdered(3);
      hitsOrdered[0] = hits[0];
      hitsOrdered[1] = hits[2];
      hitsOrdered[2] = hits[1];
      result = reconstruct3HitsSiSiCdTe(hitsOrdered, *eventReconstructed);
    }
    else if (hit0CdTe && hit1Si && hit2Si) {
      std::vector<DetectorHit_sptr> hitsOrdered(3);
      hitsOrdered[0] = hits[1];
      hitsOrdered[1] = hits[2];
      hitsOrdered[2] = hits[0];
      result = reconstruct3HitsSiSiCdTe(hitsOrdered, *eventReconstructed);
    }
    else if (hit0Si && hit1CdTe && hit2CdTe) {
      result = reconstruct3HitsSiCdTeCdTeByFOM(hits, *eventReconstructed);
    }
    else if (hit0CdTe && hit1Si && hit2CdTe) {
      std::vector<DetectorHit_sptr> hitsOrdered(3);
      hitsOrdered[0] = hits[1];
      hitsOrdered[1] = hits[0];
      hitsOrdered[2] = hits[2];
      result = reconstruct3HitsSiCdTeCdTeByFOM(hitsOrdered, *eventReconstructed);
    }
    else if (hit0CdTe && hit1CdTe && hit2Si) {
      std::vector<DetectorHit_sptr> hitsOrdered(3);
      hitsOrdered[0] = hits[2];
      hitsOrdered[1] = hits[0];
      hitsOrdered[2] = hits[1];
      result = reconstruct3HitsSiCdTeCdTeByFOM(hitsOrdered, *eventReconstructed);
    }
    else {
      // to be implemented
      result = false;
    }
  }
  else {
    // to be implemented
    result = false;
  }

  eventsReconstructed.push_back(eventReconstructed);
  return result;
}

bool StandardEventReconstructionAlgorithm::
reconstruct2HitsSiCdTe(const std::vector<DetectorHit_sptr>& hits,
                       BasicComptonEvent& eventReconstructed)
{
  eventReconstructed.setHit1(0, hits[0]);
  eventReconstructed.setHit2(1, hits[1]);
  fluorescenceCut_ = checkFluorescence(hits[0], hits[1]);
  eventReconstructed.setNumberOfHits(2);
  return true;
}

bool StandardEventReconstructionAlgorithm::
reconstruct2HitsSameMaterial(const std::vector<DetectorHit_sptr>& hits,
                             BasicComptonEvent& eventReconstructed)
{
  eventReconstructed.setHit1(0, hits[0]);
  eventReconstructed.setHit2(1, hits[1]);

  if (eventReconstructed.IncidentEnergy() < 0.5*CLHEP::electron_mass_c2) {
    if (!eventReconstructed.EnergyOrder()) {
      eventReconstructed.swap();
    }
  }
  else {
    const double dtheta1 = eventReconstructed.DeltaTheta();
    eventReconstructed.swap();
    const double dtheta2 = eventReconstructed.DeltaTheta();
    if (std::abs(dtheta1) < std::abs(dtheta2)) {
      eventReconstructed.swap();
    }
  }

  if (hits[0]->isFlags(flag::HighZHit)) {
    const int ComptonHitID = eventReconstructed.Hit1ID();
    const int AbsorptionHitID = eventReconstructed.Hit2ID();
    fluorescenceCut_ = checkFluorescence(hits[ComptonHitID],
                                         hits[AbsorptionHitID]);
  }
  else {
    fluorescenceCut_ = false;
  }

  eventReconstructed.setNumberOfHits(2);
  return true;
}

bool StandardEventReconstructionAlgorithm::
reconstruct3HitsSiSiCdTe(const std::vector<DetectorHit_sptr>& hits,
                         BasicComptonEvent& eventReconstructed)
{
  if (checkFluorescence(hits[0], hits[2])) {
    // 0: Fl, 1: Comp, 2: Abs
    eventReconstructed.setHit1(1, hits[1]);
    eventReconstructed.setHit2(2, hits[2]);
    eventReconstructed.setHit2Energy(hits[2]->Energy()+hits[0]->Energy());
  }
  else if (checkFluorescence(hits[1], hits[2])) {
    // 0: Comp, 1: Fl, 2: Abs
    eventReconstructed.setHit1(0, hits[0]);
    eventReconstructed.setHit2(2, hits[2]);
    eventReconstructed.setHit2Energy(hits[2]->Energy()+hits[1]->Energy());
  }
  else {
    // 0: Comp1, 1: Comp2, 2: Abs
    eventReconstructed.setHit1(0, hits[0]);
    eventReconstructed.setHit2(1, hits[1]);
    eventReconstructed.setHit2Energy(hits[1]->Energy()+hits[2]->Energy());
    const double dtheta1 = eventReconstructed.DeltaTheta();
    
    // 0: Comp2, 1: Comp1, 2: Abs
    BasicComptonEvent event2;
    event2.setHit1(1, hits[1]);
    event2.setHit2(0, hits[0]);
    event2.setHit2Energy(hits[0]->Energy()+hits[2]->Energy());
    const double dtheta2 = event2.DeltaTheta();

    if (std::abs(dtheta2) < std::abs(dtheta1)) {
      eventReconstructed = event2;
    }
  }

  eventReconstructed.setNumberOfHits(3);
  return true;
}

bool StandardEventReconstructionAlgorithm::
reconstruct3HitsSiCdTeCdTe(const std::vector<DetectorHit_sptr>& hits,
                           BasicComptonEvent& eventReconstructed)
{
  if (checkFluorescence(hits[1], hits[2])) {
    // 0: Comp, 1: Fl, 2: Abs
    eventReconstructed.setHit1(0, hits[0]);
    eventReconstructed.setHit2(2, hits[2]);
    eventReconstructed.setHit2Energy(hits[2]->Energy()+hits[1]->Energy());
  }
  else if (checkFluorescence(hits[2], hits[1])) {
    // 0: Comp, 1: Abs, 2: Fl
    eventReconstructed.setHit1(0, hits[0]);
    eventReconstructed.setHit2(1, hits[1]);
    eventReconstructed.setHit2Energy(hits[1]->Energy()+hits[2]->Energy());
  }
  else {
    // 0: Comp1, 1: Comp2, 2: Abs
    eventReconstructed.setHit1(0, hits[0]);
    eventReconstructed.setHit2(1, hits[1]);
    eventReconstructed.setHit2Energy(hits[1]->Energy()+hits[2]->Energy());
    const double dtheta1 = eventReconstructed.DeltaTheta();

    // 0: Comp1, 1: Abs, 2: Comp2
    BasicComptonEvent event2;
    event2.setHit1(0, hits[0]);
    event2.setHit2(2, hits[2]);
    event2.setHit2Energy(hits[2]->Energy()+hits[1]->Energy());
    const double dtheta2 = event2.DeltaTheta();

    if (std::abs(dtheta2) < std::abs(dtheta1)) {
      eventReconstructed = event2;
    }
  }

  eventReconstructed.setNumberOfHits(3);
  return true;
}

bool StandardEventReconstructionAlgorithm::
reconstruct3HitsSiCdTeCdTeByFOM(const std::vector<DetectorHit_sptr>& hits,
                                BasicComptonEvent& eventReconstructed)
{
  if (checkFluorescence(hits[1], hits[2])) {
    // 0: Comp, 1: Fl, 2: Abs
    eventReconstructed.setHit1(0, hits[0]);
    eventReconstructed.setHit2(2, hits[2]);
    eventReconstructed.setHit2Energy(hits[2]->Energy()+hits[1]->Energy());
  }
  else if (checkFluorescence(hits[2], hits[1])) {
    // 0: Comp, 1: Abs, 2: Fl
    eventReconstructed.setHit1(0, hits[0]);
    eventReconstructed.setHit2(1, hits[1]);
    eventReconstructed.setHit2Energy(hits[1]->Energy()+hits[2]->Energy());
  }
  else {
    // 0: Comp1, 1: Comp2, 2: Abs
    BasicComptonEvent compton12;
    compton12.setSourcePosition(hits[0]->Position());
    compton12.setHit1(1, hits[1]);
    compton12.setHit2(2, hits[2]);
    const double dtheta12 = compton12.DeltaTheta();

    // 0: Comp1, 1: Abs, 2: Comp2
    BasicComptonEvent compton21;
    compton21.setSourcePosition(hits[0]->Position());
    compton21.setHit1(2, hits[2]);
    compton21.setHit2(1, hits[1]);
    const double dtheta21 = compton21.DeltaTheta();
    
    if (std::abs(dtheta12) <= std::abs(dtheta21)) {
      // 0: Comp1, 1: Comp2, 2: Abs
      eventReconstructed.setHit1(0, hits[0]);
      eventReconstructed.setHit2(1, hits[1]);
      eventReconstructed.setHit2Energy(hits[1]->Energy()+hits[2]->Energy());
    }
    else {
      // 0: Comp1, 1: Abs, 2: Comp2
      eventReconstructed.setHit1(0, hits[0]);
      eventReconstructed.setHit2(2, hits[2]);
      eventReconstructed.setHit2Energy(hits[2]->Energy()+hits[1]->Energy());
    }
  }

  eventReconstructed.setNumberOfHits(3);
  return true;
}

bool StandardEventReconstructionAlgorithm::
checkFluorescence(DetectorHit_sptr fluorescence,
                  DetectorHit_sptr absorption) const
{
  const double FluorCutLengthSiCdTe = 10.0 * unit::mm;
  const double FluorCutLengthCdTeCdTe = 7.0 * unit::mm;
  
  if (fluorescence->isFlags(flag::FluorescenceHit)) {
    if (fluorescence->isFlags(flag::LowZHit) && absorption->isFlags(flag::HighZHit)) {
      if (fluorescence->distance(*absorption) < FluorCutLengthSiCdTe) {
        return true;
      }
    }
    else if (fluorescence->isFlags(flag::HighZHit) && absorption->isFlags(flag::HighZHit)) {
      if (fluorescence->distance(*absorption) < FluorCutLengthCdTeCdTe) {
        return true;
      }
    }
  }

  return false;
}

bool StandardEventReconstructionAlgorithm::
extractEventsByTotalEnergy(const std::vector<DetectorHit_sptr>& hits,
                           double energy, double deltaE, int maxHits,
                           std::vector<DetectorHit_sptr>& output)
{
  const int NumHits = hits.size();
  if (NumHits > maxHits) {
    return false;
  }

  const double totalE = std::accumulate(hits.begin(), hits.end(), 0.0,
                                        [](double energy, DetectorHit_sptr hit) {
                                          return energy + hit->Energy();
                                        });
  if (totalE < energy-deltaE) { return false; }
  if (totalE <= energy+deltaE) {
    output = hits;
    return true;
  }

  // generate all hit patterns.
  std::vector<std::list<DetectorHit_sptr>> hitLists;
  hitLists.push_back(std::list<DetectorHit_sptr>(0));
  for (int hitIndex=0; hitIndex<NumHits; hitIndex++) {
    const auto EndOfList = hitLists.end();
    for (auto itList=hitLists.begin(); itList!=EndOfList; ++itList) {
      std::list<DetectorHit_sptr> newList = *itList;
      newList.push_back(hits[hitIndex]);
      hitLists.push_back(std::move(newList));
    }
  }

  // calcualte sum of energies.
  const int NumLists = hitLists.size();
  std::vector<double> deltaEVector(NumLists, 0.0);
  for (int i=0; i<NumLists; i++) {
    auto& hitList = hitLists[i];
    const double sumE = std::accumulate(hitList.begin(), hitList.end(), 0.0,
                                        [](double energy, DetectorHit_sptr hit) {
                                          return energy + hit->Energy();
                                        });
    deltaEVector[i] = std::abs(sumE-energy);
  }
  
  auto iterSelected = std::min_element(deltaEVector.begin(), deltaEVector.end());
  if (*iterSelected>deltaE) { return false; }

  int indexSelected = iterSelected - deltaEVector.begin();
  auto& hitListSelected = hitLists[indexSelected];
  std::copy(hitListSelected.begin(), hitListSelected.end(), output.begin());
  return true;
}

} /* namespace comptonsoft */
