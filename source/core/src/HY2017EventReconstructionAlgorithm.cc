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

#include "HY2017EventReconstructionAlgorithm.hh"
#include <algorithm>
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "ComptonConstraints.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

using namespace compton_constraints;

HY2017EventReconstructionAlgorithm::HY2017EventReconstructionAlgorithm()
  : energy_margin_for_checkEdepIsPhysicallyAcceptable_(0.0),
    energy_margin_for_checkScatteringAngle_(20.0)
{
}

HY2017EventReconstructionAlgorithm::~HY2017EventReconstructionAlgorithm() = default;

void HY2017EventReconstructionAlgorithm::initializeEvent()
{
}

bool HY2017EventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  setTotalEnergyDepositsAndNumHits(hits);

  if (num_hits_ > MaxHits() || num_hits_< 2) {
    return false;
  }

  bool result = reconstructFullDepositEvent(hits, baseEvent, eventsReconstructed);
  if (result == true) {
    normalizeReconstructionFraction(eventsReconstructed);
    return result;
  }

  result = reconstructEscapeEvent(hits, baseEvent, eventsReconstructed);
  if (result == true) {
    normalizeReconstructionFraction(eventsReconstructed);
  }

  return result;
}

bool HY2017EventReconstructionAlgorithm::
reconstructFullDepositEvent(const std::vector<DetectorHit_sptr>& hits,
                            const BasicComptonEvent& baseEvent,
                            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  bool result = false;

  std::vector<int> scattering_order(num_hits_);
  for (int i_hit = 0; i_hit < num_hits_; ++i_hit){
    scattering_order[i_hit] = i_hit;
  }

  int reconstructedOrder = 0;
  do {
    auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);

    std::vector<DetectorHit_sptr> ordered_hits(num_hits_);
    for(int i_hit = 0; i_hit < num_hits_; ++i_hit){
      ordered_hits[i_hit] = hits[ scattering_order[i_hit] ];
    }

    bool this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, total_energy_deposits_);
    if (this_result) {
      eventReconstructed->setReconstructedOrder(reconstructedOrder);
      eventsReconstructed.push_back(eventReconstructed);
      result = this_result;
    }
    
    ++reconstructedOrder;
  } while (std::next_permutation(scattering_order.begin(), scattering_order.end()));

  return result;
}

bool HY2017EventReconstructionAlgorithm::
reconstructEscapeEvent(const std::vector<DetectorHit_sptr>& hits,
                       const BasicComptonEvent& baseEvent,
                       std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  bool result = false;

  std::vector<int> scattering_order(num_hits_);
  for (int i_hit = 0; i_hit < num_hits_; ++i_hit) {
    scattering_order[i_hit] = i_hit;
  }

  int reconstructedOrder = 0;
  do {
    auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);

    std::vector<DetectorHit_sptr> ordered_hits(num_hits_);
    for (int i_hit = 0; i_hit < num_hits_; ++i_hit) {
      ordered_hits[i_hit] = hits[ scattering_order[i_hit] ];
    }

    double escaped_energy = 0.0;
    bool this_result = estimateEscapedEnergy(ordered_hits, escaped_energy);
    if (!this_result) { continue; }
   
    const double corrected_total_energy = total_energy_deposits_ + escaped_energy;
    this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, corrected_total_energy, true);
    if (this_result) {
      eventReconstructed->setReconstructedOrder(reconstructedOrder);
      eventsReconstructed.push_back(eventReconstructed);
      result = this_result;
    }

    ++reconstructedOrder;
  } while (std::next_permutation(scattering_order.begin(), scattering_order.end()));

  return result;
}

bool HY2017EventReconstructionAlgorithm::
reconstructOrderedHits(const std::vector<DetectorHit_sptr>& ordered_hits,
                       BasicComptonEvent& eventReconstructed,
                       double incident_energy,
                       bool is_escape_event)
{
  for (int i_hit = 0; i_hit < num_hits_ - 1; ++i_hit) {
    double gammaray_energy_before_ihit = incident_energy;
    for(int j_hit = 0; j_hit < i_hit; ++j_hit){
      gammaray_energy_before_ihit -= ordered_hits[j_hit]->Energy();
    }
    bool is_this_edep_acceptable = checkEdepIsPhysicallyAcceptable(gammaray_energy_before_ihit, 
                                                                   ordered_hits[i_hit]->Energy(),
                                                                   energy_margin_for_checkEdepIsPhysicallyAcceptable_);
    if (!is_this_edep_acceptable) { return false; }
  }

  for (int i_hit = 2; i_hit < num_hits_; ++i_hit) {
    double gammaray_energy_before_im1hit = incident_energy;
    for (int j_hit = 0; j_hit < i_hit - 1; ++j_hit) {
      gammaray_energy_before_im1hit -= ordered_hits[j_hit]->Energy();
    }
    const bool is_this_scattering_acceptable = checkScatteringAngle(ordered_hits[i_hit - 2], 
                                                                    ordered_hits[i_hit - 1],
                                                                    ordered_hits[i_hit],
                                                                    gammaray_energy_before_im1hit,
                                                                    energy_margin_for_checkScatteringAngle_);
    if (!is_this_scattering_acceptable) { return false; }
  }

  eventReconstructed.setHit1(0, ordered_hits[0]);
  eventReconstructed.setHit2(1, ordered_hits[1]);
  eventReconstructed.setHit2Energy(incident_energy - ordered_hits[0]->Energy());
  eventReconstructed.setNumberOfHits(num_hits_);
  eventReconstructed.setEscapeFlag(is_escape_event);
  eventReconstructed.setTotalEnergyDeposit(total_energy_deposits_);

  const double this_reconstruction_fraction = calReconstructionFraction(ordered_hits, incident_energy, is_escape_event);
  eventReconstructed.setReconstructionFraction(this_reconstruction_fraction);

  return true;
}

double HY2017EventReconstructionAlgorithm::
calReconstructionFraction(const std::vector<DetectorHit_sptr>& /* ordered_hits */,
                          double /* incident_energy */,
                          bool /* is_escape_event */)
{
  //to be determined
  return 1.0;
}

void HY2017EventReconstructionAlgorithm::setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits)
{
  total_energy_deposits_ = total_energy_deposits(hits);
  num_hits_ = hits.size();
}

void HY2017EventReconstructionAlgorithm::normalizeReconstructionFraction(std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  double sum_reconstruction_fraction = 0.0;
  for (std::size_t i = 0; i < eventsReconstructed.size(); ++i) {
    sum_reconstruction_fraction += eventsReconstructed[i]->ReconstructionFraction();
  }
  for(std::size_t i = 0; i < eventsReconstructed.size(); ++i) {
    eventsReconstructed[i]->setReconstructionFraction(eventsReconstructed[i]->ReconstructionFraction() / sum_reconstruction_fraction);
  }
}

} /* namespace comptonsoft */
