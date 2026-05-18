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
  std::array<std::uint16_t, NUM_CHARGE_READOUT> arrayHitnum_{};
  std::array<std::array<std::uint32_t, 3>, NUM_CHARGE_READOUT> arrayChFlag_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> arrayCmn_{};
  std::array<std::array<std::uint16_t, NUM_CH_CHARGE>, NUM_CHARGE_READOUT> arrayAdc_{};
  std::array<std::array<std::uint16_t, NUM_CH_CHARGE>, NUM_CHARGE_READOUT> arrayIndex_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> arrayRef_{};
  std::array<double, NUM_CHARGE_READOUT> arrayCmnEx_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> ti_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> eventIdCharge_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> liveTime_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> integralLiveTime_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> driftTime_{};
  std::uint32_t lightTriggerIndex_ = 0;
  std::array<std::uint32_t, NUM_CHARGE_READOUT> unixTime_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> eventFlags_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> flagPseudo_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> flagNotrigForcedTrigger_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> flagSelfTrig_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> fecIndex_{};
  std::array<bool, NUM_CHARGE_READOUT> goodEvent_{};

  std::array<std::uint32_t, NUM_CH_LIGHT> eventIdLight_{};
  std::array<std::uint64_t, NUM_CH_LIGHT> realTime_{};
  std::array<std::uint8_t, NUM_CH_LIGHT> realTimePrecise_{};
  std::array<std::uint16_t, NUM_CH_LIGHT> ch_{};
  std::array<std::uint16_t, NUM_CH_LIGHT> qdc_{};
  std::array<std::uint16_t, NUM_CH_LIGHT> waveNum_{};
  std::array<std::array<std::int16_t, MAX_WAVENUM>, NUM_CH_LIGHT> waveData_{};
  std::uint32_t triggerid_ = 0;
  std::array<bool, NUM_CH_LIGHT> trigger_{};
  std::array<std::uint16_t, NUM_CH_LIGHT> waveCompress_{};
  std::array<bool, NUM_CH_LIGHT> isWaveList_{};
  std::array<bool, NUM_CH_LIGHT> registered_{};

  std::uint16_t errorFlags_ = 0;
  TTree* tpctree_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_TPCTreeIO_H */
