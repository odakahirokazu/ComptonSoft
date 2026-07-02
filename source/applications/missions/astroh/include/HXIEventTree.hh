/*
 * HXIEventTree.hpp
 *
 *  Created on: 2013/01/29
 *      Author: sakurai
 */

#ifndef HXIEVENTTREE_HPP_
#define HXIEVENTTREE_HPP_

#include "HXIEvent.hpp"

#include "TTree.h"

#include <iostream>

namespace hxi {

class EventTree: public TTree {
private:
	double TIME = 0.0;
	double S_TIME = 0.0;
	uint16_t ADU_CNT = 0;
	uint32_t L32TI = 0;
	uint32_t OCCURRENCE_ID = 0;

	uint32_t LOCAL_TIME = 0;

	uint32_t flag;

	uint32_t LIVETIME = 0;
	uint8_t NUM_ASIC = 0;

	uint16_t RawASICDataSize;
	uint8_t RAW_ASIC_DATA[MaxSizeOfRawASICDATA];
	uint32_t PROC_STATUS = 0;

	uint8_t ASIC_ID[MaxNASIC];
	uint8_t ASIC_ID_RMAP[MaxNASIC];
	uint8_t ASIC_CHIP[MaxNASIC];
	uint8_t ASIC_TRIG[MaxNASIC];
	uint8_t ASIC_SEU[MaxNASIC];
	uint32_t READOUT_FLAG[MaxNASIC];
	uint16_t NUM_READOUT[MaxNASIC];
	uint16_t ASIC_REF[MaxNASIC];
	uint16_t ASIC_CMN[MaxNASIC];

	uint16_t NumberOfAllHitChannels;
	uint8_t READOUT_ASIC_ID[MaxNChannel];
	uint8_t READOUT_ID[MaxNChannel];
	uint16_t READOUT_ID_RMAP[MaxNChannel];
	uint16_t PHA[MaxNChannel];
	uint16_t EPI[MaxNChannel];

	void setLeaves() {
		this->Branch("Time", &TIME, "Time/D");
		this->Branch("SpacePacketTime", &S_TIME, "SpacePacketTime/D");

		this->Branch("ADUCount", &ADU_CNT, "ADUCount/s");
		this->Branch("CCSDSPacketTime", &L32TI, "CCSDSPacketTime/i");
		this->Branch("OccurrenceID", &OCCURRENCE_ID, "OccurrenceID/i");
		this->Branch("LocalTime", &LOCAL_TIME, "LocalTime/i");
		this->Branch("Flag", &flag, "Flag/i");
		this->Branch("LiveTime", &LIVETIME, "LiveTime/i");

		this->Branch("NumberOfHitASICs", &NUM_ASIC, "NumberOfHitASICs/b");

		this->Branch("RawASICDataSize", &RawASICDataSize, "RawASICDataSize/s");
		this->Branch("RawASICData", RAW_ASIC_DATA, "RawASICData[RawASICDataSize]/b");

		this->Branch("ProcessStatus", &PROC_STATUS, "ProcessStatus/i");

		this->Branch("ASICID", ASIC_ID, "ASICID[NumberOfHitASICs]/b");
		this->Branch("RemappedASICID", ASIC_ID_RMAP, "RemappedASICID[NumberOfHitASICs]/b");
		this->Branch("ChipDataBit", ASIC_CHIP, "ChipDataBit[NumberOfHitASICs]/b");
		this->Branch("AnalogTrigger", ASIC_TRIG, "AnalogTrigger[NumberOfHitASICs]/b");
		this->Branch("SEU", ASIC_SEU, "SEU[NumberOfHitASICs]/b");
		this->Branch("ChannelDataBit", READOUT_FLAG, "ChannelDataBit[NumberOfHitASICs]/i");
		this->Branch("NumberOfHitChannelsOfEachASIC", NUM_READOUT, "NumberOfHitChannelForEachASIC[NumberOfHitASICs]/s");
		this->Branch("ReferenceChannel", ASIC_REF, "ReferenceChannel[NumberOfHitASICs]/s");
		this->Branch("CommonModeNoise", ASIC_CMN, "CommonModeNoise[NumberOfHitASICs]/s");

		this->Branch("NumberOfAllHitChannels", &NumberOfAllHitChannels, "NumberOfAllHitChannels/s");
		this->Branch("ASICIDOfEachChannel", &READOUT_ASIC_ID, "ASICIDOfEachChannel[NumberOfAllHitChannels]/b");
		this->Branch("ChannelID", &READOUT_ID, "ChannelID[NumberOfAllHitChannels]/b");
		this->Branch("RemappedChannelID", &READOUT_ID_RMAP, "RemappedChannelID[NumberOfAllHitChannels]/s");
		this->Branch("ADCValue", &PHA, "ADCValue[NumberOfAllHitChannels]/s");
		this->Branch("CalibratedADCValue", &EPI, "CalibratedADCValue[NumberOfAllHitChannels]/s");
	}

public:
	EventTree(const char* name, const char* title, Int_t splitlevel = 99) :
			TTree(name, title, splitlevel) {
		this->setLeaves();
	}

	void fillEvent(const hxi::Event& event) {
		TIME = event.getTime();
		S_TIME = event.getSpacePacketTime();
		ADU_CNT = event.getADUCount();
		L32TI = event.getCCSDSPacketTime();
		OCCURRENCE_ID = event.getOccurrenceID();
		LOCAL_TIME = event.getLocalTime();
		flag = event.getFlag().get();
		LIVETIME = event.getLiveTime();
		NUM_ASIC = event.getNumberOfHitASICs();

		RawASICDataSize = event.getRawASICData().size();
		this->copyVectorToArray(event.getRawASICData(), RAW_ASIC_DATA);
		PROC_STATUS = event.getProcessStatus();

		this->copyVectorToArray(event.getASICID(), ASIC_ID);
		this->copyVectorToArray(event.getRemappedASICID(), ASIC_ID_RMAP);
		this->copyVectorToArray(event.getChipDataBit(), ASIC_CHIP);
		this->copyVectorToArray(event.getAnalogTrigger(), ASIC_TRIG);
		this->copyVectorToArray(event.getSEU(), ASIC_SEU);
		this->copyVectorToArray(event.getChannelDataBit(), READOUT_FLAG);
		this->copyVectorToArray(event.getNumberOfHitChannelsOfEachASIC(), NUM_READOUT);
		this->copyVectorToArray(event.getReferenceChannel(), ASIC_REF);
		this->copyVectorToArray(event.getCommonModeNoise(), ASIC_CMN);

		NumberOfAllHitChannels = event.getASICIDOfEachChannel().size();
		;
		this->copyVectorToArray(event.getASICIDOfEachChannel(), READOUT_ASIC_ID);
		this->copyVectorToArray(event.getChannelID(), READOUT_ID);
		this->copyVectorToArray(event.getRemappedChannelID(), READOUT_ID_RMAP);
		this->copyVectorToArray(event.getADCValue(), PHA);
		this->copyVectorToArray(event.getCalibratedADCValue(), EPI);

		this->Fill();
	}

	static uint64_t restoreEvents(TTree* tree, hxi::Events& events) {
		using namespace std;
		double TIME = 0.0;
		double S_TIME = 0.0;
		uint16_t ADU_CNT = 0;
		uint32_t L32TI = 0;
		uint32_t OCCURRENCE_ID = 0;

		uint32_t LOCAL_TIME = 0;

		uint32_t flag;

		uint32_t LIVETIME = 0;
		uint8_t NUM_ASIC = 0;

		uint16_t RawASICDataSize;
		uint8_t RAW_ASIC_DATA[MaxSizeOfRawASICDATA];
		uint32_t PROC_STATUS = 0;

		uint8_t ASIC_ID[MaxNASIC];
		uint8_t ASIC_ID_RMAP[MaxNASIC];
		uint8_t ASIC_CHIP[MaxNASIC];
		uint8_t ASIC_TRIG[MaxNASIC];
		uint8_t ASIC_SEU[MaxNASIC];
		uint32_t READOUT_FLAG[MaxNASIC];
		uint16_t NUM_READOUT[MaxNASIC];
		uint16_t ASIC_REF[MaxNASIC];
		uint16_t ASIC_CMN[MaxNASIC];

		uint16_t NumberOfAllHitChannels;
		uint8_t READOUT_ASIC_ID[MaxNChannel];
		uint8_t READOUT_ID[MaxNChannel];
		uint16_t READOUT_ID_RMAP[MaxNChannel];
		uint16_t PHA[MaxNChannel];
		uint16_t EPI[MaxNChannel];

		tree->SetBranchAddress("Time", &TIME);
		tree->SetBranchAddress("SpacePacketTime", &S_TIME);

		tree->SetBranchAddress("ADUCount", &ADU_CNT);
		tree->SetBranchAddress("CCSDSPacketTime", &L32TI);
		tree->SetBranchAddress("OccurrenceID", &OCCURRENCE_ID);
		tree->SetBranchAddress("LocalTime", &LOCAL_TIME);
		tree->SetBranchAddress("Flag", &flag);
		tree->SetBranchAddress("LiveTime", &LIVETIME);

		tree->SetBranchAddress("NumberOfHitASICs", &NUM_ASIC);

		tree->SetBranchAddress("RawASICDataSize", &RawASICDataSize);
		tree->SetBranchAddress("RawASICData", RAW_ASIC_DATA);

		tree->SetBranchAddress("ProcessStatus", &PROC_STATUS);

		tree->SetBranchAddress("ASICID", ASIC_ID);
		tree->SetBranchAddress("RemappedASICID", ASIC_ID_RMAP);
		tree->SetBranchAddress("ChipDataBit", ASIC_CHIP);
		tree->SetBranchAddress("AnalogTrigger", ASIC_TRIG);
		tree->SetBranchAddress("SEU", ASIC_SEU);
		tree->SetBranchAddress("ChannelDataBit", READOUT_FLAG);
		tree->SetBranchAddress("NumberOfHitChannelsOfEachASIC", NUM_READOUT);
		tree->SetBranchAddress("ReferenceChannel", ASIC_REF);
		tree->SetBranchAddress("CommonModeNoise", ASIC_CMN);

		tree->SetBranchAddress("NumberOfAllHitChannels", &NumberOfAllHitChannels);
		tree->SetBranchAddress("ASICIDOfEachChannel", &READOUT_ASIC_ID);
		tree->SetBranchAddress("ChannelID", &READOUT_ID);
		tree->SetBranchAddress("RemappedChannelID", &READOUT_ID_RMAP);
		tree->SetBranchAddress("ADCValue", &PHA);
		tree->SetBranchAddress("CalibratedADCValue", &EPI);

		uint64_t nentry = tree->GetEntries();
		for (uint64_t i = 0; i < nentry; i++) {
			tree->GetEntry(i);
			hxi::Event event;
			event.setTime(TIME);
			event.setSpacePacketTime(S_TIME);
			event.setADUCount(ADU_CNT);
			event.setCCSDSPacketTime(L32TI);
			event.setOccurrenceID(OCCURRENCE_ID);
			event.setLocalTime(LOCAL_TIME);
			event.setFlag(flag);
			event.setLiveTime(LIVETIME);
			event.setNumberOfHitASICs(NUM_ASIC);
			event.setRawASICData(RAW_ASIC_DATA, RawASICDataSize);
			event.setProcessStatus(PROC_STATUS);

			event.setASICID(ASIC_ID, NUM_ASIC);
			event.setRemappedASICID(ASIC_ID_RMAP, NUM_ASIC);
			event.setChipDataBit(ASIC_CHIP, NUM_ASIC);
			event.setAnalogTrigger(ASIC_TRIG, NUM_ASIC);
			event.setSEU(ASIC_SEU, NUM_ASIC);
			event.setChannelDataBit(READOUT_FLAG, NUM_ASIC);
			event.setNumberOfHitChannelsOfEachASIC(NUM_READOUT, NUM_ASIC);
			event.setReferenceChannel(ASIC_REF, NUM_ASIC);
			event.setCommonModeNoise(ASIC_CMN, NUM_ASIC);

			event.setASICIDOfEachChannel(READOUT_ASIC_ID, NumberOfAllHitChannels);
			event.setChannelID(READOUT_ID, NumberOfAllHitChannels);
			event.setRemappedChannelID(READOUT_ID_RMAP, NumberOfAllHitChannels);
			event.setADCValue(PHA, NumberOfAllHitChannels);
			event.setCalibratedADCValue(EPI, NumberOfAllHitChannels);

			events.push_back(event);
		}
		return nentry;
	}

	static void restoreOneEvent(TTree* tree, hxi::Event& event, const uint64_t ientry) {
		using namespace std;
		static double TIME = 0.0;
		static double S_TIME = 0.0;
		static uint16_t ADU_CNT = 0;
		static uint32_t L32TI = 0;
		static uint32_t OCCURRENCE_ID = 0;

		static uint32_t LOCAL_TIME = 0;

		static uint32_t flag;

		static uint32_t LIVETIME = 0;
		static uint8_t NUM_ASIC = 0;

		static uint16_t RawASICDataSize;
		static uint8_t RAW_ASIC_DATA[MaxSizeOfRawASICDATA];
		static uint32_t PROC_STATUS = 0;

		static uint8_t ASIC_ID[MaxNASIC];
		static uint8_t ASIC_ID_RMAP[MaxNASIC];
		static uint8_t ASIC_CHIP[MaxNASIC];
		static uint8_t ASIC_TRIG[MaxNASIC];
		static uint8_t ASIC_SEU[MaxNASIC];
		static uint32_t READOUT_FLAG[MaxNASIC];
		static uint16_t NUM_READOUT[MaxNASIC];
		static uint16_t ASIC_REF[MaxNASIC];
		static uint16_t ASIC_CMN[MaxNASIC];

		static uint16_t NumberOfAllHitChannels;
		static uint8_t READOUT_ASIC_ID[MaxNChannel];
		static uint8_t READOUT_ID[MaxNChannel];
		static uint16_t READOUT_ID_RMAP[MaxNChannel];
		static uint16_t PHA[MaxNChannel];
		static uint16_t EPI[MaxNChannel];

		tree->SetBranchAddress("Time", &TIME);
		tree->SetBranchAddress("SpacePacketTime", &S_TIME);

		tree->SetBranchAddress("ADUCount", &ADU_CNT);
		tree->SetBranchAddress("CCSDSPacketTime", &L32TI);
		tree->SetBranchAddress("OccurrenceID", &OCCURRENCE_ID);
		tree->SetBranchAddress("LocalTime", &LOCAL_TIME);
		tree->SetBranchAddress("Flag", &flag);
		tree->SetBranchAddress("LiveTime", &LIVETIME);

		tree->SetBranchAddress("NumberOfHitASICs", &NUM_ASIC);

		tree->SetBranchAddress("RawASICDataSize", &RawASICDataSize);
		tree->SetBranchAddress("RawASICData", RAW_ASIC_DATA);

		tree->SetBranchAddress("ProcessStatus", &PROC_STATUS);

		tree->SetBranchAddress("ASICID", ASIC_ID);
		tree->SetBranchAddress("RemappedASICID", ASIC_ID_RMAP);
		tree->SetBranchAddress("ChipDataBit", ASIC_CHIP);
		tree->SetBranchAddress("AnalogTrigger", ASIC_TRIG);
		tree->SetBranchAddress("SEU", ASIC_SEU);
		tree->SetBranchAddress("ChannelDataBit", READOUT_FLAG);
		tree->SetBranchAddress("NumberOfHitChannelsOfEachASIC", NUM_READOUT);
		tree->SetBranchAddress("ReferenceChannel", ASIC_REF);
		tree->SetBranchAddress("CommonModeNoise", ASIC_CMN);

		tree->SetBranchAddress("NumberOfAllHitChannels", &NumberOfAllHitChannels);
		tree->SetBranchAddress("ASICIDOfEachChannel", &READOUT_ASIC_ID);
		tree->SetBranchAddress("ChannelID", &READOUT_ID);
		tree->SetBranchAddress("RemappedChannelID", &READOUT_ID_RMAP);
		tree->SetBranchAddress("ADCValue", &PHA);
		tree->SetBranchAddress("CalibratedADCValue", &EPI);

		tree->GetEntry(ientry);

		event.setTime(TIME);
		event.setSpacePacketTime(S_TIME);
		event.setADUCount(ADU_CNT);
		event.setCCSDSPacketTime(L32TI);
		event.setOccurrenceID(OCCURRENCE_ID);
		event.setLocalTime(LOCAL_TIME);
		event.setFlag(flag);
		event.setLiveTime(LIVETIME);
		event.setNumberOfHitASICs(NUM_ASIC);
		event.setRawASICData(RAW_ASIC_DATA, RawASICDataSize);
		event.setProcessStatus(PROC_STATUS);

		event.setASICID(ASIC_ID, NUM_ASIC);
		event.setRemappedASICID(ASIC_ID_RMAP, NUM_ASIC);
		event.setChipDataBit(ASIC_CHIP, NUM_ASIC);
		event.setAnalogTrigger(ASIC_TRIG, NUM_ASIC);
		event.setSEU(ASIC_SEU, NUM_ASIC);
		event.setChannelDataBit(READOUT_FLAG, NUM_ASIC);
		event.setNumberOfHitChannelsOfEachASIC(NUM_READOUT, NUM_ASIC);
		event.setReferenceChannel(ASIC_REF, NUM_ASIC);
		event.setCommonModeNoise(ASIC_CMN, NUM_ASIC);

		event.setASICIDOfEachChannel(READOUT_ASIC_ID, NumberOfAllHitChannels);
		event.setChannelID(READOUT_ID, NumberOfAllHitChannels);
		event.setRemappedChannelID(READOUT_ID_RMAP, NumberOfAllHitChannels);
		event.setADCValue(PHA, NumberOfAllHitChannels);
		event.setCalibratedADCValue(EPI, NumberOfAllHitChannels);

	}

private:
	template<typename T>
	void copyVectorToArray(const std::vector<T>& vec, T* array) const {
		uint32_t size = vec.size();
		for (uint32_t i = 0; i < size; i++) {
			array[i] = vec[i];
		}
	}
};

}
#endif /* HXIEVENTTREE_HPP_ */
