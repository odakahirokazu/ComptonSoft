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

#ifndef COMPTONSOFT_NanoGRAMSConstants_H
#define COMPTONSOFT_NanoGRAMSConstants_H 1

#include <array>
#include <cstdint>

#include "AstroUnits.hh"
#include "NanoGRAMSChannelMap.hh"
#include "NanoGRAMSMultiChannelData.hh"

namespace comptonsoft {

constexpr int NUM_CH_DPP_MAX = 8;
constexpr int NUM_VATA = NanoGRAMSMultiChannelData::NUM_FECS;
constexpr int NUM_CH_EACH_VATA = NanoGRAMSMultiChannelData::NUM_CHANNELS;
constexpr int MAX_WAVE_LENGTH = 8000;

namespace grams {

constexpr const char* kTpcTreeName = "tpctree";
constexpr const char* kHitTreeName = "hittree";
constexpr const char* kQuickLookTreeName = "tpcquicklook";

constexpr double kDppResponseTime = 0.68 * anlgeant4::unit::us;
constexpr double kClkToUs = 0.01; // 1 clock = 10 ns

inline double driftTimeFromClock(uint32_t drift_time_count)
{
  return static_cast<double>(drift_time_count) * kClkToUs * anlgeant4::unit::us + kDppResponseTime;
}

// the channel map of the anode is defined in the core (NanoGRAMSChannelMap.hh)
constexpr int kFECSectionSidePixels = nanograms::FECSectionSidePixels;
constexpr int kTPCPlaneSidePixels = nanograms::AnodeSidePixels;
inline constexpr const auto& kFECSectionGridToChannel = nanograms::FECSectionGridToChannel;

static_assert(NUM_VATA == static_cast<int>(kFECSectionGridToChannel.size()),
              "kFECSectionGridToChannel must match NUM_VATA.");

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSConstants_H */
