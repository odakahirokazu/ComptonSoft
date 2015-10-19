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

#ifndef COMPTONSOFT_FocalPlaneEventReconstructionAlgorithm_H
#define COMPTONSOFT_FocalPlaneEventReconstructionAlgorithm_H 1

#include "VEventReconstructionAlgorithm.hh"

namespace comptonsoft {

/**
 * A class of an event reconstuction algorithm for a multi-layer focal plane detector.
 * @author Hirokazu Odaka
 * @date 2015-10-16
 */
class FocalPlaneEventReconstructionAlgorithm : public VEventReconstructionAlgorithm
{
public:
  FocalPlaneEventReconstructionAlgorithm();
  virtual ~FocalPlaneEventReconstructionAlgorithm();
  FocalPlaneEventReconstructionAlgorithm(const FocalPlaneEventReconstructionAlgorithm&) = default;
  FocalPlaneEventReconstructionAlgorithm(FocalPlaneEventReconstructionAlgorithm&&) = default;
  FocalPlaneEventReconstructionAlgorithm& operator=(const FocalPlaneEventReconstructionAlgorithm&) = default;
  FocalPlaneEventReconstructionAlgorithm& operator=(FocalPlaneEventReconstructionAlgorithm&&) = default;

  void initializeEvent();

  /**
   * perform the event reconstruction.
   * @param hits vector of hits to reconstruct.
   * @param eventReconstructed event reconstructed.
   * @return true if the reconstruction is successful.
   */
  bool reconstruct(const std::vector<DetectorHit_sptr>& hits,
                   BasicComptonEvent& eventReconstructed);

protected:
  bool reconstruct2HitsLH(const std::vector<DetectorHit_sptr>& hits,
                          BasicComptonEvent& eventReconstructed);
  bool reconstruct2HitsHH(const std::vector<DetectorHit_sptr>& hits,
                          BasicComptonEvent& eventReconstructed);
  bool reconstruct2HitsLL(const std::vector<DetectorHit_sptr>& hits,
                          BasicComptonEvent& eventReconstructed);
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_FocalPlaneEventReconstructionAlgorithm_H */
