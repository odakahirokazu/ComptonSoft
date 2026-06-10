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

#ifndef COMPTONSOFT_NanoGRAMSEventReconstructionAlgorithm_H
#define COMPTONSOFT_NanoGRAMSEventReconstructionAlgorithm_H 1

#include "VEventReconstructionAlgorithm.hh"
#include "TString.h"
#include "TFile.h"
#include "TGraph.h"

namespace comptonsoft {

/**
 * A class of a standard Compton event reconstuction algorithm.
 * @author Satoshi Takashima
 * @date 2026-06-08 | Satoshi Takashima
 */
class NanoGRAMSEventReconstructionAlgorithm : public VEventReconstructionAlgorithm
{
public:
  NanoGRAMSEventReconstructionAlgorithm();
  virtual ~NanoGRAMSEventReconstructionAlgorithm();
  NanoGRAMSEventReconstructionAlgorithm(const NanoGRAMSEventReconstructionAlgorithm&) = default;
  NanoGRAMSEventReconstructionAlgorithm(NanoGRAMSEventReconstructionAlgorithm&&) = default;
  NanoGRAMSEventReconstructionAlgorithm& operator=(const NanoGRAMSEventReconstructionAlgorithm&) = default;
  NanoGRAMSEventReconstructionAlgorithm& operator=(NanoGRAMSEventReconstructionAlgorithm&&) = default;

  void initializeEvent() override;

  /**
   * perform the event reconstruction.
   * @param hits vector of hits to reconstruct.
   * @param baseEvent event based for new reconstructed events.
   * @param eventsReconstructed events reconstructed.
   * @return true if the reconstruction is successful.
   */
  bool reconstruct(const std::vector<DetectorHit_sptr>& hits,
                   const BasicComptonEvent& baseEvent,
                   std::vector<BasicComptonEvent_sptr>& eventsReconstructed) override;

protected:
  bool loadParameters(boost::property_tree::ptree& pt) override;
#if CS_USE_YAMLCPP
  bool loadParametersYAML(YAML::Node& node) override;
#endif /* CS_USE_YAMLCPP */

  bool reconstruct2HitEvent(const std::vector<DetectorHit_sptr>& hits,
                       const BasicComptonEvent& baseEvent,
                       std::vector<BasicComptonEvent_sptr>& eventsReconstructed);

  bool reconstructOrderedHits(const std::vector<DetectorHit_sptr>& ordered_hits,
                              BasicComptonEvent& eventReconstructed,
                              double incident_energy);

  void selectLeastARMOrder(std::vector<BasicComptonEvent_sptr>& eventsReconstructed);

private:
  void setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits);
  bool isSatisfyKinematics(const std::vector<DetectorHit_sptr>& ordered_hits, double incident_energy);

private:
  double total_energy_deposits_;
  double num_hits_;
  std::vector<double> incident_energy_candidates_;
  bool is_escape_event_ = false;

  bool selecting_most_likely_order_ = true;
  bool kinematics_check_ = true;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSEventReconstructionAlgorithm_H */
