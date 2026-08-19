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

#ifndef COMPTONSOFT_NanoGRAMSEnergyComparisonEventReconstructionAlgorithm_H
#define COMPTONSOFT_NanoGRAMSEnergyComparisonEventReconstructionAlgorithm_H 1

#include <array>

#include "NanoGRAMSConstants.hh"
#include "VEventReconstructionAlgorithm.hh"

namespace comptonsoft
{

/**
 * Two-hit reconstruction which takes the higher-energy hit as the first hit.
 * The photon is allowed to escape after the second interaction.
 */
class NanoGRAMSEnergyComparisonEventReconstructionAlgorithm
    : public VEventReconstructionAlgorithm
{
public:
  NanoGRAMSEnergyComparisonEventReconstructionAlgorithm();
  ~NanoGRAMSEnergyComparisonEventReconstructionAlgorithm() override;

  void initializeEvent() override;
  bool reconstruct(const std::vector<DetectorHit_sptr>& hits,
                   const BasicComptonEvent& baseEvent,
                   std::vector<BasicComptonEvent_sptr>& eventsReconstructed) override;

protected:
  bool loadParameters(boost::property_tree::ptree& pt) override;
  bool loadParametersYAML(YAML::Node& node) override;

private:
  bool isPhysicalOrder(const std::vector<DetectorHit_sptr>& hits,
                       int first_hit_index, bool& escape) const;
  std::vector<DetectorHit_sptr> correctedHits(
      const std::vector<DetectorHit_sptr>& hits) const;
  double energyCorrectionFactor(const DetectorHit_sptr& hit) const;
  void printParameters() const;

  double known_initial_gammaray_energy_ = 0.0;
  double required_minimum_energy_deposit_in_higher_hit_ = 0.0;
  double energy_tolerance_sigma_ = 0.0;
  std::array<double, NUM_VATA> energy_correction_factors_{};
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSEnergyComparisonEventReconstructionAlgorithm_H */
