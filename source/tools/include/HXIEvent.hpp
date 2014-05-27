/*
 * HXIEvent.hpp
 *
 *  Created on: 2013/02/01
 *      Author: Soki Sakurai
 */

#ifndef HXIEVENT_HPP_
#define HXIEVENT_HPP_

//#include <cstdint>
#include <iostream>
#include <vector>

namespace hxi {

class EventFlag {
//	friend class HXIEventTree;
private:
	uint32_t flag;
public:
	EventFlag(uint32_t flag = 0) {
		this->flag = flag;
	}

	void set(uint32_t flag) {
		this->flag = flag;
	}

	uint32_t get() const {
		return flag;
	}

	//FLAG_FASTBGO in FITS
	uint8_t getFastBGO() const {
		return (flag & 0x3);
	}

	//FLAG_HITPAT in FITS
	uint8_t getHitPattern() const {
		return ((flag >> 2) & 0x3);
	}

	//FLAG_TRIGPAT in FITS
	uint8_t getTriggerPattern() const {
		return ((flag >> 4) & 0xff);
	}

	//FLAG_TRIG in FITS
	uint8_t getTrigger() const {
		return ((flag >> 12) & 0xff);
	}

	//FLAG_LCHK in FITS
	uint8_t getLengthCheck() const {
		return ((flag >> 20) & 0x1f);
	}

	//FLAG_SEU in FITS
	uint8_t getSEU() const {
		return ((flag >> 26) & 0x1f);
	}
};

class Event {
//	friend class HXIEventTree;
private:
	double TIME = 0.0;
	double S_TIME = 0.0;
	uint16_t ADU_CNT = 0;
	uint32_t L32TI = 0;
	uint32_t OCCURRENCE_ID = 0;
	//event: several hits	physical
	//occurence: 1 read-out	readout
	//hit: several signal (~2 for Strip Detector) physical
	//signal: 1 adc value	readout

	uint32_t LOCAL_TIME = 0;
	EventFlag flag;
	uint32_t LIVETIME = 0;
	uint8_t NUM_ASIC = 0;
	std::vector<uint8_t> RAW_ASIC_DATA;
	uint32_t PROC_STATUS = 0;
	std::vector<uint8_t> ASIC_ID;
	std::vector<uint8_t> ASIC_ID_RMAP;
	std::vector<uint8_t> ASIC_CHIP;
	std::vector<uint8_t> ASIC_TRIG;
	std::vector<uint8_t> ASIC_SEU;
	std::vector<uint32_t> READOUT_FLAG;
	std::vector<uint16_t> NUM_READOUT;
	std::vector<uint16_t> ASIC_REF;
	std::vector<uint16_t> ASIC_CMN;
	std::vector<uint8_t> READOUT_ASIC_ID;
	std::vector<uint8_t> READOUT_ID;
	std::vector<uint16_t> READOUT_ID_RMAP;
	std::vector<uint16_t> PHA;
	std::vector<uint16_t> EPI;

public:
	Event() {
		this->clearASICData();
		this->clearChannelData();
	}

	void pushASICData(uint8_t ASICID, uint8_t chipDataBit, uint8_t analogTrigger, uint8_t SEU, uint32_t channelDataBit,
			uint8_t numberOfHitChannels, uint16_t referenceChannel, uint16_t commonModeNoise) {
		this->ASIC_ID.push_back(ASICID);
//		this->ASIC_ID_RMAP.push_back((ASICID % 0x10) * 8 + (ASICID >> 4));
		this->ASIC_ID_RMAP.push_back(0);
		this->ASIC_CHIP.push_back(chipDataBit);
		this->ASIC_TRIG.push_back(analogTrigger);
		this->ASIC_SEU.push_back(SEU);
		this->READOUT_FLAG.push_back(channelDataBit);
		this->NUM_READOUT.push_back(numberOfHitChannels);
		this->ASIC_REF.push_back(referenceChannel);
		this->ASIC_CMN.push_back(commonModeNoise);
//		this->hitASICs += 1;
	}

	void clearASICData() {
		this->ASIC_ID.clear();
		this->ASIC_ID_RMAP.clear();
		this->ASIC_CHIP.clear();
		this->ASIC_TRIG.clear();
		this->ASIC_SEU.clear();
		this->READOUT_FLAG.clear();
		this->NUM_READOUT.clear();
		this->ASIC_REF.clear();
		this->ASIC_CMN.clear();
//		this->hitASICs = 0;
	}

	void reserveASICData(uint32_t size) {
		this->ASIC_ID.reserve(size);
		this->ASIC_ID_RMAP.reserve(size);
		this->ASIC_CHIP.reserve(size);
		this->ASIC_TRIG.reserve(size);
		this->ASIC_SEU.reserve(size);
		this->READOUT_FLAG.reserve(size);
		this->NUM_READOUT.reserve(size);
		this->ASIC_REF.reserve(size);
		this->ASIC_CMN.reserve(size);
	}

	void pushChannelData(uint8_t asicID, uint8_t channelID, uint16_t adcValue) {
		this->READOUT_ASIC_ID.push_back(asicID);
		this->READOUT_ID.push_back(channelID);
//		if(this->READOUT_ID.size()==1){
//			std::cerr << "!!!!! " << (int)(this->READOUT_ID[0]) << std::endl;
//		}
//		this->READOUT_ID_RMAP.push_back(ilayer*256+asicNumber*32+channelNumber);
		this->READOUT_ID_RMAP.push_back(0);
		this->PHA.push_back(adcValue);
//		this->hitChannels += 1;
	}

	void clearChannelData() {
		this->READOUT_ASIC_ID.clear();
		this->READOUT_ID.clear();
		this->READOUT_ID_RMAP.clear();
		this->PHA.clear();
	}

	void reserveChannelData(uint32_t size) {
		this->READOUT_ASIC_ID.reserve(size);
		this->READOUT_ID.reserve(size);
		this->READOUT_ID_RMAP.reserve(size);
		this->PHA.reserve(size);
	}

public:
	double getTime() const {
		return TIME;
	}
	void setTime(double time) {
		TIME = time;
	}

	double getSpacePacketTime() const {
		return S_TIME;
	}
	void setSpacePacketTime(double s_time) {
		S_TIME = s_time;
	}

	uint16_t getADUCount() const {
		return ADU_CNT;
	}
	void setADUCount(uint16_t aduCount) {
		ADU_CNT = aduCount;
	}

	uint32_t getCCSDSPacketTime() const {
		return L32TI;
	}
	void setCCSDSPacketTime(uint32_t ccsds_time) {
		L32TI = ccsds_time;
	}

	uint32_t getOccurrenceID() const {
		return OCCURRENCE_ID;
	}
	void setOccurrenceID(uint32_t occurence_id) {
		OCCURRENCE_ID = occurence_id;
	}

	uint32_t getLocalTime() const {
		return LOCAL_TIME;
	}
	void setLocalTime(uint32_t localTime) {
		this->LOCAL_TIME = localTime;
	}

	const EventFlag& getFlag() const {
		return flag;
	}
	void setFlag(const EventFlag& flag) {
		this->flag = flag;
	}
	void setFlag(uint32_t flagData) {
		this->flag.set(flagData);
	}

	uint32_t getLiveTime() const {
		return LIVETIME;
	}

	void setLiveTime(uint32_t liveTime) {
		this->LIVETIME = liveTime;
	}

	uint8_t getNumberOfHitASICs() const {
		return NUM_ASIC;
	}
	void setNumberOfHitASICs(uint8_t hitASICs) {
		this->NUM_ASIC = hitASICs;
	}

	std::vector<uint8_t> getRawASICData() const {
		return RAW_ASIC_DATA;
	}
	void setRawASICData(std::vector<uint8_t>& data) {
		RAW_ASIC_DATA = data;
	}
	void setRawASICData(const uint8_t* data, uint32_t size) {
		RAW_ASIC_DATA.clear();
        for(uint32_t i=0;i<size;i++){
            RAW_ASIC_DATA.push_back(data[i]);
        }
	}

	uint32_t getProcessStatus() const {
		return PROC_STATUS;
	}
	void setProcessStatus(uint32_t process_status) {
		this->PROC_STATUS = process_status;
	}

	//STATUS <-- no longer exists
	const std::vector<uint8_t>& getASICID() const {
		return ASIC_ID;
	}
	void setASICID(const std::vector<uint8_t>& asicid) {
		this->ASIC_ID = asicid;
	}
	void setASICID(const uint8_t* asicid, const uint32_t nasic) {
		this->ASIC_ID.clear();
		this->ASIC_ID.reserve(nasic);
		for(uint32_t i=0;i<nasic;i++){
			this->ASIC_ID.push_back(asicid[i]);
		}
	}

	const std::vector<uint8_t>& getRemappedASICID() const {
		return ASIC_ID_RMAP;
	}
	void setRemappedASICID(const std::vector<uint8_t>& remapped_asicid) {
		this->ASIC_ID_RMAP = remapped_asicid;
	}
	void setRemappedASICID(const uint8_t* remapped_asicid, const uint32_t nasic) {
		this->ASIC_ID_RMAP.clear();
		this->ASIC_ID_RMAP.reserve(nasic);
		for(uint32_t i=0;i<nasic;i++){
			this->ASIC_ID_RMAP.push_back(remapped_asicid[i]);
		}
	}

	const std::vector<uint8_t>& getChipDataBit() const {
		return ASIC_CHIP;
	}
	void setChipDataBit(const std::vector<uint8_t>& chip_data) {
		this->ASIC_CHIP = chip_data;
	}
	void setChipDataBit(const uint8_t* chip_data, const uint32_t nasic) {
		this->ASIC_CHIP.clear();
		this->ASIC_CHIP.reserve(nasic);
		for(uint32_t i=0;i<nasic;i++){
			this->ASIC_CHIP.push_back(chip_data[i]);
		}
	}

	const std::vector<uint8_t>& getAnalogTrigger() const {
		return ASIC_TRIG;
	}
	void setAnalogTrigger(const std::vector<uint8_t>& analog_trigger) {
		this->ASIC_TRIG = analog_trigger;
	}
	void setAnalogTrigger(const uint8_t* analog_trigger, const uint32_t nasic) {
		this->ASIC_TRIG.clear();
		this->ASIC_TRIG.reserve(nasic);
		for(uint32_t i=0;i<nasic;i++){
			this->ASIC_TRIG.push_back(analog_trigger[i]);
		}
	}

	const std::vector<uint8_t>& getSEU() const {
		return ASIC_SEU;
	}
	void setSEU(const std::vector<uint8_t>& SEU) {
		this->ASIC_SEU = SEU;
	}
	void setSEU(const uint8_t* SEU, const uint32_t nasic) {
		this->ASIC_SEU.clear();
		this->ASIC_SEU.reserve(nasic);
		for(uint32_t i=0;i<nasic;i++){
			this->ASIC_SEU.push_back(SEU[i]);
		}
	}

	const std::vector<uint32_t>& getChannelDataBit() const {
		return READOUT_FLAG;
	}
	void setChannelDataBit(const std::vector<uint32_t>& channel_data_bit) {
		this->READOUT_FLAG = channel_data_bit;
	}
	void setChannelDataBit(const uint32_t* channel_data_bit, const uint32_t nasic) {
		this->READOUT_FLAG.clear();
		this->READOUT_FLAG.reserve(nasic);
		for(uint32_t i=0;i<nasic;i++){
			this->READOUT_FLAG.push_back(channel_data_bit[i]);
		}
	}

	const std::vector<uint16_t>& getNumberOfHitChannelsOfEachASIC() const {
		return NUM_READOUT;
	}
	void setNumberOfHitChannelsOfEachASIC(const std::vector<uint16_t>& hit_channels) {
		this->NUM_READOUT = hit_channels;
	}
	void setNumberOfHitChannelsOfEachASIC(const uint16_t* hit_channels, const uint32_t nasic) {
		this->NUM_READOUT.clear();
		this->NUM_READOUT.reserve(nasic);
		for(uint32_t i=0;i<nasic;i++){
			this->NUM_READOUT.push_back(hit_channels[i]);
		}
	}

	const std::vector<uint16_t>& getReferenceChannel() const {
		return ASIC_REF;
	}
	void setReferenceChannel(const std::vector<uint16_t>& reference_channel) {
		this->ASIC_REF = reference_channel;
	}
	void setReferenceChannel(const uint16_t* reference_channel, const uint32_t nasic) {
		this->ASIC_REF.clear();
		this->ASIC_REF.reserve(nasic);
		for(uint32_t i=0;i<nasic;i++){
			this->ASIC_REF.push_back(reference_channel[i]);
		}
	}

	const std::vector<uint16_t>& getCommonModeNoise() const {
		return ASIC_CMN;
	}
	void setCommonModeNoise(const std::vector<uint16_t>& common_mode_noise) {
		this->ASIC_CMN = common_mode_noise;
	}
	void setCommonModeNoise(const uint16_t* common_mode_noise, const uint32_t nasic) {
		this->ASIC_CMN.clear();
		this->ASIC_CMN.reserve(nasic);
		for(uint32_t i=0;i<nasic;i++){
			this->ASIC_CMN.push_back(common_mode_noise[i]);
		}
	}

	const std::vector<uint8_t>& getASICIDOfEachChannel() const {
		return READOUT_ASIC_ID;
	}
	void setASICIDOfEachChannel(const std::vector<uint8_t>& asicid) {
		this->READOUT_ASIC_ID = asicid;
	}
	void setASICIDOfEachChannel(const uint8_t* asicid, const uint32_t nchannel) {
		this->READOUT_ASIC_ID.clear();
		this->READOUT_ASIC_ID.reserve(nchannel);
		for(uint32_t i=0;i<nchannel;i++){
			this->READOUT_ASIC_ID.push_back(asicid[i]);
		}
	}

	const std::vector<uint8_t>& getChannelID() const {
		return READOUT_ID;
	}
	void setChannelID(const std::vector<uint8_t>& channelID) {
		this->READOUT_ID = channelID;
	}
	void setChannelID(const uint8_t* channelID, const uint32_t nchannel) {
		this->READOUT_ID.clear();
		this->READOUT_ID.reserve(nchannel);
		for(uint32_t i=0;i<nchannel;i++){
			this->READOUT_ID.push_back(channelID[i]);
		}
	}

	const std::vector<uint16_t>& getRemappedChannelID() const {
		return READOUT_ID_RMAP;
	}
	void setRemappedChannelID(const std::vector<uint16_t>& remappedChannelID) {
		this->READOUT_ID_RMAP = remappedChannelID;
	}
	void setRemappedChannelID(const uint16_t* remappedChannelID, const uint32_t nchannel) {
		this->READOUT_ID_RMAP.clear();
		this->READOUT_ID_RMAP.reserve(nchannel);
		for(uint32_t i=0;i<nchannel;i++){
			this->READOUT_ID_RMAP.push_back(remappedChannelID[i]);
		}
	}

	const std::vector<uint16_t>& getADCValue() const {
		return PHA;
	}
	void setADCValue(const std::vector<uint16_t>& adc_value) {
		this->PHA = adc_value;
	}
	void setADCValue(const uint16_t* adc_value, const uint32_t nchannel) {
		this->PHA.clear();
		this->PHA.reserve(nchannel);
		for(uint32_t i=0;i<nchannel;i++){
			this->PHA.push_back(adc_value[i]);
		}
	}

	const std::vector<uint16_t>& getCalibratedADCValue() const {
		return EPI;
	}
	void setCalibratedADCValue(const std::vector<uint16_t>& calibrated_adc_value) {
		this->EPI = calibrated_adc_value;
	}
	void setCalibratedADCValue(const uint16_t* calibrated_adc_value, const uint32_t nchannel) {
		this->EPI.clear();
		this->EPI.reserve(nchannel);
		for(uint32_t i=0;i<nchannel;i++){
			this->EPI.push_back(calibrated_adc_value[i]);
		}
	}

};

}

#endif /* HXIEVENT_HPP_ */
