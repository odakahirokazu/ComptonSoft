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

#ifndef COMPTONSOFT_HY2017EventReconstructionAlgorithm_H
#define COMPTONSOFT_HY2017EventReconstructionAlgorithm_H 1

#include "VEventReconstructionAlgorithm.hh"

namespace comptonsoft {

/**
 * A class of a standard Compton event reconstuction algorithm.
 * @author Hiroki Yoneda
 * @date 2020-07-10 | Hiroki Yoneda
 * @date 2020-07-15 | Hirokazu Odaka | code cleanup
 */
class HY2017EventReconstructionAlgorithm : public VEventReconstructionAlgorithm
{
public:
  HY2017EventReconstructionAlgorithm();
  virtual ~HY2017EventReconstructionAlgorithm();
  HY2017EventReconstructionAlgorithm(const HY2017EventReconstructionAlgorithm&) = default;
  HY2017EventReconstructionAlgorithm(HY2017EventReconstructionAlgorithm&&) = default;
  HY2017EventReconstructionAlgorithm& operator=(const HY2017EventReconstructionAlgorithm&) = default;
  HY2017EventReconstructionAlgorithm& operator=(HY2017EventReconstructionAlgorithm&&) = default;

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

  double calReconstructionFraction(const std::vector<DetectorHit_sptr>& ordered_hits,
                                   double incident_energy,
                                   bool is_escape_event);

  void normalizeReconstructionFraction(std::vector<BasicComptonEvent_sptr>& eventsReconstructed);

private:
  void setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits);

private:
  double energy_margin_for_checkEdepIsPhysicallyAcceptable_;
  double energy_margin_for_checkScatteringAngle_;

  double total_energy_deposits_;
  double num_hits_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HY2017EventReconstructionAlgorithm_H */
