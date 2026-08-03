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

/**
 * @file NanoGRAMSLightAnalysis.hh
 * @brief Light waveform analysis helpers for NanoGRAMS data reduction.
 * @author Satoshi Takashima
 * @date 2026-06-11
 */

#ifndef COMPTONSOFT_NanoGRAMSLightAnalysis_H
#define COMPTONSOFT_NanoGRAMSLightAnalysis_H 1

#include <array>
#include <cstdint>
#include <string>

#include "AstroUnits.hh"
#include "NanoGRAMSConfig.hh"
#include "NanoGRAMSTPCTreeIO.hh"

namespace comptonsoft
{
namespace grams
{

struct LightTimingState
{
  bool ready = false;
  std::array<uint16_t, NUM_CH_DPP_MAX> wave_compress{};
  std::array<double,   NUM_CH_DPP_MAX> timebin{};
  std::array<int,      NUM_CH_DPP_MAX> pre_pileup_start_index{};
  std::array<int,      NUM_CH_DPP_MAX> pre_pileup_stop_index{};
  std::array<int,      NUM_CH_DPP_MAX> post_pileup_start_index{};
  std::array<int,      NUM_CH_DPP_MAX> post_pileup_stop_index{};
};

struct LightStatus
{
  bool valid_any = false;
  bool general_valid = false;
  bool pileup_valid = false;
  bool gamma = false;
  bool cosmic = false;
  bool pileup_pre_roi = false;
  bool pileup_post_roi = false;
  double roi_integral_charge = 0.0 * anlgeant4::unit::coulomb;

  bool hasPileup() const
  {
    return pileup_pre_roi || pileup_post_roi;
  }
};

std::string normalizeLightWaveformAnalysis(const std::string& mode);

LightStatus analyzeLightEvent(const Config& cfg,
                              const TPCTreeBuffer& tpc_tree_buffer,
                              const LightTimingState& light_timing,
                              bool light_ok);

double lightRoiIntegralCharge(const Config& cfg,
                              const TPCTreeBuffer& tpc_tree_buffer,
                              const LightTimingState& light_timing,
                              bool light_ok);

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSLightAnalysis_H */
