#include "SGDEventTree.hh"

namespace astroh {
namespace sgd {

EventTreeIOHelper::EventTreeIOHelper(TTree* tree)
  : _tree(tree)
{
}

EventTreeIOHelper::~EventTreeIOHelper() = default;

void EventTreeIOHelper::fillEvent(const sgd::Event& event)
{
  TIME_ = event.getTime();
  S_TIME_ = event.getSTime();
  ADU_CNT_ = event.getADUCount();
  L32TI_ = event.getL32TI();
  OCCURRENCE_ID_ = event.getOccurrenceID();
  LOCAL_TIME_ = event.getLocalTime();
  FLAGS_ = event.getFlags().get();
  LIVETIME_ = event.getLiveTime();
  NUM_ASIC_ = event.getNumberOfHitASICs();
  
  RawASICDataSize_ = event.getRawASICData().size();
  copyVectorToArray(event.getRawASICData(), RAW_ASIC_DATA_);
  PROC_STATUS_ = event.getProcessStatus();
  
  copyVectorToArray(event.getASICIDVector(), ASIC_ID_);
  copyVectorToArray(event.getASICIDRemappedVector(), ASIC_ID_RMAP_);
  copyVectorToArray(event.getChipDataBitVector(), ASIC_CHIP_);
  copyVectorToArray(event.getTriggerVector(), ASIC_TRIG_);
  copyVectorToArray(event.getSEUVector(), ASIC_SEU_);
  copyVectorToArray(event.getChannelDataBitVector(), READOUT_FLAG_);
  copyVectorToArray(event.getNumberOfHitChannelsVector(), NUM_READOUT_);
  copyVectorToArray(event.getReferenceLevelVector(), ASIC_REF_);
  copyVectorToArray(event.getCommonModeNoiseVector(), ASIC_CMN_);
  
  NumberOfAllHitChannels_ = event.LengthOfReadoutData();
  copyVectorToArray(event.getReadoutASICIDVector(), READOUT_ASIC_ID_);
  copyVectorToArray(event.getReadoutChannelIDVector(), READOUT_ID_);
  copyVectorToArray(event.getReadoutChannelIDRemappedVector(), READOUT_ID_RMAP_);
  copyVectorToArray(event.getPHAVector(), PHA_);
  copyVectorToArray(event.getEPIVector(), EPI_);
  
  _tree->Fill();
}

void EventTreeIOHelper::restoreEvent(Long64_t entry, sgd::Event& event)
{
  _tree->GetEntry(entry);
  
  event.setTime(TIME_);
  event.setSTime(S_TIME_);
  event.setADUCount(ADU_CNT_);
  event.setL32TI(L32TI_);
  event.setOccurrenceID(OCCURRENCE_ID_);
  event.setLocalTime(LOCAL_TIME_);
  event.setFlags(FLAGS_);
  event.setLiveTime(LIVETIME_);
  event.setNumberOfHitASICs(NUM_ASIC_);
  event.setRawASICData(std::vector<uint8_t>(RAW_ASIC_DATA_.begin(),
                                            RAW_ASIC_DATA_.begin()+RawASICDataSize_));
  event.setProcessStatus(PROC_STATUS_);
  
  const int NumASICs = NUM_ASIC_;
  event.reserveASICData(NumASICs);
  for (int i=0; i<NumASICs; i++) {
    const int16_t ASIC_ID = ASIC_ID_[i];
    const uint8_t ASIC_ID_remapped = ASIC_ID_RMAP_[i];
    const uint8_t chipDataBit = ASIC_CHIP_[i];
    const uint8_t trigger = ASIC_TRIG_[i];
    const uint8_t SEU = ASIC_SEU_[i];
    const uint64_t channelDataBit = READOUT_FLAG_[i];
    const int16_t numberOfHitChannels = NUM_READOUT_[i];
    const int16_t referenceLevel = ASIC_REF_[i];
    const int16_t commonModeNoise = ASIC_CMN_[i];
    event.pushASICData(ASIC_ID,
                       ASIC_ID_remapped,
                       chipDataBit,
                       trigger,
                       SEU,
                       channelDataBit,
                       numberOfHitChannels,
                       referenceLevel,
                       commonModeNoise);
  }
  
  const int NumReadouts = NumberOfAllHitChannels_;
  event.reserveReadoutData(NumReadouts);
  for (int i=0; i<NumReadouts; i++) {
    const int16_t ASIC_ID = READOUT_ASIC_ID_[i];
    const uint8_t channelID = READOUT_ID_[i];
    const int16_t channelID_remapped = READOUT_ID_RMAP_[i];
    const int16_t PHA = PHA_[i];
    const float EPI = EPI_[i];
    event.pushReadoutData(ASIC_ID,
                          channelID,
                          channelID_remapped,
                          PHA,
                          EPI);
  }
}

std::shared_ptr<sgd::Event> EventTreeIOHelper::getEvent(Long64_t entry)
{
  std::shared_ptr<sgd::Event> event = std::make_shared<sgd::Event>();
  restoreEvent(entry, *event);
  return event;
}

void EventTreeIOHelper::makeLeaves()
{
  _tree->Branch("Time", &TIME_, "Time/D");
  _tree->Branch("SpacePacketTime", &S_TIME_, "SpacePacketTime/D");
  _tree->Branch("ADUCount", &ADU_CNT_, "ADUCount/s");
  _tree->Branch("CCSDSPacketTime", &L32TI_, "CCSDSPacketTime/i");
  _tree->Branch("OccurrenceID", &OCCURRENCE_ID_, "OccurrenceID/i");
  _tree->Branch("LocalTime", &LOCAL_TIME_, "LocalTime/i");
  _tree->Branch("Flag", &FLAGS_, "Flag/l");
  _tree->Branch("LiveTime", &LIVETIME_, "LiveTime/i");
  _tree->Branch("NumberOfHitASICs", &NUM_ASIC_, "NumberOfHitASICs/S");
  _tree->Branch("RawASICDataSize", &RawASICDataSize_, "RawASICDataSize/s");
  _tree->Branch("RawASICData", RAW_ASIC_DATA_.data(), "RawASICData[RawASICDataSize]/b");
  _tree->Branch("ProcessStatus", &PROC_STATUS_, "ProcessStatus/i");
  _tree->Branch("ASICID", ASIC_ID_.data(), "ASICID[NumberOfHitASICs]/s");
  _tree->Branch("RemappedASICID", ASIC_ID_RMAP_.data(), "RemappedASICID[NumberOfHitASICs]/b");
  _tree->Branch("ChipDataBit", ASIC_CHIP_.data(), "ChipDataBit[NumberOfHitASICs]/b");
  _tree->Branch("AnalogTrigger", ASIC_TRIG_.data(), "AnalogTrigger[NumberOfHitASICs]/b");
  _tree->Branch("SEU", ASIC_SEU_.data(), "SEU[NumberOfHitASICs]/b");
  _tree->Branch("ChannelDataBit", READOUT_FLAG_.data(), "ChannelDataBit[NumberOfHitASICs]/l");
  _tree->Branch("NumberOfHitChannelsOfEachASIC", NUM_READOUT_.data(), "NumberOfHitChannelForEachASIC[NumberOfHitASICs]/s");
  _tree->Branch("ReferenceChannel", ASIC_REF_.data(), "ReferenceChannel[NumberOfHitASICs]/s");
  _tree->Branch("CommonModeNoise", ASIC_CMN_.data(), "CommonModeNoise[NumberOfHitASICs]/s");
  _tree->Branch("NumberOfAllHitChannels", &NumberOfAllHitChannels_, "NumberOfAllHitChannels/s");
  _tree->Branch("ASICIDOfEachChannel", READOUT_ASIC_ID_.data(), "ASICIDOfEachChannel[NumberOfAllHitChannels]/s");
  _tree->Branch("ChannelID", READOUT_ID_.data(), "ChannelID[NumberOfAllHitChannels]/b");
  _tree->Branch("RemappedChannelID", READOUT_ID_RMAP_.data(), "RemappedChannelID[NumberOfAllHitChannels]/s");
  _tree->Branch("ADCValue", PHA_.data(), "ADCValue[NumberOfAllHitChannels]/s");
  _tree->Branch("CalibratedADCValue", EPI_.data(), "CalibratedADCValue[NumberOfAllHitChannels]/s");
}

void EventTreeIOHelper::setAddresses()
{
  _tree->SetBranchAddress("Time", &TIME_);
  _tree->SetBranchAddress("SpacePacketTime", &S_TIME_);
  _tree->SetBranchAddress("ADUCount", &ADU_CNT_);
  _tree->SetBranchAddress("CCSDSPacketTime", &L32TI_);
  _tree->SetBranchAddress("OccurrenceID", &OCCURRENCE_ID_);
  _tree->SetBranchAddress("LocalTime", &LOCAL_TIME_);
  _tree->SetBranchAddress("Flag", &FLAGS_);
  _tree->SetBranchAddress("LiveTime", &LIVETIME_);
  _tree->SetBranchAddress("NumberOfHitASICs", &NUM_ASIC_);
  _tree->SetBranchAddress("RawASICDataSize", &RawASICDataSize_);
  _tree->SetBranchAddress("RawASICData", RAW_ASIC_DATA_.data());
  _tree->SetBranchAddress("ProcessStatus", &PROC_STATUS_);
  _tree->SetBranchAddress("ASICID", ASIC_ID_.data());
  _tree->SetBranchAddress("RemappedASICID", ASIC_ID_RMAP_.data());
  _tree->SetBranchAddress("ChipDataBit", ASIC_CHIP_.data());
  _tree->SetBranchAddress("AnalogTrigger", ASIC_TRIG_.data());
  _tree->SetBranchAddress("SEU", ASIC_SEU_.data());
  _tree->SetBranchAddress("ChannelDataBit", READOUT_FLAG_.data());
  _tree->SetBranchAddress("NumberOfHitChannelsOfEachASIC", NUM_READOUT_.data());
  _tree->SetBranchAddress("ReferenceChannel", ASIC_REF_.data());
  _tree->SetBranchAddress("CommonModeNoise", ASIC_CMN_.data());
  _tree->SetBranchAddress("NumberOfAllHitChannels", &NumberOfAllHitChannels_);
  _tree->SetBranchAddress("ASICIDOfEachChannel", READOUT_ASIC_ID_.data());
  _tree->SetBranchAddress("ChannelID", READOUT_ID_.data());
  _tree->SetBranchAddress("RemappedChannelID", READOUT_ID_RMAP_.data());
  _tree->SetBranchAddress("ADCValue", PHA_.data());
  _tree->SetBranchAddress("CalibratedADCValue", EPI_.data());
}

} // namespace sgd
} // namespace astroh
