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

#include "NanoGRAMSChargeToEnergySpline.hh"

#include <TGraph.h>
#include <TSpline.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace comptonsoft
{

namespace
{

constexpr int kNumKnots = 99;
constexpr double kMinEnergyKeV = 1.0;
constexpr double kMaxEnergyKeV = 10000.0;

double collectedCharge(double energy_kev, double efield_v_cm)
{
  constexpr double kWIonizationEV = 23.6;
  constexpr double kWQuantaEV = 19.5;
  constexpr double kExcitonToIonRatio = 0.21;
  constexpr double kAprileK = 3.7e-3;
  constexpr double kAprileGamma = 2.8e4;
  constexpr double kAlpha = 0.227e12;
  constexpr double kBeta = 1.7e6;
  constexpr double kElementaryChargeC = 1.602176634e-19;

  const double denominator = efield_v_cm + kAprileK * kBeta / kWIonizationEV;
  const double z = energy_kev * 1000.0 * denominator /
                   (kAprileK * kAlpha / kWIonizationEV);
  const double recombination =
      (efield_v_cm / denominator) * (1.0 - std::log1p(z) / z) *
      (1.0 + kAprileK * kAprileGamma / efield_v_cm);
  return energy_kev * 1000.0 * recombination /
         (kWQuantaEV * (1.0 + kExcitonToIonRatio)) * kElementaryChargeC;
}

} // namespace

ChargeToEnergySpline::ChargeToEnergySpline() = default;

ChargeToEnergySpline::~ChargeToEnergySpline() = default;

void ChargeToEnergySpline::setElectricField(double efield_v_cm)
{
  if (efield_v_cm <= 0.0) {
    throw std::runtime_error("Electric field must be positive.");
  }

  std::array<double, kNumKnots> charges;
  std::array<double, kNumKnots> energies;
  const double log_step = std::log(kMaxEnergyKeV / kMinEnergyKeV) /
                          static_cast<double>(kNumKnots - 1);
  for (int i = 0; i < kNumKnots; ++i) {
    const double energy = kMinEnergyKeV * std::exp(log_step * i);
    energies[i] = energy;
    charges[i] = collectedCharge(energy, efield_v_cm);
  }
  TGraph graph(kNumKnots, charges.data(), energies.data());
  spline_ = std::make_unique<TSpline3>("charge_to_energy", &graph);
  minimum_charge_ = charges.front();
  maximum_charge_ = charges.back();
}

double ChargeToEnergySpline::evaluate(double charge_coulomb) const
{
  const double charge = std::clamp(charge_coulomb, minimum_charge_, maximum_charge_);
  return spline_->Eval(charge);
}

} /* namespace comptonsoft */
