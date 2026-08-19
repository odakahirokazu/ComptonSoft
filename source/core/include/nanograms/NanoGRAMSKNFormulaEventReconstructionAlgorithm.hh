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

#ifndef COMPTONSOFT_NanoGRAMSKNFormulaEventReconstructionAlgorithm_H
#define COMPTONSOFT_NanoGRAMSKNFormulaEventReconstructionAlgorithm_H 1

#include <array>

#include "NanoGRAMSConstants.hh"
#include "VEventReconstructionAlgorithm.hh"

namespace comptonsoft
{

/**
 * Two-hit reconstruction for a known incident energy.
 *
 * The photon is allowed to escape after the second interaction, as is usual
 * for a small LArTPC.
 */
class NanoGRAMSKNFormulaEventReconstructionAlgorithm
    : public VEventReconstructionAlgorithm
{
public:
  NanoGRAMSKNFormulaEventReconstructionAlgorithm();
  ~NanoGRAMSKNFormulaEventReconstructionAlgorithm() override;

  void initializeEvent() override;
  bool reconstruct(const std::vector<DetectorHit_sptr>& hits,
                   const BasicComptonEvent& baseEvent,
                   std::vector<BasicComptonEvent_sptr>& eventsReconstructed) override;

protected:
  bool loadParameters(boost::property_tree::ptree& pt) override;
  bool loadParametersYAML(YAML::Node& node) override;

private:
  struct Candidate
  {
    int first_hit_index = -1;
    double likelihood = 0.0;
    bool escape = false;
  };

  Candidate evaluateOrder(const std::vector<DetectorHit_sptr>& hits,
                          int first_hit_index) const;
  double fullAbsorptionLikelihood(const DetectorHit& first_hit,
                                  const DetectorHit& second_hit) const;
  double escapeLikelihood(const DetectorHit& first_hit,
                          const DetectorHit& second_hit) const;
  double secondScatterLikelihood(const DetectorHit& second_hit,
                                 double incident_energy) const;
  std::vector<DetectorHit_sptr> correctedHits(
      const std::vector<DetectorHit_sptr>& hits) const;
  double energyCorrectionFactor(const DetectorHit_sptr& hit) const;
  double energyResolution(double energy) const;
  void printParameters() const;

  double known_initial_gammaray_energy_ = 0.0;
  double required_minimum_energy_deposit_in_higher_hit_ = 0.0;
  double energy_resolution_param0_ = 0.0;
  double energy_resolution_param1_ = 0.0;
  double energy_resolution_param2_ = 0.0;
  double energy_margin_multiplier_ = 1.0;
  std::array<double, NUM_VATA> energy_correction_factors_{};
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSKNFormulaEventReconstructionAlgorithm_H */
