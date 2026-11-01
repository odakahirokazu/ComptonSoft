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

#ifndef COMPTONSOFT_NanoGRAMSAnalyzeLight_H
#define COMPTONSOFT_NanoGRAMSAnalyzeLight_H 1

#include <vector>

#include "VCSModule.hh"

namespace comptonsoft {

class LightData;
class RealDetectorUnitNanoGRAMS;
class NanoGRAMSLoadConfig;
namespace grams {
struct Config;
}

/**
 * Light (scintillation) judgement of NanoGRAMS events using the light data of the detector unit.
 * The results are set as event flags of the detector unit (nanograms_event_flag::Light*).
 *
 * Time windows (time = 0 at the trigger):
 *   pre-ROI  [-drift_time_max, -pre_roi_window)
 *   ROI      [-pre_roi_window, post_roi_window)
 *   post-ROI [post_roi_window, drift_time_max)
 * - general analysis channels: peak in ROI > light_gamma_thr -> LightGamma, > light_cosmic_thr -> LightCosmic
 * - pileup analysis channels: peak in pre- or post-ROI > out_roi_peak_thr -> LightPileup
 * - general analysis channels: charge integrated in ROI (baseline = mean in pre-ROI) is set to the detector unit
 *   (RealDetectorUnitNanoGRAMS::LightIntegratedCharge), converted with the transimpedance amplifier parameters;
 *   their sum over the general analysis channels is set as the photon count (RealDetectorUnitNanoGRAMS::PhotonCount).
 * - pileup analysis channels: the integrated charge is also set (not included in the photon count)
 * This module must be placed before SelectHits so that the photon count is given to the hits.
 * The parameters are taken from the configuration of NanoGRAMSLoadConfig.
 *
 * @date 2026-09-24 | ported from NanoGRAMSLightAnalysis (analyzeLightEvent)
 */
class NanoGRAMSAnalyzeLight : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSAnalyzeLight, 1.0);

public:
  NanoGRAMSAnalyzeLight();
  ~NanoGRAMSAnalyzeLight() override;

  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  struct LightPeaks
  {
    double preROI;
    double ROI;
    double postROI;
  };

  std::vector<int> collectValidChannels(const RealDetectorUnitNanoGRAMS& detector,
                                        const std::vector<int>& channels) const;
  LightPeaks analyzeChannelGroup(const RealDetectorUnitNanoGRAMS& detector, const std::vector<int>& channels) const;
  double integratedROICharge(const LightData& lightData) const;

private:
  const grams::Config* config_ = nullptr;
  const NanoGRAMSLoadConfig* configLoader_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSAnalyzeLight_H */
