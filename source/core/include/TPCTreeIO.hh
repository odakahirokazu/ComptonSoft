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

#ifndef COMPTONSOFT_TPCTreeIO_H
#define COMPTONSOFT_TPCTreeIO_H 1

#include <array>
#include <cstdint>

#include "NanoGRAMSEvent.hh"

class TTree;

namespace comptonsoft
{

class TPCTreeIO
{
public:
  TPCTreeIO();
  virtual ~TPCTreeIO();

  void SetBranchAddress(TTree* tree);
  void RetrieveEvent(NanoGRAMSEvent& event) const;

private:
  std::array<uint16_t, NUM_VATA> arrayHitnum_{};
  std::array<std::array<uint32_t, 3>, NUM_VATA> arrayChFlag_{};
  std::array<uint16_t, NUM_VATA> arrayCmn_{};
  std::array<std::array<uint16_t, NUM_CH_EACH_VATA>, NUM_VATA> arrayAdc_{};
  std::array<std::array<uint16_t, NUM_CH_EACH_VATA>, NUM_VATA> arrayIndex_{};
  std::array<uint16_t, NUM_VATA> arrayRef_{};
  std::array<double,   NUM_VATA> arrayCmnEx_{};
  std::array<uint32_t, NUM_VATA> ti_{};
  std::array<uint32_t, NUM_VATA> eventIdCharge_{};
  std::array<uint32_t, NUM_VATA> liveTime_{};
  std::array<uint32_t, NUM_VATA> integralLiveTime_{};
  std::array<uint32_t, NUM_VATA> driftTime_{};
  uint32_t lightTriggerIndex_ = 0;
  std::array<uint32_t, NUM_VATA> unixTime_{};
  std::array<uint32_t, NUM_VATA> eventFlags_{};
  std::array<uint16_t, NUM_VATA> flagPseudo_{};
  std::array<uint16_t, NUM_VATA> flagNotrigForcedTrigger_{};
  std::array<uint16_t, NUM_VATA> flagSelfTrig_{};
  std::array<uint32_t, NUM_VATA> fecIndex_{};
  std::array<bool,     NUM_VATA> goodEvent_{};

  std::array<uint32_t, NUM_CH_DPP_ON> eventIdLight_{};
  std::array<uint64_t, NUM_CH_DPP_ON> realTime_{};
  std::array<uint8_t,  NUM_CH_DPP_ON> realTimePrecise_{};
  std::array<uint16_t, NUM_CH_DPP_ON> ch_{};
  std::array<uint16_t, NUM_CH_DPP_ON> qdc_{};
  std::array<uint16_t, NUM_CH_DPP_ON> waveNum_{};
  std::array<std::array<int16_t, MAX_WAVENUM>, NUM_CH_DPP_ON> waveData_{};
  uint32_t triggerid_ = 0;
  std::array<bool,     NUM_CH_DPP_ON> trigger_{};
  std::array<uint16_t, NUM_CH_DPP_ON> waveCompress_{};
  std::array<bool,     NUM_CH_DPP_ON> isWaveList_{};
  std::array<bool,     NUM_CH_DPP_ON> registered_{};

  uint16_t errorFlags_ = 0;
  TTree* tpctree_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_TPCTreeIO_H */
