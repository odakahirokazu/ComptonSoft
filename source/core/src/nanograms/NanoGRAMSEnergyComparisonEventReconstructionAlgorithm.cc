/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 *************************************************************************/

#include "NanoGRAMSEnergyComparisonEventReconstructionAlgorithm.hh"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "AstroUnits.hh"
#include "ComptonConstraints.hh"
#include "DetectorHit.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

namespace
{

std::array<double, NUM_VATA> readEnergyCorrectionFactors(
    boost::property_tree::ptree& pt)
{
  std::array<double, NUM_VATA> factors{};
  factors.fill(1.0);
  auto node = pt.get_child_optional("energy_correction_factor");
  if (!node) {
    node = pt.get_child_optional("energy_correction_factors");
  }
  if (!node) {
    return factors;
  }

  int index = 0;
  for (const auto& item : *node) {
    const double factor = item.second.get_value<double>();
    if (index >= NUM_VATA || factor <= 0.0) {
      throw std::runtime_error(
          "energy_correction_factor must contain 4 positive values.");
    }
    factors[index++] = factor;
  }
  if (index != NUM_VATA) {
    throw std::runtime_error(
        "energy_correction_factor must contain 4 positive values.");
  }
  return factors;
}

std::array<double, NUM_VATA> readEnergyCorrectionFactors(YAML::Node& node)
{
  std::array<double, NUM_VATA> factors{};
  factors.fill(1.0);
  YAML::Node factor_node = node["energy_correction_factor"];
  if (!factor_node) {
    factor_node = node["energy_correction_factors"];
  }
  if (!factor_node) {
    return factors;
  }
  if (!factor_node.IsSequence() || factor_node.size() != NUM_VATA) {
    throw std::runtime_error(
        "energy_correction_factor must contain 4 positive values.");
  }
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    factors[fec] = factor_node[fec].as<double>();
    if (factors[fec] <= 0.0) {
      throw std::runtime_error(
          "energy_correction_factor must contain 4 positive values.");
    }
  }
  return factors;
}

} // namespace

NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::
NanoGRAMSEnergyComparisonEventReconstructionAlgorithm()
{
  energy_correction_factors_.fill(1.0);
}

NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::
~NanoGRAMSEnergyComparisonEventReconstructionAlgorithm() = default;

bool NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::
loadParameters(boost::property_tree::ptree& pt)
{
  known_initial_gammaray_energy_ =
      pt.get<double>("known_initial_gammaray_energy") * unit::keV;
  required_minimum_energy_deposit_in_higher_hit_ =
      pt.get<double>("required_minimum_energy_deposit_in_higher_hit", 0.0) *
      unit::keV;
  energy_tolerance_sigma_ =
      pt.get<double>("energy_tolerance_sigma_keV", 0.0) * unit::keV;
  energy_correction_factors_ = readEnergyCorrectionFactors(pt);
  printParameters();
  return known_initial_gammaray_energy_ > 0.0;
}

bool NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::
loadParametersYAML(YAML::Node& node)
{
  known_initial_gammaray_energy_ =
      node["known_initial_gammaray_energy"].as<double>() * unit::keV;
  required_minimum_energy_deposit_in_higher_hit_ =
      node["required_minimum_energy_deposit_in_higher_hit"].as<double>(0.0) *
      unit::keV;
  if (node["energy_tolerance_sigma_keV"]) {
    energy_tolerance_sigma_ =
        node["energy_tolerance_sigma_keV"].as<double>() * unit::keV;
  }
  energy_correction_factors_ = readEnergyCorrectionFactors(node);
  printParameters();
  return known_initial_gammaray_energy_ > 0.0;
}

void NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::initializeEvent()
{
}

bool NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  if (hits.size() != 2 || known_initial_gammaray_energy_ <= 0.0) {
    return false;
  }

  const std::vector<DetectorHit_sptr> corrected_hits = correctedHits(hits);
  if (required_minimum_energy_deposit_in_higher_hit_ > 0.0 &&
      corrected_hits[0]->Energy() <= required_minimum_energy_deposit_in_higher_hit_ &&
      corrected_hits[1]->Energy() <= required_minimum_energy_deposit_in_higher_hit_) {
    return false;
  }
  const int first_hit_index = corrected_hits[0]->Energy() >= corrected_hits[1]->Energy()
      ? 0 : 1;
  bool escape = false;
  if (!isPhysicalOrder(corrected_hits, first_hit_index, escape)) {
    return false;
  }

  const int second_hit_index = 1 - first_hit_index;
  auto event = std::make_shared<BasicComptonEvent>(baseEvent);
  event->setHit1(0, corrected_hits[first_hit_index]);
  event->setHit2(1, corrected_hits[second_hit_index]);
  event->setHit2Energy(
      known_initial_gammaray_energy_ - corrected_hits[first_hit_index]->Energy());
  event->setNumberOfHits(2);
  event->setEscapeFlag(escape);
  event->setTotalEnergyDeposit(total_energy_deposits(corrected_hits));
  event->setLikelihood(1.0);
  event->setReconstructionFraction(1.0);
  event->setReconstructedOrder(first_hit_index);
  eventsReconstructed.push_back(std::move(event));
  return true;
}

bool NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::
isPhysicalOrder(const std::vector<DetectorHit_sptr>& hits,
                int first_hit_index, bool& escape) const
{
  const DetectorHit_sptr& first_hit = hits[first_hit_index];
  const DetectorHit_sptr& second_hit = hits[1 - first_hit_index];
  if (first_hit->Energy() <= 0.0 || second_hit->Energy() <= 0.0) {
    return false;
  }

  const double energy_margin = std::hypot(
      std::hypot(std::abs(first_hit->EnergyError()),
                 std::abs(second_hit->EnergyError())),
      energy_tolerance_sigma_);
  if (!compton_constraints::checkEdepIsPhysicallyAcceptable(
          known_initial_gammaray_energy_, first_hit->Energy(), energy_margin)) {
    return false;
  }

  const double energy_after_first =
      known_initial_gammaray_energy_ - first_hit->Energy();
  if (energy_after_first <= 0.0) {
    return false;
  }
  if (second_hit->Energy() > energy_after_first + energy_margin) {
    return false;
  }

  const double energy_after_second = energy_after_first - second_hit->Energy();
  escape = energy_after_second > energy_margin;
  return !escape || compton_constraints::checkEdepIsPhysicallyAcceptable(
                        energy_after_first, second_hit->Energy(), energy_margin);
}

std::vector<DetectorHit_sptr> NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::
correctedHits(const std::vector<DetectorHit_sptr>& hits) const
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

double NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::
energyCorrectionFactor(const DetectorHit_sptr& hit) const
{
  const int fec = hit->ReadoutModuleID();
  if (fec < 0 || fec >= NUM_VATA) {
    throw std::runtime_error("NanoGRAMS hit has an invalid readout FEC ID.");
  }
  return energy_correction_factors_[fec];
}

void NanoGRAMSEnergyComparisonEventReconstructionAlgorithm::printParameters() const
{
  std::cout << "--- NanoGRAMS energy comparison reconstruction ---\n"
            << "known_initial_gammaray_energy: "
            << known_initial_gammaray_energy_ / unit::keV << " keV\n"
            << "required_minimum_energy_deposit_in_higher_hit: "
            << required_minimum_energy_deposit_in_higher_hit_ / unit::keV << " keV\n"
            << "energy_tolerance_sigma_keV: "
            << energy_tolerance_sigma_ / unit::keV << "\n";
}

} /* namespace comptonsoft */
