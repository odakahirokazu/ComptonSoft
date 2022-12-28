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

#ifndef COMPTONSOFT_OberlackAlgorithm_H
#define COMPTONSOFT_OberlackAlgorithm_H 1

#include "VEventReconstructionAlgorithm.hh"
#include "TString.h"
#include "TFile.h"
#include "TGraph.h"

namespace comptonsoft {

/**
 * A class of a standard Compton event reconstuction algorithm.
 * @author Hiroki Yoneda
 * @date 2020-11-18 | Hiroki Yoneda
 * @date 2021-01-04 | Hirokazu Odaka | code cleanup
 * @date 2022-12-28 | Hiroki Yoneda | review
 */
class OberlackAlgorithm : public VEventReconstructionAlgorithm
{
public:
  OberlackAlgorithm();
  virtual ~OberlackAlgorithm();
  OberlackAlgorithm(const OberlackAlgorithm&) = default;
  OberlackAlgorithm(OberlackAlgorithm&&) = default;
  OberlackAlgorithm& operator=(const OberlackAlgorithm&) = default;
  OberlackAlgorithm& operator=(OberlackAlgorithm&&) = default;

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

  bool reconstructFullDepositEvent(const std::vector<DetectorHit_sptr>& hits,
                                   const BasicComptonEvent& baseEvent,
                                   std::vector<BasicComptonEvent_sptr>& eventsReconstructed);

  bool reconstructEscapeEvent(const std::vector<DetectorHit_sptr>& hits,
                              const BasicComptonEvent& baseEvent,
                              std::vector<BasicComptonEvent_sptr>& eventsReconstructed);

  bool reconstructOrderedHits(const std::vector<DetectorHit_sptr>& ordered_hits,
                              BasicComptonEvent& eventReconstructed,
                              double incident_energy,
                              bool is_escape_event = false);

  double calFOM(const std::vector<DetectorHit_sptr>& ordered_hits,
                       double incident_energy,
                       bool is_escape_event);

  void normalizeReconstructionFraction(std::vector<BasicComptonEvent_sptr>& eventsReconstructed);
  void selectMaximumLikelihoodOrder(std::vector<BasicComptonEvent_sptr>& eventsReconstructed);

private:
  void setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits);

  double getEnergyResolution(double energy);
  double getErrorCosThetaGeom(const vector3_t& incident_direction, const vector3_t& scattering_direction);

  double compScatteringAngle(const std::vector<DetectorHit_sptr>& ordered_hits, int i_hit, double incident_energy, bool is_escape_event);
  double compScatteringAngleWithInitialDirection(const std::vector<DetectorHit_sptr>& ordered_hits, double incident_energy, bool is_escape_event);

private:
  double sigma_level_energy_margin_for_checkScatteringAngle_;
  int process_mode;
  bool assume_initial_gammaray_energy;
  double known_initial_gammaray_energy;
  bool assume_initial_direction;
  vector3_t known_initial_direction;
  bool use_averaged_escaped_energy;

  double total_energy_deposits_;
  double num_hits_;

  double par0_energy_resolution_;
  double par1_energy_resolution_;
  double par2_energy_resolution_;

  bool consider_position_resolution;
  
  double position_resolution_x_;
  double position_resolution_y_;
  double position_resolution_z_;

  double escape_weight_;

  std::string cross_section_filename_;
  TFile* cross_section_file_;
  TGraph* tg_cross_section_compton_;
  TGraph* tg_cross_section_phot_abs_;
  TGraph* tg_cross_section_pair_;
  TGraph* tg_cross_section_tot_;

  bool selecting_maximum_likelihood_order_ = true;

  bool Kroger_mode = false; // to compare Kroger et al. 2002 
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_OberlackAlgorithm_H */
