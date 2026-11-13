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

#ifndef COMPTONSOFT_NanoGRAMSSelectEvents_H
#define COMPTONSOFT_NanoGRAMSSelectEvents_H 1

#include <cstdint>

#include "VCSModule.hh"

namespace comptonsoft {

class CSHitCollection;
class DetectorHit;
class RealDetectorUnitNanoGRAMS;
namespace grams {
struct Config;
enum class LightEventSelectionMode;
}

/**
 * Selection of NanoGRAMS gamma-ray events using the flags set by the detector unit and NanoGRAMSAnalyzeLight.
 * Rejected hits are removed from the hit collection; if no hit remains, AS_SKIP is returned.
 *
 * All the hits of a detector are rejected if
 *   - the highest pixel of an FEC is an excluded pixel (ExcludedCore),
 *   - light.event_selection_mode is not disabled, and the light is cosmic-like or a pileup is found,
 *   - light.event_selection_mode is gamma_required, and the light is not gamma-like.
 * A hit (cluster) is rejected if it has any of the flags
 *   NanoGRAMSTimeUp, NanoGRAMSPixelCountOutOfRange, NanoGRAMSCollinear, NanoGRAMSMultipleClustersInFEC
 * (only with reject_charge_clusters, default true).
 *
 * This module should be placed after SelectHits (and before the hit tree writer).
 *
 * @date 2026-09-24 | the selection of FECChargeSelector/TPCTreeReader is ported onto flags
 */
class NanoGRAMSSelectEvents : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSSelectEvents, 1.0);

public:
  NanoGRAMSSelectEvents();
  ~NanoGRAMSSelectEvents() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  // the selection criteria (also used by NanoGRAMSQuickLookWriter)
  static bool isDetectorRejected(const RealDetectorUnitNanoGRAMS& detector, grams::LightEventSelectionMode mode);
  static bool isClusterRejected(const DetectorHit& hit);

private:
  const grams::Config* config_ = nullptr;
  CSHitCollection* hitCollection_ = nullptr;
  bool rejectChargeClusters_ = true;

  int64_t numEvents_ = 0;
  int64_t numSelectedEvents_ = 0;
  int64_t numRejectedDetectorEvents_ = 0;
  int64_t numRejectedHits_ = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSSelectEvents_H */
