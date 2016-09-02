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

#include "EventTreeIO.hh"
#include "G4SystemOfUnits.hh"
#include "TTree.h"
#include "DetectorHit.hh"

namespace comptonsoft
{

EventTreeIO::EventTreeIO()
  : tree_(nullptr)
{
}

EventTreeIO::~EventTreeIO() = default;

void EventTreeIO::defineBranches()
{
  tree_->Branch("eventid",        &eventid_,              "eventid/L");
  tree_->Branch("num_hits",       &num_hits_,             "num_hits/I");
  
  // measured data
  tree_->Branch("time",           &time_,                 "time/L");
  tree_->Branch("instrument",     &instrument_,           "instrument/S");
  tree_->Branch("detector",       detector_.data(),       "detector[num_hits]/S");
  tree_->Branch("det_section",    det_section_.data(),    "det_section[num_hits]/S");
  tree_->Branch("readout_module", readout_module_.data(), "readout_module[num_hits]/S");
  tree_->Branch("section",        section_.data(),        "section[num_hits]/S");
  tree_->Branch("channel",        channel_.data(),        "channel[num_hits]/S");
  tree_->Branch("pixelx",         pixelx_.data(),         "pixelx[num_hits]/S");
  tree_->Branch("pixely",         pixely_.data(),         "pixely[num_hits]/S");
  tree_->Branch("rawpha",         rawpha_.data(),         "rawpha[num_hits]/I");
  tree_->Branch("pha",            pha_.data(),            "pha[num_hits]/F");
  tree_->Branch("epi",            epi_.data(),            "epi[num_hits]/F");
  tree_->Branch("flag_data",      &flag_data_,            "flag_data/l");
  tree_->Branch("flags",          &flags_,                "flags/l");
  
  // simulation
  tree_->Branch("real_time",      real_time_.data(),      "real_time[num_hits]/F");
  tree_->Branch("time_group",     time_group_.data(),     "time_group[num_hits]/S");
  tree_->Branch("real_posx",      real_posx_.data(),      "real_posx[num_hits]/F");
  tree_->Branch("real_posy",      real_posy_.data(),      "real_posy[num_hits]/F");
  tree_->Branch("real_posz",      real_posz_.data(),      "real_posz[num_hits]/F");
  tree_->Branch("edep",           edep_.data(),           "edep[num_hits]/F");
  tree_->Branch("echarge",        echarge_.data(),        "echarge[num_hits]/F");
  tree_->Branch("process",        process_.data(),        "process[num_hits]/i");

  // reconstructed
  tree_->Branch("energy",         energy_.data(),         "energy[num_hits]/F");
  tree_->Branch("posx",           posx_.data(),           "posx[num_hits]/F");
  tree_->Branch("posy",           posy_.data(),           "posy[num_hits]/F");
  tree_->Branch("posz",           posz_.data(),           "posz[num_hits]/F");
  tree_->Branch("local_posx",     local_posx_.data(),     "local_posx[num_hits]/F");
  tree_->Branch("local_posy",     local_posy_.data(),     "local_posy[num_hits]/F");
  tree_->Branch("local_posz",     local_posz_.data(),     "local_posz[num_hits]/F");
  tree_->Branch("grade",          &grade_,                "grade/I");
}

void EventTreeIO::setBranchAddresses()
{
  tree_->SetBranchAddress("eventid",        &eventid_);
  tree_->SetBranchAddress("num_hits",       &num_hits_);

  // measured data
  tree_->SetBranchAddress("time",           &time_);
  tree_->SetBranchAddress("instrument",     &instrument_);
  tree_->SetBranchAddress("detector",       detector_.data());
  tree_->SetBranchAddress("det_section",    det_section_.data());
  tree_->SetBranchAddress("readout_module", readout_module_.data());
  tree_->SetBranchAddress("section",        section_.data());
  tree_->SetBranchAddress("channel",        channel_.data());
  tree_->SetBranchAddress("pixelx",         pixelx_.data());
  tree_->SetBranchAddress("pixely",         pixely_.data());
  tree_->SetBranchAddress("rawpha",         rawpha_.data());
  tree_->SetBranchAddress("pha",            pha_.data());
  tree_->SetBranchAddress("epi",            epi_.data());
  tree_->SetBranchAddress("flag_data",      &flag_data_);
  tree_->SetBranchAddress("flags",          &flags_);

  // simulation
  tree_->SetBranchAddress("real_time",      real_time_.data());
  tree_->SetBranchAddress("time_group",     time_group_.data());
  tree_->SetBranchAddress("real_posx",      real_posx_.data());
  tree_->SetBranchAddress("real_posy",      real_posy_.data());
  tree_->SetBranchAddress("real_posz",      real_posz_.data());
  tree_->SetBranchAddress("edep",           edep_.data());
  tree_->SetBranchAddress("echarge",        echarge_.data());
  tree_->SetBranchAddress("process",        process_.data());

  // reconstructed
  tree_->SetBranchAddress("energy",         energy_.data());
  tree_->SetBranchAddress("posx",           posx_.data());
  tree_->SetBranchAddress("posy",           posy_.data());
  tree_->SetBranchAddress("posz",           posz_.data());
  tree_->SetBranchAddress("local_posx",     local_posx_.data());
  tree_->SetBranchAddress("local_posy",     local_posy_.data());
  tree_->SetBranchAddress("local_posz",     local_posz_.data());
  tree_->SetBranchAddress("grade",          &grade_);
}

void EventTreeIO::fillHits(const int64_t eventID,
                           const std::vector<DetectorHit_sptr>& hits)
{
  const int NumHits = hits.size();
  if (NumHits==0) return;

  num_hits_ = NumHits;

  const DetectorHit_sptr& hit = hits[0];
  eventid_ = (eventID >= 0) ? eventID : hit->EventID();
  time_ = hit->Time() / second;
  instrument_ = hit->InstrumentID();
  flag_data_ = hit->FlagData();
  flags_ = hit->Flags();
  grade_ = hit->Grade();

  for (int i=0; i<NumHits; i++) {
    const DetectorHit_sptr& hit = hits[i];
    detector_[i] = hit->DetectorID();
    det_section_[i] = hit->DetectorSection();
    readout_module_[i] = hit->ReadoutModuleID();
    section_[i] = hit->ReadoutSection();
    channel_[i] = hit->ReadoutChannel();
    pixelx_[i] = hit->PixelX();
    pixely_[i] = hit->PixelY();
    rawpha_[i] = hit->RawPHA();
    pha_[i] = hit->PHA();
    epi_[i] = hit->EPI() / keV;
    real_time_[i] = hit->RealTime() / second;
    time_group_[i] = hit->TimeGroup();
    real_posx_[i] = hit->RealPositionX() / cm;
    real_posy_[i] = hit->RealPositionY() / cm;
    real_posz_[i] = hit->RealPositionZ() / cm;
    edep_[i] = hit->EnergyDeposit() / keV;
    echarge_[i] = hit->EnergyCharge() / keV;
    process_[i] = hit->Process();
    energy_[i] = hit->Energy() / keV;
    posx_[i] = hit->PositionX() / cm;
    posy_[i] = hit->PositionY() / cm;
    posz_[i] = hit->PositionZ() / cm;
    local_posx_[i] = hit->LocalPositionX() / cm;
    local_posy_[i] = hit->LocalPositionY() / cm;
    local_posz_[i] = hit->LocalPositionZ() / cm;
  }

  tree_->Fill();
}

DetectorHit_sptr EventTreeIO::retrieveHit(std::size_t i) const
{
  DetectorHit_sptr hit(new DetectorHit);
  hit->setEventID(eventid_);
  hit->setTime(time_ * second);
  hit->setInstrumentID(instrument_);
  hit->setDetectorChannelID(detector_[i], det_section_[i], channel_[i]);
  hit->setReadoutChannelID(readout_module_[i], section_[i], channel_[i]);
  hit->setPixel(pixelx_[i], pixely_[i]);
  hit->setRawPHA(rawpha_[i]);
  hit->setPHA(pha_[i]);
  hit->setEPI(epi_[i] * keV);
  hit->setFlagData(flag_data_);
  hit->setFlags(flags_);
  hit->setRealTime(real_time_[i] * second);
  hit->setTimeGroup(time_group_[i]);
  hit->setRealPosition(real_posx_[i] * cm, real_posy_[i] * cm, real_posz_[i] * cm);
  hit->setEnergyDeposit(edep_[i] * keV);
  hit->setEnergyCharge(echarge_[i] * keV);
  hit->setProcess(process_[i]);
  hit->setEnergy(energy_[i] * keV);
  hit->setPosition(posx_[i] * cm, posy_[i] * cm, posz_[i] * cm);
  hit->setLocalPosition(local_posx_[i] * cm, local_posy_[i] * cm, local_posz_[i] * cm);
  hit->setGrade(grade_);

  return hit;
}

std::vector<DetectorHit_sptr> EventTreeIO::retrieveHits(int64_t& entry, bool get_entry)
{
  std::vector<DetectorHit_sptr> hits;

  if (get_entry) {
    tree_->GetEntry(entry);
  }
  
  const int numHits = getNumberOfHits();
  for (int i=0; i<numHits; i++) {
    DetectorHit_sptr hit = retrieveHit(i);
    hits.push_back(std::move(hit));
  }

  entry += 1;
  return hits;
}

} /* namespace comptonsoft */
