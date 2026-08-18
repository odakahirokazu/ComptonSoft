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

#include "NanoGRAMSKNFormulaEventReconstructionAlgorithm.hh"

#include <algorithm>
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

constexpr int kEnergyIntegrationPoints = 32;
constexpr double kMinimumEnergyResolution = 1.0e-6 * unit::keV;
constexpr double kIntegrationSigmaRange = 6.0;

double maximumComptonEnergyDeposit(double incident_energy)
{
  return 2.0 * incident_energy * incident_energy /
         (2.0 * incident_energy + CLHEP::electron_mass_c2);
}

double maximumComptonEnergyDepositDerivative(double incident_energy)
{
  const double denominator = 2.0 * incident_energy + CLHEP::electron_mass_c2;
  return 4.0 * incident_energy *
         (incident_energy + CLHEP::electron_mass_c2) /
         (denominator * denominator);
}

double normalDensity(double value, double mean, double sigma)
{
  const double normalized = (value - mean) / sigma;
  return std::exp(-0.5 * normalized * normalized) /
         (std::sqrt(2.0 * CLHEP::pi) * sigma);
}

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

NanoGRAMSKNFormulaEventReconstructionAlgorithm::
NanoGRAMSKNFormulaEventReconstructionAlgorithm()
{
  energy_correction_factors_.fill(1.0);
}

NanoGRAMSKNFormulaEventReconstructionAlgorithm::
~NanoGRAMSKNFormulaEventReconstructionAlgorithm() = default;

bool NanoGRAMSKNFormulaEventReconstructionAlgorithm::
loadParameters(boost::property_tree::ptree& pt)
{
  known_initial_gammaray_energy_ =
      pt.get<double>("known_initial_gammaray_energy") * unit::keV;
  energy_resolution_param0_ = pt.get<double>("energy_resolution.param0", 0.0);
  energy_resolution_param1_ = pt.get<double>("energy_resolution.param1", 0.0);
  energy_resolution_param2_ = pt.get<double>("energy_resolution.param2", 0.0);
  energy_margin_multiplier_ = pt.get<double>("energy_margin_multiplier", 1.0);
  if (energy_margin_multiplier_ < 0.0) {
    throw std::runtime_error("energy_margin_multiplier must be non-negative.");
  }
  energy_correction_factors_ = readEnergyCorrectionFactors(pt);
  printParameters();
  return known_initial_gammaray_energy_ > 0.0;
}

bool NanoGRAMSKNFormulaEventReconstructionAlgorithm::
loadParametersYAML(YAML::Node& node)
{
  known_initial_gammaray_energy_ =
      node["known_initial_gammaray_energy"].as<double>() * unit::keV;
  if (const YAML::Node energy_resolution = node["energy_resolution"]) {
    energy_resolution_param0_ = energy_resolution["param0"].as<double>(0.0);
    energy_resolution_param1_ = energy_resolution["param1"].as<double>(0.0);
    energy_resolution_param2_ = energy_resolution["param2"].as<double>(0.0);
  }
  energy_margin_multiplier_ = node["energy_margin_multiplier"].as<double>(1.0);
  if (energy_margin_multiplier_ < 0.0) {
    throw std::runtime_error("energy_margin_multiplier must be non-negative.");
  }
  energy_correction_factors_ = readEnergyCorrectionFactors(node);
  printParameters();
  return known_initial_gammaray_energy_ > 0.0;
}

void NanoGRAMSKNFormulaEventReconstructionAlgorithm::initializeEvent()
{
}

bool NanoGRAMSKNFormulaEventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  if (hits.size() != 2 || known_initial_gammaray_energy_ <= 0.0) {
    return false;
  }

  const std::vector<DetectorHit_sptr> corrected_hits = correctedHits(hits);
  const Candidate order_01 = evaluateOrder(corrected_hits, 0);
  const Candidate order_10 = evaluateOrder(corrected_hits, 1);
  const Candidate best = order_01.likelihood >= order_10.likelihood
      ? order_01 : order_10;
  if (best.first_hit_index < 0) {
    return false;
  }

  const int second_hit_index = 1 - best.first_hit_index;
  auto event = std::make_shared<BasicComptonEvent>(baseEvent);
  event->setHit1(0, corrected_hits[best.first_hit_index]);
  event->setHit2(1, corrected_hits[second_hit_index]);
  event->setHit2Energy(
      known_initial_gammaray_energy_ - corrected_hits[best.first_hit_index]->Energy());
  event->setNumberOfHits(2);
  event->setEscapeFlag(best.escape);
  event->setTotalEnergyDeposit(total_energy_deposits(corrected_hits));
  event->setLikelihood(best.likelihood);
  event->setReconstructionFraction(1.0);
  event->setReconstructedOrder(best.first_hit_index);
  eventsReconstructed.push_back(std::move(event));
  return true;
}

NanoGRAMSKNFormulaEventReconstructionAlgorithm::Candidate
NanoGRAMSKNFormulaEventReconstructionAlgorithm::
evaluateOrder(const std::vector<DetectorHit_sptr>& hits, int first_hit_index) const
{
  const DetectorHit_sptr& first_hit = hits[first_hit_index];
  const DetectorHit_sptr& second_hit = hits[1 - first_hit_index];
  if (first_hit->Energy() <= 0.0 || second_hit->Energy() <= 0.0) {
    return {};
  }

  const double first_energy_sigma = energyResolution(first_hit->Energy());
  const double second_energy_sigma = energyResolution(second_hit->Energy());
  const double first_energy_margin =
      energy_margin_multiplier_ * first_energy_sigma;
  const double energy_balance_margin = energy_margin_multiplier_ * std::hypot(
      first_energy_sigma, second_energy_sigma);
  if (!compton_constraints::checkEdepIsPhysicallyAcceptable(
          known_initial_gammaray_energy_, first_hit->Energy(),
          first_energy_margin)) {
    return {};
  }

  const double energy_after_first =
      known_initial_gammaray_energy_ - first_hit->Energy();
  if (energy_after_first <= 0.0) {
    return {};
  }

  // For the second Compton edge, both E_dep,2 and E_in,2 = E0 - E_dep,1
  // are uncertain.  Propagate the latter through E_dep,max(E_in,2).
  const double second_energy_margin = energy_margin_multiplier_ * std::hypot(
      second_energy_sigma,
      maximumComptonEnergyDepositDerivative(energy_after_first) *
          first_energy_sigma);

  // A two-hit event in a small LArTPC need not be fully absorbed.  At the
  // second interaction the deposited energy must not exceed the energy left
  // after the first scattering; the remainder is allowed to escape.
  if (second_hit->Energy() > energy_after_first + energy_balance_margin) {
    return {};
  }

  const double energy_after_second = energy_after_first - second_hit->Energy();
  const bool escape = energy_after_second > energy_balance_margin;
  if (escape) {
    if (!compton_constraints::checkEdepIsPhysicallyAcceptable(
            energy_after_first, second_hit->Energy(), second_energy_margin)) {
      return {};
    }
    const double likelihood = escapeLikelihood(*first_hit, *second_hit);
    if (!std::isfinite(likelihood) || likelihood <= 0.0) {
      return {};
    }
    return {first_hit_index, likelihood, true};
  } else {
    const double likelihood = fullAbsorptionLikelihood(*first_hit, *second_hit);
    if (!std::isfinite(likelihood) || likelihood <= 0.0) {
      return {};
    }
    return {first_hit_index, likelihood, false};
  }
}

double NanoGRAMSKNFormulaEventReconstructionAlgorithm::fullAbsorptionLikelihood(
    const DetectorHit& first_hit,
    const DetectorHit& second_hit) const
{
  const double maximum_energy_deposit =
      maximumComptonEnergyDeposit(known_initial_gammaray_energy_);
  const double sigma_energy = energyResolution(first_hit.Energy());
  const double lower = std::max(
      0.0, first_hit.Energy() - kIntegrationSigmaRange * sigma_energy);
  const double upper = std::min(
      maximum_energy_deposit,
      first_hit.Energy() + kIntegrationSigmaRange * sigma_energy);
  if (!(upper > lower)) {
    return 0.0;
  }

  // With no source direction, detector boundary, or material-response model,
  // the incident direction is marginalized out.  The two measured positions
  // then carry no order-dependent first-scatter information.  Marginalize the
  // joint energy response over the finite physical Compton range instead.
  const double step = (upper - lower) / kEnergyIntegrationPoints;
  double likelihood = 0.0;
  for (int i = 0; i < kEnergyIntegrationPoints; ++i) {
    const double energy_deposit = lower + (i + 0.5) * step;
    const double cos_theta = compton_constraints::cosThetaKinematics(
        known_initial_gammaray_energy_, energy_deposit);
    likelihood += compton_constraints::normalized_differentialCrossSection(
        known_initial_gammaray_energy_, cos_theta) *
        normalDensity(first_hit.Energy(), energy_deposit,
                      energyResolution(energy_deposit)) *
        normalDensity(second_hit.Energy(),
                      known_initial_gammaray_energy_ - energy_deposit,
                      energyResolution(
                          known_initial_gammaray_energy_ - energy_deposit));
  }
  return likelihood * step;
}

double NanoGRAMSKNFormulaEventReconstructionAlgorithm::escapeLikelihood(
    const DetectorHit& first_hit,
    const DetectorHit& second_hit) const
{
  const double maximum_energy_deposit =
      maximumComptonEnergyDeposit(known_initial_gammaray_energy_);
  const double sigma_energy = energyResolution(first_hit.Energy());
  const double lower = std::max(
      0.0, first_hit.Energy() - kIntegrationSigmaRange * sigma_energy);
  const double upper = std::min(
      maximum_energy_deposit,
      first_hit.Energy() + kIntegrationSigmaRange * sigma_energy);
  if (!(upper > lower)) {
    return 0.0;
  }

  const double step = (upper - lower) / kEnergyIntegrationPoints;
  double likelihood = 0.0;
  for (int i = 0; i < kEnergyIntegrationPoints; ++i) {
    const double first_energy_deposit = lower + (i + 0.5) * step;
    const double cos_theta = compton_constraints::cosThetaKinematics(
        known_initial_gammaray_energy_, first_energy_deposit);
    const double energy_after_first =
        known_initial_gammaray_energy_ - first_energy_deposit;
    likelihood += compton_constraints::normalized_differentialCrossSection(
        known_initial_gammaray_energy_, cos_theta) *
        normalDensity(first_hit.Energy(), first_energy_deposit,
                      energyResolution(first_energy_deposit)) *
        secondScatterLikelihood(second_hit, energy_after_first);
  }
  return likelihood * step;
}

double NanoGRAMSKNFormulaEventReconstructionAlgorithm::secondScatterLikelihood(
    const DetectorHit& second_hit,
    double incident_energy) const
{
  const double maximum_energy_deposit =
      maximumComptonEnergyDeposit(incident_energy);
  const double sigma_energy = energyResolution(second_hit.Energy());
  const double lower = std::max(
      0.0, second_hit.Energy() - kIntegrationSigmaRange * sigma_energy);
  const double upper = std::min(
      maximum_energy_deposit,
      second_hit.Energy() + kIntegrationSigmaRange * sigma_energy);
  if (!(upper > lower)) {
    return 0.0;
  }

  const double step = (upper - lower) / kEnergyIntegrationPoints;
  double likelihood = 0.0;
  for (int i = 0; i < kEnergyIntegrationPoints; ++i) {
    const double energy_deposit = lower + (i + 0.5) * step;
    const double cos_theta = compton_constraints::cosThetaKinematics(
        incident_energy, energy_deposit);
    likelihood += compton_constraints::normalized_differentialCrossSection(
        incident_energy, cos_theta) *
        normalDensity(second_hit.Energy(), energy_deposit,
                      energyResolution(energy_deposit));
  }
  return likelihood * step;
}

std::vector<DetectorHit_sptr> NanoGRAMSKNFormulaEventReconstructionAlgorithm::
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

double NanoGRAMSKNFormulaEventReconstructionAlgorithm::
energyCorrectionFactor(const DetectorHit_sptr& hit) const
{
  const int fec = hit->ReadoutModuleID();
  if (fec < 0 || fec >= NUM_VATA) {
    throw std::runtime_error("NanoGRAMS hit has an invalid readout FEC ID.");
  }
  return energy_correction_factors_[fec];
}

double NanoGRAMSKNFormulaEventReconstructionAlgorithm::energyResolution(
    double energy) const
{
  const double energy_keV = std::max(0.0, energy / unit::keV);
  const double sigma_keV = std::sqrt(
      energy_resolution_param0_ * energy_resolution_param0_ +
      energy_resolution_param1_ * energy_resolution_param1_ * energy_keV +
      energy_resolution_param2_ * energy_resolution_param2_ * energy_keV * energy_keV);
  return std::max(kMinimumEnergyResolution, sigma_keV * unit::keV);
}

void NanoGRAMSKNFormulaEventReconstructionAlgorithm::printParameters() const
{
  std::cout << "--- NanoGRAMS KN formula reconstruction ---\n"
            << "known_initial_gammaray_energy: "
            << known_initial_gammaray_energy_ / unit::keV << " keV\n"
            << "energy_resolution: sqrt(param0^2 + param1^2 * E_keV "
               "+ param2^2 * E_keV^2) keV\n"
            << "  param0: " << energy_resolution_param0_ << "\n"
            << "  param1: " << energy_resolution_param1_ << "\n"
            << "  param2: " << energy_resolution_param2_ << "\n"
            << "energy_margin_multiplier: "
            << energy_margin_multiplier_ << "\n";
}

} /* namespace comptonsoft */
