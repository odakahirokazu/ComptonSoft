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

#include "HitTreeIO.hh"
#include "TTree.h"
#include "DetectorHit.hh"

namespace comptonsoft
{

HitTreeIO::HitTreeIO()
  : hittree_(nullptr)
{
}

HitTreeIO::~HitTreeIO() = default;

void HitTreeIO::defineBranches()
{
  hittree_->Branch("eventid",        &eventid_,        "eventid/L");
  hittree_->Branch("ihit",           &ihit_,           "ihit/S");
  hittree_->Branch("num_hits",       &num_hits_,       "num_hits/I");
  
  // measured data
  hittree_->Branch("time",           &time_,           "time/L");
  hittree_->Branch("instrument",     &instrument_,     "instrument/S");
  hittree_->Branch("detector",       &detector_,       "detector/S");
  hittree_->Branch("det_section",    &det_section_,    "det_section/S");
  hittree_->Branch("readout_module", &readout_module_, "readout_module/S");
  hittree_->Branch("section",        &section_,        "section/S");
  hittree_->Branch("channel",        &channel_,        "channel/S");
  hittree_->Branch("pixelx",         &pixelx_,         "pixelx/S");
  hittree_->Branch("pixely",         &pixely_,         "pixely/S");
  hittree_->Branch("rawpha",         &rawpha_,         "rawpha/I");
  hittree_->Branch("pha",            &pha_,            "pha/F");
  hittree_->Branch("epi",            &epi_,            "epi/F");
  hittree_->Branch("flag_data",      &flag_data_,      "flag_data/l");
  hittree_->Branch("flags",          &flags_,          "flags/l");
  
  // simulation
  hittree_->Branch("real_time",      &real_time_,      "real_time/F");
  hittree_->Branch("time_group",     &time_group_,     "time_group/S");
  hittree_->Branch("real_posx",      &real_posx_,      "real_posx/F");
  hittree_->Branch("real_posy",      &real_posy_,      "real_posy/F");
  hittree_->Branch("real_posz",      &real_posz_,      "real_posz/F");
  hittree_->Branch("edep",           &edep_,           "edep/F");
  hittree_->Branch("echarge",        &echarge_,        "echarge/F");
  hittree_->Branch("process",        &process_,        "process/i");

  // reconstructed
  hittree_->Branch("energy",         &energy_,         "energy/F");
  hittree_->Branch("posx",           &posx_,           "posx/F");
  hittree_->Branch("posy",           &posy_,           "posy/F");
  hittree_->Branch("posz",           &posz_,           "posz/F");
  hittree_->Branch("local_posx",     &local_posx_,     "local_posx/F");
  hittree_->Branch("local_posy",     &local_posy_,     "local_posy/F");
  hittree_->Branch("local_posz",     &local_posz_,     "local_posz/F");
  hittree_->Branch("grade",          &grade_,          "grade/I");
}

void HitTreeIO::setBranchAddresses()
{
  hittree_->SetBranchAddress("eventid",        &eventid_);
  hittree_->SetBranchAddress("ihit",           &ihit_);
  hittree_->SetBranchAddress("num_hits",       &num_hits_);

  // measured data
  hittree_->SetBranchAddress("time",           &time_);
  hittree_->SetBranchAddress("instrument",     &instrument_);
  hittree_->SetBranchAddress("detector",       &detector_);
  hittree_->SetBranchAddress("det_section",    &det_section_);
  hittree_->SetBranchAddress("readout_module", &readout_module_);
  hittree_->SetBranchAddress("section",        &section_);
  hittree_->SetBranchAddress("channel",        &channel_);
  hittree_->SetBranchAddress("pixelx",         &pixelx_);
  hittree_->SetBranchAddress("pixely",         &pixely_);
  hittree_->SetBranchAddress("rawpha",         &rawpha_);
  hittree_->SetBranchAddress("pha",            &pha_);
  hittree_->SetBranchAddress("epi",            &epi_);
  hittree_->SetBranchAddress("flag_data",      &flag_data_);
  hittree_->SetBranchAddress("flags",          &flags_);

  // simulation
  hittree_->SetBranchAddress("real_time",      &real_time_);
  hittree_->SetBranchAddress("time_group",     &time_group_);
  hittree_->SetBranchAddress("real_posx",      &real_posx_);
  hittree_->SetBranchAddress("real_posy",      &real_posy_);
  hittree_->SetBranchAddress("real_posz",      &real_posz_);
  hittree_->SetBranchAddress("edep",           &edep_);
  hittree_->SetBranchAddress("echarge",        &echarge_);
  hittree_->SetBranchAddress("process",        &process_);

  // reconstructed
  hittree_->SetBranchAddress("energy",         &energy_);
  hittree_->SetBranchAddress("posx",           &posx_);
  hittree_->SetBranchAddress("posy",           &posy_);
  hittree_->SetBranchAddress("posz",           &posz_);
  hittree_->SetBranchAddress("local_posx",     &local_posx_);
  hittree_->SetBranchAddress("local_posy",     &local_posy_);
  hittree_->SetBranchAddress("local_posz",     &local_posz_);
  hittree_->SetBranchAddress("grade",          &grade_);
}

void HitTreeIO::fillHits(const int64_t eventID,
                         const std::vector<DetectorHit_sptr>& hits)
{
  const int NumHits = hits.size();
  num_hits_ = NumHits;

  for (int i=0; i<NumHits; i++) {
    const DetectorHit_sptr& hit = hits[i];
    eventid_ = (eventID >= 0) ? eventID : hit->EventID();
    ihit_ = i;

    time_ = hit->Time();
    instrument_ = hit->InstrumentID();
    detector_ = hit->DetectorID();
    det_section_ = hit->DetectorChannelSection();
    readout_module_ = hit->ReadoutModuleID();
    section_ = hit->ReadoutChannelSection();
    channel_ = hit->ReadoutChannelIndex();
    pixelx_ = hit->PixelX();
    pixely_ = hit->PixelY();
    rawpha_ = hit->RawPHA();
    pha_ = hit->PHA();
    epi_ = hit->EPI();
    flag_data_ = hit->FlagData();
    flags_ = hit->Flags();
    real_time_ = hit->RealTime();
    time_group_ = hit->TimeGroup();
    real_posx_ = hit->RealPositionX();
    real_posy_ = hit->RealPositionY();
    real_posz_ = hit->RealPositionZ();
    edep_ = hit->EnergyDeposit();
    echarge_ = hit->EnergyCharge();
    process_ = hit->Process();
    energy_ = hit->Energy();
    posx_ = hit->PositionX();
    posy_ = hit->PositionY();
    posz_ = hit->PositionZ();
    local_posx_ = hit->LocalPositionX();
    local_posy_ = hit->LocalPositionY();
    local_posz_ = hit->LocalPositionZ();
    grade_ = hit->Grade();
    
    hittree_->Fill();
  }
}

DetectorHit_sptr HitTreeIO::retrieveHit() const
{
  DetectorHit_sptr hit(new DetectorHit);
  hit->setEventID(eventid_);
  hit->setTime(time_);
  hit->setInstrumentID(instrument_);
  hit->setDetectorChannel(detector_, det_section_, channel_);
  hit->setReadoutChannel(readout_module_, section_, channel_);
  hit->setPixel(pixelx_, pixely_);
  hit->setRawPHA(rawpha_);
  hit->setPHA(pha_);
  hit->setEPI(epi_);
  hit->setFlagData(flag_data_);
  hit->setFlags(flags_);
  hit->setRealTime(real_time_);
  hit->setTimeGroup(time_group_);
  hit->setRealPosition(real_posx_, real_posy_, real_posz_);
  hit->setEnergyDeposit(edep_);
  hit->setEnergyCharge(echarge_);
  hit->setProcess(process_);
  hit->setEnergy(energy_);
  hit->setPosition(posx_, posy_, posz_);
  hit->setLocalPosition(local_posx_, local_posy_, local_posz_);
  hit->setGrade(grade_);

  return std::move(hit);
}

std::vector<DetectorHit_sptr> HitTreeIO::retrieveHits(int64_t& entry,
                                                      bool get_first_entry)
{
  std::vector<DetectorHit_sptr> hits;

  if (get_first_entry) {
    hittree_->GetEntry(entry);
  }
  
  const int numHits = getNumberOfHits();
  for (int i=0; i<numHits; i++) {
    if (i!=0) {
      hittree_->GetEntry(entry+i);
    }

    DetectorHit_sptr hit = retrieveHit();
    hits.push_back(std::move(hit));
  }
  entry += numHits;
  return std::move(hits);
}

} /* namespace comptonsoft */
