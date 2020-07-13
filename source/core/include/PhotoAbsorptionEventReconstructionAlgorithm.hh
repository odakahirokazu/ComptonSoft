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

#ifndef COMPTONSOFT_PhotoAbsorptionEventReconstructionAlgorithm_H
#define COMPTONSOFT_PhotoAbsorptionEventReconstructionAlgorithm_H 1

#include "VEventReconstructionAlgorithm.hh"

namespace comptonsoft {

/**
 * A class of an event reconstuction algorithm for a photo-absorption mode.
 * @author Hirokazu Odaka
 * @date 2015-10-16
 */
class PhotoAbsorptionEventReconstructionAlgorithm : public VEventReconstructionAlgorithm
{
public:
  PhotoAbsorptionEventReconstructionAlgorithm();
  virtual ~PhotoAbsorptionEventReconstructionAlgorithm();
  PhotoAbsorptionEventReconstructionAlgorithm(const PhotoAbsorptionEventReconstructionAlgorithm&) = default;
  PhotoAbsorptionEventReconstructionAlgorithm(PhotoAbsorptionEventReconstructionAlgorithm&&) = default;
  PhotoAbsorptionEventReconstructionAlgorithm& operator=(const PhotoAbsorptionEventReconstructionAlgorithm&) = default;
  PhotoAbsorptionEventReconstructionAlgorithm& operator=(PhotoAbsorptionEventReconstructionAlgorithm&&) = default;

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
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PhotoAbsorptionEventReconstructionAlgorithm_H */
