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
 * @file NanoGRAMSTPCTreeIO.hh
 * @brief Lightweight TTree input buffer for NanoGRAMS tpctree files.
 */

#ifndef COMPTONSOFT_NanoGRAMSTPCTreeIO_H
#define COMPTONSOFT_NanoGRAMSTPCTreeIO_H 1

#include <array>
#include <cstdint>
#include <vector>

#include "NanoGRAMSConstants.hh"

class TTree;

namespace comptonsoft {
namespace grams {

class TPCTreeBuffer
{
public:
  explicit TPCTreeBuffer(TTree* tpc_tree);

  int64_t nEntries() const;
  uint32_t representativeUnixTime() const;
  void getEntry(int64_t entry);
  bool waveRegistered(int channel) const { return wave_num[channel] > 0; }

  std::array<uint16_t, NUM_CH_DPP_MAX> wave_compress{};
  std::vector<uint16_t> adc;
  std::vector<uint32_t> drift_time;
  std::vector<uint32_t> ti;
  std::array<uint32_t, NUM_VATA> unixtime{};
  std::array<std::array<int16_t, MAX_WAVE_LENGTH>, NUM_CH_DPP_MAX> waveform;
  std::array<uint32_t, NUM_CH_DPP_MAX> wave_num{};
  uint16_t error_flags = 0;

private:
  void bindBranches(TTree* tpc_tree);

  TTree* tpc_tree_ = nullptr;
};

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSTPCTreeIO_H */
