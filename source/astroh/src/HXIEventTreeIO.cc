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

#include "HXIEventTreeIO.hh"
#include "TTree.h"

namespace comptonsoft
{

HXIEventTreeIO::HXIEventTreeIO()
  : tree_(nullptr)
{
}

HXIEventTreeIO::~HXIEventTreeIO() = default;

void HXIEventTreeIO::defineBranches()
{
  tree_->Branch("Time",                          &TIME_,                   "Time/D");
  tree_->Branch("SpacePacketTime",               &S_TIME_,                 "SpacePacketTime/D");
  tree_->Branch("ADUCount",                      &ADU_CNT_,                "ADUCount/b");
  tree_->Branch("CCSDSPacketTime",               &L32TI_,                  "CCSDSPacketTime/i");
  tree_->Branch("OccurrenceID",                  &OCCURRENCE_ID_,          "OccurrenceID/i");
  tree_->Branch("LocalTime",                     &LOCAL_TIME_,             "LocalTime/i");
  tree_->Branch("Category",                      &Category_,               "Category/b");
  tree_->Branch("Flag",                          &FLAGS_,                  "Flag/i");
  tree_->Branch("LiveTime",                      &LIVETIME_,               "LiveTime/i");
  tree_->Branch("NumberOfHitASICs",              &NUM_ASIC_,               "NumberOfHitASICs/b");
  tree_->Branch("RawASICDataSize",               &RawASICDataSize_,        "RawASICDataSize/s");
  tree_->Branch("RawASICData",                   RAW_ASIC_DATA,            "RawASICData[RawASICDataSize]/b");
  tree_->Branch("ProcessStatus",                 &PROC_STATUS_,            "ProcessStatus/i");
  tree_->Branch("Status",                        &STATUS_,                 "Status/i");

  tree_->Branch("ASICID",                        ASIC_ID_,                 "ASICID[NumberOfHitASICs]/b");
  tree_->Branch("RemappedASICID",                ASIC_ID_RMAP_,            "RemappedASICID[NumberOfHitASICs]/b");
  tree_->Branch("ChipDataBit",                   ASIC_CHIP_,               "ChipDataBit[NumberOfHitASICs]/b");
  tree_->Branch("AnalogTrigger",                 ASIC_TRIG_,               "AnalogTrigger[NumberOfHitASICs]/b");
  tree_->Branch("SEU",                           ASIC_SEU_,                "SEU[NumberOfHitASICs]/b");
  tree_->Branch("ChannelDataBit",                READOUT_FLAG_,            "ChannelDataBit[NumberOfHitASICs]/i");
  tree_->Branch("NumberOfHitChannelsOfEachASIC", NUM_READOUT_,             "NumberOfHitChannelForEachASIC[NumberOfHitASICs]/s");
  tree_->Branch("ReferenceChannel",              ASIC_REF_,                "ReferenceChannel[NumberOfHitASICs]/s");
  tree_->Branch("CommonModeNoise",               ASIC_CMN_,                "CommonModeNoise[NumberOfHitASICs]/s");
  
  tree_->Branch("NumberOfAllHitChannels",        &NumberOfAllHitChannels_, "NumberOfAllHitChannels/s");
  tree_->Branch("ASICIDOfEachChannel",           READOUT_ASIC_ID_,         "ASICIDOfEachChannel[NumberOfAllHitChannels]/b");
  tree_->Branch("ChannelID",                     READOUT_ID_,              "ChannelID[NumberOfAllHitChannels]/b");
  tree_->Branch("RemappedChannelID",             READOUT_ID_RMAP_,         "RemappedChannelID[NumberOfAllHitChannels]/s");
  tree_->Branch("ADCValue",                      PHA_,                     "ADCValue[NumberOfAllHitChannels]/s");
  tree_->Branch("CalibratedADCValue",            EPI_,                     "CalibratedADCValue[NumberOfAllHitChannels]/F");
}

void HXIEventTreeIO::setBranchAddresses()
{
  tree_->SetBranchAddress("Time",                          &TIME_);
  tree_->SetBranchAddress("SpacePacketTime",               &S_TIME_);
  tree_->SetBranchAddress("ADUCount",                      &ADU_CNT_);
  tree_->SetBranchAddress("CCSDSPacketTime",               &L32TI_);
  tree_->SetBranchAddress("OccurrenceID",                  &OCCURRENCE_ID_);
  tree_->SetBranchAddress("LocalTime",                     &LOCAL_TIME_);
  tree_->SetBranchAddress("Category",                      &Category_);
  tree_->SetBranchAddress("Flag",                          &FLAGS_);
  tree_->SetBranchAddress("LiveTime",                      &LIVETIME_);
  tree_->SetBranchAddress("NumberOfHitASICs",              &NUM_ASIC_);
  tree_->SetBranchAddress("RawASICDataSize",               &RawASICDataSize_);
  tree_->SetBranchAddress("RawASICData",                   RAW_ASIC_DATA_);
  tree_->SetBranchAddress("ProcessStatus",                 &PROC_STATUS_);
  tree_->SetBranchAddress("Status",                        &STATUS_);

  tree_->SetBranchAddress("ASICID",                        ASIC_ID_);
  tree_->SetBranchAddress("RemappedASICID",                ASIC_ID_RMAP_);
  tree_->SetBranchAddress("ChipDataBit",                   ASIC_CHIP_);
  tree_->SetBranchAddress("AnalogTrigger",                 ASIC_TRIG_);
  tree_->SetBranchAddress("SEU",                           ASIC_SEU_);
  tree_->SetBranchAddress("ChannelDataBit",                READOUT_FLAG_);
  tree_->SetBranchAddress("NumberOfHitChannelsOfEachASIC", NUM_READOUT_);
  tree_->SetBranchAddress("ReferenceChannel",              ASIC_REF_);
  tree_->SetBranchAddress("CommonModeNoise",               ASIC_CMN_);
  
  tree_->SetBranchAddress("NumberOfAllHitChannels",        &NumberOfAllHitChannels_);
  tree_->SetBranchAddress("ASICIDOfEachChannel",           READOUT_ASIC_ID_);
  tree_->SetBranchAddress("ChannelID",                     READOUT_ID_);
  tree_->SetBranchAddress("RemappedChannelID",             READOUT_ID_RMAP_);
  tree_->SetBranchAddress("ADCValue",                      PHA_);
  tree_->SetBranchAddress("CalibratedADCValue",            EPI_);
}

void HXIEventTreeIO::fillHits(const HXIEvent& event)
{
  TIME_ = event.



    tree_->Fill();
}

DetectorHit_sptr HXIEventTreeIO::retrieveHit(int64_t entry)
{
  hittree_->GetEntry(entry);

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

  return hit;
}

} /* namespace comptonsoft */
