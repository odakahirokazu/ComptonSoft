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

#ifndef COMPTONSOFT_NanoGRAMSEvent_H
#define COMPTONSOFT_NanoGRAMSEvent_H 1

#include <array>
#include <cstdint>
#include <memory>

namespace comptonsoft
{

constexpr int NUM_CH_LIGHT = 8;
constexpr int NUM_CHARGE_READOUT = 4;
constexpr int NUM_CH_CHARGE = 64;
constexpr int MAX_WAVENUM = 8000;
constexpr std::uint32_t CLOCK_PER_TI_UNIT = 16;

class NanoGRAMSEvent
{
public:
  void ClearErrorFlags() { errorFlags_ = 0; }

  int NumRegistered() const { return numRegistered_; }

  const auto& GetTI() const { return ti_; }
  void SetTI(const std::array<std::uint32_t, NUM_CHARGE_READOUT>& ti) { ti_ = ti; }

  const auto& GetLiveTime() const { return liveTime_; }
  void SetLiveTime(const std::array<std::uint32_t, NUM_CHARGE_READOUT>& live_time)
  {
    liveTime_ = live_time;
  }

  const auto& GetIntegralLiveTime() const { return integralLiveTime_; }
  void SetIntegralLiveTime(
      const std::array<std::uint32_t, NUM_CHARGE_READOUT>& integral_live_time)
  {
    integralLiveTime_ = integral_live_time;
  }

  const auto& GetEventFlags() const { return eventFlags_; }
  void SetEventFlags(const std::array<std::uint32_t, NUM_CHARGE_READOUT>& event_flags)
  {
    eventFlags_ = event_flags;
  }

  const auto& GetFlagPseudo() const { return flagPseudo_; }
  void SetFlagPseudo(const std::array<std::uint16_t, NUM_CHARGE_READOUT>& flag_pseudo)
  {
    flagPseudo_ = flag_pseudo;
  }

  const auto& GetFlagNotrigForcedTrigger() const { return flagNotrigForcedTrigger_; }
  void SetFlagNotrigForcedTrigger(
      const std::array<std::uint16_t, NUM_CHARGE_READOUT>& flag_notrig_forced_trigger)
  {
    flagNotrigForcedTrigger_ = flag_notrig_forced_trigger;
  }

  const auto& GetFlagSelfTrig() const { return flagSelfTrig_; }
  void SetFlagSelfTrig(const std::array<std::uint16_t, NUM_CHARGE_READOUT>& flag_self_trig)
  {
    flagSelfTrig_ = flag_self_trig;
  }

  const auto& GetDriftTime() const { return driftTime_; }
  void SetDriftTime(const std::array<std::uint32_t, NUM_CHARGE_READOUT>& drift_time)
  {
    driftTime_ = drift_time;
  }

  auto GetLightTriggerIndex() const { return lightTriggerIndex_; }
  void SetLightTriggerIndex(std::uint32_t light_trigger_index)
  {
    lightTriggerIndex_ = light_trigger_index;
  }

  const auto& GetUnixTime() const { return unixTime_; }
  void SetUnixTime(const std::array<std::uint32_t, NUM_CHARGE_READOUT>& unix_time)
  {
    unixTime_ = unix_time;
  }

  const auto& GetArrayChFlag() const { return arrayChFlag_; }
  void SetArrayChFlag(
      const std::array<std::array<std::uint32_t, 3>, NUM_CHARGE_READOUT>& array_ch_flag)
  {
    arrayChFlag_ = array_ch_flag;
  }

  const auto& GetArrayADC() const { return arrayAdc_; }
  void SetArrayADC(
      const std::array<std::array<std::uint16_t, NUM_CH_CHARGE>, NUM_CHARGE_READOUT>& array_adc)
  {
    arrayAdc_ = array_adc;
  }

  const auto& GetArrayRef() const { return arrayRef_; }
  void SetArrayRef(const std::array<std::uint16_t, NUM_CHARGE_READOUT>& array_ref)
  {
    arrayRef_ = array_ref;
  }

  const auto& GetArrayCmn() const { return arrayCmn_; }
  void SetArrayCmn(const std::array<std::uint16_t, NUM_CHARGE_READOUT>& array_cmn)
  {
    arrayCmn_ = array_cmn;
  }

  const auto& GetArrayIndex() const { return arrayIndex_; }
  void SetArrayIndex(
      const std::array<std::array<std::uint16_t, NUM_CH_CHARGE>, NUM_CHARGE_READOUT>&
          array_index)
  {
    arrayIndex_ = array_index;
  }

  const auto& GetArrayHitNum() const { return arrayHitnum_; }
  void SetArrayHitNum(const std::array<std::uint16_t, NUM_CHARGE_READOUT>& array_hitnum)
  {
    arrayHitnum_ = array_hitnum;
  }

  const auto& GetArrayCmnEx() const { return arrayCmnEx_; }
  void SetArrayCmnEx(const std::array<double, NUM_CHARGE_READOUT>& array_cmn_ex)
  {
    arrayCmnEx_ = array_cmn_ex;
  }

  const auto& GetEventIDLight() const { return eventIdLight_; }
  void SetEventIDLight(const std::array<std::uint32_t, NUM_CH_LIGHT>& event_id_light)
  {
    eventIdLight_ = event_id_light;
  }

  const auto& GetEventIDCharge() const { return eventIdCharge_; }
  void SetEventIDCharge(const std::array<std::uint32_t, NUM_CHARGE_READOUT>& event_id_charge)
  {
    eventIdCharge_ = event_id_charge;
  }

  const auto& GetGoodEvent() const { return goodEvent_; }
  void SetGoodEvent(const std::array<bool, NUM_CHARGE_READOUT>& good_event)
  {
    goodEvent_ = good_event;
  }

  const auto& GetFecIndex() const { return fecIndex_; }
  void SetFecIndex(const std::array<std::uint32_t, NUM_CHARGE_READOUT>& fec_index)
  {
    fecIndex_ = fec_index;
  }

  const auto& GetRealTime() const { return realTime_; }
  void SetRealTime(const std::array<std::uint64_t, NUM_CH_LIGHT>& real_time)
  {
    realTime_ = real_time;
  }

  const auto& GetRealTimePrecise() const { return realTimePrecise_; }
  void SetRealTimePrecise(const std::array<std::uint8_t, NUM_CH_LIGHT>& real_time_precise)
  {
    realTimePrecise_ = real_time_precise;
  }

  const auto& GetChannel() const { return ch_; }
  void SetChannel(const std::array<std::uint16_t, NUM_CH_LIGHT>& channel) { ch_ = channel; }

  const auto& GetQDC() const { return qdc_; }
  void SetQDC(const std::array<std::uint16_t, NUM_CH_LIGHT>& qdc) { qdc_ = qdc; }

  const auto& GetWaveNum() const { return waveNum_; }
  void SetWaveNum(const std::array<std::uint16_t, NUM_CH_LIGHT>& wave_num)
  {
    waveNum_ = wave_num;
  }

  const auto& GetWaveData() const { return waveData_; }
  void SetWaveData(
      const std::array<std::array<std::int16_t, MAX_WAVENUM>, NUM_CH_LIGHT>& wave_data)
  {
    waveData_ = wave_data;
  }

  const auto& GetIsWaveList() const { return isWaveList_; }
  void SetIsWaveList(const std::array<bool, NUM_CH_LIGHT>& is_wave_list)
  {
    isWaveList_ = is_wave_list;
  }

  const auto& GetTriggerID() const { return triggerid_; }
  void SetTriggerID(std::uint32_t trigger_id) { triggerid_ = trigger_id; }

  const auto& GetTrigger() const { return trigger_; }
  void SetTrigger(const std::array<bool, NUM_CH_LIGHT>& trigger) { trigger_ = trigger; }

  const auto& GetWaveCompress() const { return waveCompress_; }
  void SetWaveCompress(const std::array<std::uint16_t, NUM_CH_LIGHT>& wave_compress)
  {
    waveCompress_ = wave_compress;
  }

private:
  std::array<std::uint32_t, NUM_CHARGE_READOUT> ti_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> liveTime_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> integralLiveTime_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> eventFlags_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> flagPseudo_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> flagNotrigForcedTrigger_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> flagSelfTrig_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> driftTime_{};
  std::uint32_t lightTriggerIndex_ = 0;
  std::array<std::uint32_t, NUM_CHARGE_READOUT> unixTime_{};
  std::array<std::array<std::uint32_t, 3>, NUM_CHARGE_READOUT> arrayChFlag_{};
  std::array<std::array<std::uint16_t, NUM_CH_CHARGE>, NUM_CHARGE_READOUT> arrayAdc_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> arrayRef_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> arrayCmn_{};
  std::array<std::array<std::uint16_t, NUM_CH_CHARGE>, NUM_CHARGE_READOUT> arrayIndex_{};
  std::array<std::uint16_t, NUM_CHARGE_READOUT> arrayHitnum_{};
  std::array<double, NUM_CHARGE_READOUT> arrayCmnEx_{};
  std::array<bool, NUM_CHARGE_READOUT> goodEvent_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> fecIndex_{};

  int numRegistered_ = 0;
  std::uint16_t errorFlags_ = 0;

  std::array<std::uint64_t, NUM_CH_LIGHT> realTime_{};
  std::array<std::uint8_t, NUM_CH_LIGHT> realTimePrecise_{};
  std::array<std::uint16_t, NUM_CH_LIGHT> ch_{};
  std::array<std::uint16_t, NUM_CH_LIGHT> qdc_{};
  std::array<std::uint16_t, NUM_CH_LIGHT> waveNum_{};
  std::array<std::array<std::int16_t, MAX_WAVENUM>, NUM_CH_LIGHT> waveData_{};
  std::array<bool, NUM_CH_LIGHT> isWaveList_{};
  std::uint32_t triggerid_ = 0;
  std::array<bool, NUM_CH_LIGHT> trigger_{};
  std::array<std::uint16_t, NUM_CH_LIGHT> waveCompress_{};
  std::array<std::uint32_t, NUM_CH_LIGHT> eventIdLight_{};
  std::array<std::uint32_t, NUM_CHARGE_READOUT> eventIdCharge_{};
};

using NanoGRAMSEvent_sptr = std::shared_ptr<NanoGRAMSEvent>;

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSEvent_H */
