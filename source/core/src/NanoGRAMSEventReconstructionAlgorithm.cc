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

#include "NanoGRAMSEventReconstructionAlgorithm.hh"
#include <algorithm>
#include <stdexcept>
#include <utility>
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "ComptonConstraints.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

using namespace compton_constraints;

namespace
{

std::array<double, NUM_VATA> defaultEnergyCorrectionFactors()
{
  std::array<double, NUM_VATA> factors{};
  factors.fill(1.0);
  return factors;
}

void validateEnergyCorrectionFactors(const std::array<double, NUM_VATA>& factors)
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    if (factors[fec] <= 0.0) {
      throw std::runtime_error("energy_correction_factor values must be positive.");
    }
  }
}

void printEnergyCorrectionFactors(const std::array<double, NUM_VATA>& factors)
{
  std::cout << "energy_correction_factor: [ ";
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    if (fec != 0) {
      std::cout << ", ";
    }
    std::cout << factors[fec];
  }
  std::cout << " ]" << std::endl;
}

void printRequiredMinimumEnergyDepositInHigherHit(double threshold)
{
  std::cout << "required_minimum_energy_deposit_in_higher_hit: "
            << threshold / unit::keV << " keV" << std::endl;
}

std::array<double, NUM_VATA>
readEnergyCorrectionFactors(boost::property_tree::ptree& pt)
{
  std::array<double, NUM_VATA> factors = defaultEnergyCorrectionFactors();
  auto node = pt.get_child_optional("energy_correction_factor");
  if (!node) {
    node = pt.get_child_optional("energy_correction_factors");
  }
  if (!node) {
    return factors;
  }
  if (node->empty()) {
    throw std::runtime_error("energy_correction_factor must be a length-4 array.");
  }

  int index = 0;
  for (const auto& item : *node) {
    if (index >= NUM_VATA) {
      throw std::runtime_error("energy_correction_factor must contain exactly 4 values.");
    }
    factors[index] = item.second.get_value<double>();
    ++index;
  }
  if (index != NUM_VATA) {
    throw std::runtime_error("energy_correction_factor must contain exactly 4 values.");
  }

  validateEnergyCorrectionFactors(factors);
  return factors;
}

#if CS_USE_YAMLCPP
std::array<double, NUM_VATA>
readEnergyCorrectionFactors(YAML::Node& node)
{
  std::array<double, NUM_VATA> factors = defaultEnergyCorrectionFactors();
  YAML::Node factor_node = node["energy_correction_factor"];
  if (!factor_node) {
    factor_node = node["energy_correction_factors"];
  }
  if (!factor_node) {
    return factors;
  }
  if (!factor_node.IsSequence()) {
    throw std::runtime_error("energy_correction_factor must be a length-4 array.");
  }

  const std::vector<double> values = factor_node.as<std::vector<double>>();
  if (values.size() != NUM_VATA) {
    throw std::runtime_error("energy_correction_factor must contain exactly 4 values.");
  }
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    factors[fec] = values[fec];
  }

  validateEnergyCorrectionFactors(factors);
  return factors;
}
#endif /* CS_USE_YAMLCPP */

} // namespace

NanoGRAMSEventReconstructionAlgorithm::NanoGRAMSEventReconstructionAlgorithm()
    : is_escape_event_(true)
{
  energy_correction_factors_.fill(1.0);
}

NanoGRAMSEventReconstructionAlgorithm::~NanoGRAMSEventReconstructionAlgorithm() = default;

bool NanoGRAMSEventReconstructionAlgorithm::loadParameters(boost::property_tree::ptree& pt)
{
    incident_energy_candidates_.clear();
    energy_correction_factors_ = readEnergyCorrectionFactors(pt);
    required_minimum_energy_deposit_in_higher_hit_ =
        pt.get<double>("required_minimum_energy_deposit_in_higher_hit", 0.0) *
        unit::keV;

    for (const auto& item : pt.get_child("incident_energy_candidates")) {
        incident_energy_candidates_.push_back(item.second.get_value<double>() * unit::keV);
    }

    std::cout << "incident_energy_candidates:" << std::endl;;
    for (double e : incident_energy_candidates_) {
        std::cout << e / unit::keV << " keV" << std::endl;
    }
    printEnergyCorrectionFactors(energy_correction_factors_);
    printRequiredMinimumEnergyDepositInHigherHit(
        required_minimum_energy_deposit_in_higher_hit_);
    std::cout << std::endl;;

    return true;
}

#if CS_USE_YAMLCPP
bool NanoGRAMSEventReconstructionAlgorithm::loadParametersYAML(YAML::Node& node)
{
    incident_energy_candidates_.clear();
    energy_correction_factors_ = readEnergyCorrectionFactors(node);
    required_minimum_energy_deposit_in_higher_hit_ = 0.0 * unit::keV;
    if (node["required_minimum_energy_deposit_in_higher_hit"]) {
      required_minimum_energy_deposit_in_higher_hit_ =
          node["required_minimum_energy_deposit_in_higher_hit"].as<double>() *
          unit::keV;
    }

    std::vector<double> energy_values = node["incident_energy_candidates"].as<std::vector<double>>();

    for (const auto& energy_keV : energy_values) {
        incident_energy_candidates_.push_back(energy_keV * unit::keV);
    }

    std::cout << "incident_energy_candidates:" << std::endl;;
    for (double e : incident_energy_candidates_) {
        std::cout << e / unit::keV << " keV" << std::endl;
    }
    printEnergyCorrectionFactors(energy_correction_factors_);
    printRequiredMinimumEnergyDepositInHigherHit(
        required_minimum_energy_deposit_in_higher_hit_);
    std::cout << std::endl;;

    return true;
}
#endif /* CS_USE_YAMLCPP */

void NanoGRAMSEventReconstructionAlgorithm::initializeEvent()
{
}

bool NanoGRAMSEventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  const std::vector<DetectorHit_sptr> corrected_hits = correctedHits(hits);
  setTotalEnergyDepositsAndNumHits(corrected_hits);

  bool result = false;
  if (num_hits_ != 2) {
      return false;
  } else{
      result = reconstruct2HitEvent(corrected_hits, baseEvent, eventsReconstructed);
  }

  if (result == true) {
    if(selecting_most_likely_order_){
        selectLeastARMOrder(eventsReconstructed);
    }
    return true;
  } else{
    return false;
  }
}


void NanoGRAMSEventReconstructionAlgorithm::selectLeastARMOrder(std::vector<BasicComptonEvent_sptr>& eventsReconstructed){
  using event_t = BasicComptonEvent_sptr;
  const event_t event_yielding_least_arm =
    *std::min_element(eventsReconstructed.begin(),
                      eventsReconstructed.end(),
                      [](const event_t& a, const event_t& b) {
                        return std::abs(a->DeltaTheta()) < std::abs(b->DeltaTheta());
                      });
  eventsReconstructed.clear();
  eventsReconstructed.push_back(event_yielding_least_arm);
}

bool NanoGRAMSEventReconstructionAlgorithm::
reconstruct2HitEvent(const std::vector<DetectorHit_sptr>& hits,
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
    for(int i_hit=0; i_hit<num_hits_; ++i_hit){
      ordered_hits[i_hit] = hits[ scattering_order[i_hit] ];
    }

    for(const auto& e0:incident_energy_candidates_){
        bool this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, e0);
        if (this_result) {
          eventReconstructed->setReconstructedOrder(reconstructedOrder);
          eventsReconstructed.push_back(eventReconstructed);
          result = this_result;
        }
    }
        
    ++reconstructedOrder;
  } while (std::next_permutation(scattering_order.begin(), scattering_order.end()));

  return result;
}



bool NanoGRAMSEventReconstructionAlgorithm::
reconstructOrderedHits(const std::vector<DetectorHit_sptr>& ordered_hits,
                       BasicComptonEvent& eventReconstructed,
                       double incident_energy)
{
  if (!hasRequiredHigherHitEnergy(ordered_hits)) {
    return false;
  }

  bool result_kinematics = isSatisfyKinematics(ordered_hits, incident_energy);
  if(!result_kinematics){
      return false;
  }

  eventReconstructed.setHit1(0, ordered_hits[0]);
  eventReconstructed.setHit2(1, ordered_hits[1]);
  eventReconstructed.setHit2Energy(incident_energy - ordered_hits[0]->Energy());
  eventReconstructed.setNumberOfHits(num_hits_);
  eventReconstructed.setEscapeFlag(is_escape_event_);
  eventReconstructed.setTotalEnergyDeposit(total_energy_deposits_);

  return true;
}

void NanoGRAMSEventReconstructionAlgorithm::setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits)
{
  total_energy_deposits_ = total_energy_deposits(hits);
  num_hits_ = hits.size();
}

std::vector<DetectorHit_sptr>
NanoGRAMSEventReconstructionAlgorithm::correctedHits(
    const std::vector<DetectorHit_sptr>& hits) const
{
  std::vector<DetectorHit_sptr> corrected_hits;
  corrected_hits.reserve(hits.size());

  for (const auto& hit : hits) {
    const double correction_factor = energyCorrectionFactor(hit);
    DetectorHit_sptr corrected_hit = hit->clone();
    corrected_hit->setEnergy(hit->Energy() * correction_factor);
    corrected_hit->setEnergyError(hit->EnergyError() * correction_factor);
    corrected_hits.push_back(std::move(corrected_hit));
  }

  return corrected_hits;
}

double NanoGRAMSEventReconstructionAlgorithm::energyCorrectionFactor(
    const DetectorHit_sptr& hit) const
{
  const int fec = hit->ReadoutModuleID();
  if (fec < 0 || fec >= NUM_VATA) {
    throw std::runtime_error("NanoGRAMS hit has an invalid readout FEC ID.");
  }
  return energy_correction_factors_[fec];
}

bool NanoGRAMSEventReconstructionAlgorithm::hasRequiredHigherHitEnergy(
    const std::vector<DetectorHit_sptr>& ordered_hits) const
{
  if (required_minimum_energy_deposit_in_higher_hit_ <= 0.0) {
    return true;
  }
  if (ordered_hits.size() < 2) {
    return false;
  }

  const double higher_hit_energy =
      std::max(ordered_hits[0]->Energy(), ordered_hits[1]->Energy());
  return higher_hit_energy > required_minimum_energy_deposit_in_higher_hit_;
}

bool NanoGRAMSEventReconstructionAlgorithm::isSatisfyKinematics(const std::vector<DetectorHit_sptr>& ordered_hits, double incident_energy)
{
    double cosThetaK = 1.0 - CLHEP::electron_mass_c2 * (1.0/(incident_energy-ordered_hits[0]->Energy()) - 1.0/incident_energy);
    return (-1.0 < cosThetaK) && (cosThetaK < 1.0);
}


} /* namespace comptonsoft */
