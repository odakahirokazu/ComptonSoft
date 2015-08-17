/** @file HXIEvent.hh
 * contains definitions of HXI event classes.
 *
 * @author Soki Sakurai
 * @author Hirokazu Odaka
 *
 * @date 2013-02-01 | S. Sakurai
 * @date 2015-02-08 | H. Odaka | Modification for ComptonSoft.
 *
 */

#ifndef COMPTONSOFT_EVENT_H
#define COMPTONSOFT_EVENT_H 1

#include <cstdint>
#include <cstddef>
#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>

namespace comptonsoft {

class HXIEventFlags
{
public:
  explicit HXIEventFlags(uint32_t flags=0)
    : flags_(flags)
  {
  }

  ~HXIEventFlags() = default;

  HXIEventFlags(const HXIEventFlags&) = default;
  HXIEventFlags(HXIEventFlags&&) = default;
  HXIEventFlags& operator=(const HXIEventFlags&) = default;
  HXIEventFlags& operator=(HXIEventFlags&&) = default;

  void set(uint32_t flags)
  { flags_ = flags;}

  uint32_t get() const
  { return flags_; }
  
	// FLAG_FASTBGO in FITS
	uint8_t getFastBGO() const
  { return (flags_ & 0x3); }
  
	// FLAG_HITPAT in FITS
	uint8_t getHitPattern() const
  { return ((flags_ >> 2) & 0x3); }

	// FLAG_TRIGPAT in FITS
	uint8_t getTriggerPattern() const
  { return ((flags_ >> 4) & 0xff); }

	// FLAG_TRIG in FITS
	uint8_t getTrigger() const
  { return ((flags_ >> 12) & 0xff); }

	// FLAG_LCHK in FITS
	uint8_t getLengthCheck() const
  { return ((flags_ >> 20) & 0x1f); }

	//FLAG_SEU in FITS
	uint8_t getSEU() const
  { return ((flags_ >> 26) & 0x1f); }

private:
	uint32_t flags_;
};


class HXIEvent
{
public:
  static const int MAX_NUM_ASIC = 40;
  static const int MAX_NUM_READOUT = 1280;
  static const int MAX_SIZE_RAW_ASIC_DATA = 2000;

  static uint8_t remapASICID(uint8_t ASIC_ID)
  {
    const uint8_t TrayNo = ASIC_ID % 0x10;
    const uint8_t ASICNo = ASIC_ID >> 4;
    const uint8_t ASICIDRemapped = TrayNo*8 + ASICNo + 1;
    return ASICIDRemapped;
  }

  static uint16_t remapChannelID(uint8_t ASIC_ID, uint16_t ChannelID)
  {
    const uint8_t TrayNo = ASIC_ID % 0x10;
    const uint8_t ASICNo = ASIC_ID >> 4;
    uint16_t channelRemapped(0u);
    if (ASICNo < 4) {
      channelRemapped = TrayNo*256 + (128 - ASICNo*32 - ChannelID);
    }
    else {
      channelRemapped = TrayNo*256 + ASICNo*32 + (32-ChannelID);
    }
    return channelRemapped;
  }
  
public:
	HXIEvent() = default;
  ~HXIEvent() = default;
  HXIEvent(const HXIEvent&) = default;
  HXIEvent(HXIEvent&&) = default;
  HXIEvent& operator=(const HXIEvent&) = default;
  HXIEvent& operator=(HXIEvent&&) = default;

  // FFF contents
  double getTime() const { return TIME_; }
  void setTime(double val) { TIME_ = val; }

	double getSpacePacketTime() const { return S_TIME;}
	void setSpacePacketTime(double val) { S_TIME_ = val; }

	uint8_t getADUCount() const { return ADU_CNT_; }
	void setADUCount(uint8_t val) { ADU_CNT_ = val; }

	uint32_t getL32TI() const { return L32TI_; }
	void setL32TI(uint32_t val) { L32TI_ = val; }

	int32_t getOccurrenceID() const { return OCCURRENCE_ID_; }
	void setOccurrenceID(int32_t val) { OCCURRENCE_ID_ = val; }

	uint32_t getLocalTime() const { return LOCAL_TIME_; }
	void setLocalTime(uint32_t val) {	LOCAL_TIME_ = val; }

	uint8_t getCategory() const { return Category_; }
	void setCategory(uint8_t val) { Category_ = val; }

	HXIEventFlags getFlags() const { return FLAGS_; }
	void setFlags(const HXIEventFlags& val) { FLAGS_ = val; }

  uint32_t getLiveTime() const { return LIVETIME_; }
  void setLiveTime(uint32_t val) { LIVETIME_ = val; }

  uint8_t getNumberOfASICs() const { return NUM_ASIC_; }
  void setNumberOfASICs(uint8_t val) { NUM_ASIC_ = val; }

	std::vector<uint8_t>& getRawASICData() const { return RAW_ASIC_DATA_; }
  void setRawASICData(const std::vector<uint8_t>& data)
  { RAW_ASIC_DATA_ = data; }
  void setRawASICData(std::vector<uint8_t>&& data)
  { RAW_ASIC_DATA_ = std::move(data); }

  void setRawASICData(const uint8_t* data, std::size_t size)
  {
		RAW_ASIC_DATA.resize(size);
    std::copy(data, data+size, std::begin(RAW_ASIC_DATA_));
	}

  uint32_t getProcessStatus() const { return PROC_STATUS_; }
  void setProcessStatus(uint32_t val) { PROC_STATUS_ = val; }

  uint8_t getProcessStatus() const { return STATUS_; }
  void setProcessStatus(uint8_t val) { STATUS_ = val; }

  // SFF contents (ASICs)
  const std::vector<uint8_t>& getASICIDVector() const
  { return ASIC_ID_; }
  const std::vector<uint8_t>& getASICIDRemappedVector() const
  { return ASIC_ID_RMAP_; }
	const std::vector<uint8_t>& getASICDataBitVector() const
  { return ASIC_CHIP_; }
  const std::vector<uint8_t>& getASICTriggerVector() const
  { return ASIC_TRIG_; }
	const std::vector<uint8_t>& getASICSEUVector() const
  { return ASIC_SEU_; }
  const std::vector<uint32_t>& getChannelDataBitsVector() const
  { return READOUT_FLAG_; }
  const std::vector<int16_t>& getNumberOfSignalsVector() const
  { return NUM_READOUT_; }
  const std::vector<int16_t>& getASICReference() const
  { return ASIC_REF_; }
  const std::vector<int16_t>& getASICCommonModeNoise() const
  { return ASIC_CMN_; }

  void pushASICData(uint8_t ASIC_ID,
                    uint8_t dataBit,
                    uint8_t trigger,
                    uint8_t SEU,
                    uint32_t channelDataBits,
                    int16_t numberOfSignals,
                    int16_t reference,
                    int16_t commonModeNoise)
  {
    ASIC_ID_.push_back(ASIC_ID);
    ASIC_ID_RMAP_.push_back(remapASICID(ASIC_ID));
    ASIC_CHIP_.push_back(dataBit);
    ASIC_TRIG_.push_back(trigger);
    ASIC_SEU_.push_back(SEU);
    Readout_FLAG_.push_back(channelDataBits);
    NUM_Readout_.push_back(numberOfSignals);
    ASIC_REF_.push_back(reference);
    ASIC_CMN_.push_back(commonModeNoise);
	}

  void clearASICData()
  {
    ASIC_ID_.clear();
    ASIC_ID_RMAP_.clear();
    ASIC_CHIP_.clear();
    ASIC_TRIG_.clear();
    ASIC_SEU_.clear();
    Readout_FLAG_.clear();
    NUM_Readout_.clear();
    ASIC_REF_.clear();
    ASIC_CMN_.clear();
  }

  void reserveASICData(std::size_t size)
  {
    ASIC_ID_.reserve(size);
    ASIC_ID_RMAP_.reserve(size);
    ASIC_CHIP_.reserve(size);
    ASIC_TRIG_.reserve(size);
    ASIC_SEU_.reserve(size);
    Readout_FLAG_.reserve(size);
    NUM_Readout_.reserve(size);
    ASIC_REF_.reserve(size);
    ASIC_CMN_.reserve(size);
	}

  // SFF contents (readout channels)
  const std::vector<uint8_t>& getReadoutASICIDVector() const
  { return READOUT_ASIC_ID_; }
  const std::vector<uint8_t>& getReadoutChannelIDVector() const
  { return READOUT_ID_; }
  const std::vector<int16_t>& getReadoutChannelIDRemappedVector() const
  {	return READOUT_ID_RMAP_; }
  const std::vector<int16_t>& getPHAVector() const
  { return PHA_; }
  const std::vector<float>& getEPIVector() const
  { return EPI_; }

  void pushChannelData(uint8_t ASIC_ID,
                       uint8_t channelID,
                       uint16_t pha,
                       float epi)
  {
    READOUT_ASIC_ID_.push_back(ASIC_ID);
    READOUT_ID_.push_back(channelID);
    READOUT_ID_RMAP_.push_back(remapChannelID(channelID));
    PHA_.push_back(pha);
    EPI_.push_back(epi);
	}

  void clearChannelData()
  {
    READOUT_ASIC_ID_.clear();
    READOUT_ID_.clear();
    READOUT_ID_RMAP_.clear();
    PHA_.clear();
    EPI_.clear();
  }

	void reserveChannelData(std::size_t size)
  {
    READOUT_ASIC_ID_.reserve(size);
    READOUT_ID_.reserve(size);
    READOUT_ID_RMAP_.reserve(size);
    PHA_.reserve(size);
    EPI_.reserve(size);
  }

private:
  // FFF contents
	double TIME_ = 0.0;
	double S_TIME_ = 0.0;
	uint8_t ADU_CNT_ = 0;
	uint32_t L32TI_ = 0;
	int32_t OCCURRENCE_ID_ = 0;
	uint32_t LOCAL_TIME_ = 0;
  uint8_t Category_ = 0;
  HXIEventFlags FLAGS_;
	uint32_t LIVETIME_ = 0;
	uint8_t NUM_ASIC_ = 0;
	std::vector<uint8_t> RAW_ASIC_DATA_;
	uint32_t PROC_STATUS_ = 0;
  uint8_t STATUS_ = 0;

  // SFF contents (ASICs)
  std::vector<uint8_t> ASIC_ID_;
	std::vector<uint8_t> ASIC_ID_RMAP_;
	std::vector<uint8_t> ASIC_CHIP_;
	std::vector<uint8_t> ASIC_TRIG_;
	std::vector<uint8_t> ASIC_SEU_;
	std::vector<uint32_t> Readout_FLAG_;
	std::vector<int16_t> NUM_Readout_;
	std::vector<int16_t> ASIC_REF_;
	std::vector<int16_t> ASIC_CMN_;

  // SFF contents (readout channels)
  std::vector<uint8_t> Readout_ASIC_ID_;
	std::vector<uint8_t> Readout_ID_;
	std::vector<int16_t> Readout_ID_RMAP_;
	std::vector<int16_t> PHA_;
	std::vector<float> EPI_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_EVENT_H */
