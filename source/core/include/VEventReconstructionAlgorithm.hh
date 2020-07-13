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

#ifndef COMPTONSOFT_VEventReconstructionAlgorithm_H
#define COMPTONSOFT_VEventReconstructionAlgorithm_H 1

#include <vector>
#include <memory>
#include "DetectorHit_sptr.hh"
#include "BasicComptonEvent.hh"

namespace comptonsoft {

/**
 * A class of an event reconstruction algorithm.
 * @author Hirokazu Odaka
 * @date 2011-02-16
 * @date 2014-11-17
 * @date 2020-07-02 | multiple reconstructed events
 */
class VEventReconstructionAlgorithm
{
public:
  VEventReconstructionAlgorithm();
  virtual ~VEventReconstructionAlgorithm();
  VEventReconstructionAlgorithm(const VEventReconstructionAlgorithm&) = default;
  VEventReconstructionAlgorithm(VEventReconstructionAlgorithm&&) = default;
  VEventReconstructionAlgorithm& operator=(const VEventReconstructionAlgorithm&) = default;
  VEventReconstructionAlgorithm& operator=(VEventReconstructionAlgorithm&&) = default;

  void setMaxHits(int v) { maxHits_ = v; }
  int MaxHits() const { return maxHits_; }

  virtual void initializeEvent() = 0;
  
  /**
   * perform the event reconstruction.
   * @param hits vector of hits to reconstruct.
   * @param baseEvent event based for new reconstructed events.
   * @param eventsReconstructed event vector reconstructed.
   * @return true if the reconstruction is successful.
   */
  virtual bool reconstruct(const std::vector<DetectorHit_sptr>& hits,
                           const BasicComptonEvent& baseEvent,
                           std::vector<BasicComptonEvent_sptr>& eventsReconstructed) = 0;

private:
  int maxHits_;
};

double total_energy_deposits(const std::vector<DetectorHit_sptr>& hits);

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VEventReconstructionAlgorithm_H */
