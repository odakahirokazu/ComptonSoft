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

#include "TPCTreeIO.hh"

#include <TTree.h>

namespace comptonsoft
{

TPCTreeIO::TPCTreeIO() = default;

TPCTreeIO::~TPCTreeIO() = default;

void TPCTreeIO::SetBranchAddress(TTree* tree)
{
  tpctree_ = tree;
  if (!tpctree_) {
    return;
  }

  tpctree_->SetBranchAddress("hitnum", arrayHitnum_.data());
  tpctree_->SetBranchAddress("chflag", arrayChFlag_.data());
  tpctree_->SetBranchAddress("cmn", arrayCmn_.data());
  tpctree_->SetBranchAddress("adc", arrayAdc_.data());
  tpctree_->SetBranchAddress("index", arrayIndex_.data());
  tpctree_->SetBranchAddress("ref", arrayRef_.data());
  tpctree_->SetBranchAddress("cmn_ex", arrayCmnEx_.data());
  tpctree_->SetBranchAddress("ti", ti_.data());
  tpctree_->SetBranchAddress("event_id_charge", eventIdCharge_.data());
  tpctree_->SetBranchAddress("livetime", liveTime_.data());
  tpctree_->SetBranchAddress("integral_livetime", integralLiveTime_.data());
  tpctree_->SetBranchAddress("drift_time", driftTime_.data());
  tpctree_->SetBranchAddress("light_trigger_index", &lightTriggerIndex_);
  tpctree_->SetBranchAddress("unixtime", unixTime_.data());
  tpctree_->SetBranchAddress("event_flags", eventFlags_.data());
  tpctree_->SetBranchAddress("flag_pseudo", flagPseudo_.data());
  tpctree_->SetBranchAddress(
      "flag_notrig_forced_trigger", flagNotrigForcedTrigger_.data());
  tpctree_->SetBranchAddress("flag_self_trig", flagSelfTrig_.data());
  tpctree_->SetBranchAddress("fec_index", fecIndex_.data());
  tpctree_->SetBranchAddress("good_event", goodEvent_.data());

  tpctree_->SetBranchAddress("event_id_light", eventIdLight_.data());
  tpctree_->SetBranchAddress("real_time", realTime_.data());
  tpctree_->SetBranchAddress("real_time_frac", realTimePrecise_.data());
  tpctree_->SetBranchAddress("ch", ch_.data());
  tpctree_->SetBranchAddress("qdc", qdc_.data());
  tpctree_->SetBranchAddress("wave_num", waveNum_.data());
  tpctree_->SetBranchAddress("triggerid", &triggerid_);
  tpctree_->SetBranchAddress("trigger", trigger_.data());
  tpctree_->SetBranchAddress("wave_compress", waveCompress_.data());
  tpctree_->SetBranchAddress("is_wavelist", isWaveList_.data());
  tpctree_->SetBranchAddress("registered", registered_.data());
  tpctree_->SetBranchAddress("error_flags", &errorFlags_);
}

void TPCTreeIO::RetrieveEvent(NanoGRAMSEvent& event) const
{
  event.SetTI(ti_);
  event.SetLiveTime(liveTime_);
  event.SetIntegralLiveTime(integralLiveTime_);
  event.SetEventFlags(eventFlags_);
  event.SetFlagPseudo(flagPseudo_);
  event.SetFlagNotrigForcedTrigger(flagNotrigForcedTrigger_);
  event.SetFlagSelfTrig(flagSelfTrig_);
  event.SetDriftTime(driftTime_);
  event.SetLightTriggerIndex(lightTriggerIndex_);
  event.SetUnixTime(unixTime_);
  event.SetArrayChFlag(arrayChFlag_);
  event.SetArrayADC(arrayAdc_);
  event.SetArrayRef(arrayRef_);
  event.SetArrayCmn(arrayCmn_);
  event.SetArrayIndex(arrayIndex_);
  event.SetArrayHitNum(arrayHitnum_);
  event.SetArrayCmnEx(arrayCmnEx_);
  event.SetEventIDCharge(eventIdCharge_);
  event.SetGoodEvent(goodEvent_);
  event.SetFecIndex(fecIndex_);

  event.SetEventIDLight(eventIdLight_);
  event.SetRealTime(realTime_);
  event.SetRealTimePrecise(realTimePrecise_);
  event.SetChannel(ch_);
  event.SetQDC(qdc_);
  event.SetWaveNum(waveNum_);
  event.SetWaveData(waveData_);
  event.SetIsWaveList(isWaveList_);
  event.SetTriggerID(triggerid_);
  event.SetTrigger(trigger_);
  event.SetWaveCompress(waveCompress_);
}

} /* namespace comptonsoft */
