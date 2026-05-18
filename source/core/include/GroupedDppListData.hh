#ifndef GroupedDppData_hh
#define GroupedDppData_hh 1

#include "DppListDataDefinition.hh"
#include "DppParameters.hh"
#include <array>

class GroupedDppListData {
public:
  GroupedDppListData();
  virtual ~GroupedDppListData() = default;

private:
  std::array<uint32_t, NUM_CH> eventid_;
  std::array<uint64_t, NUM_CH> realTime_;
  std::array<uint8_t, NUM_CH> realTimePrecise_;
  std::array<uint16_t, NUM_CH> ch_;
  std::array<uint16_t, NUM_CH> qdc_;
  std::array<uint16_t, NUM_CH> waveNum_;
  std::array<int, NUM_CH> header_;
  std::array<std::vector<int16_t>, NUM_CH> waveData_;
  std::array<bool, NUM_CH> isWaveList_;
  bool isGrouped_ = false;
  bool isValid_ = true;
  bool isFatal_ = false;
  uint32_t triggerid_ = 0;
  std::array<bool, NUM_CH> trigger_;
  std::array<uint16_t, NUM_CH> waveCompress_;
  int verbose_ = 0;
  int numRegistered_ = 0;
  std::array<bool, NUM_CH> registered_;
  int version_ = 2;

public:
  void SetVerbose(int verbose) { verbose_ = verbose; }
  void SetGrouped(bool grouped) { isGrouped_ = grouped; }
  bool AddEvent(const DppListDataDefinition &event);
  bool IsValid() const { return isValid_; }
  bool IsFatal() const { return isFatal_; }

  // Getters
  bool IsGrouped() const { return isGrouped_; }
  int GetNumRegistered() const { return numRegistered_; }
  bool IsRegistered(int ch) const { return registered_[ch]; }
  uint64_t GetEventID(int ch) const { return eventid_[ch]; }
  const std::array<uint32_t, NUM_CH> &GetEventID() const { return eventid_; }
  uint64_t GetRealTime(int ch) const { return realTime_[ch]; }
  const std::array<uint64_t, NUM_CH> &GetRealTime() const { return realTime_; }
  uint8_t GetRealTimePrecise(int ch) const { return realTimePrecise_[ch]; }
  const std::array<uint8_t, NUM_CH> &GetRealTimePrecise() const { return realTimePrecise_; }
  uint8_t GetChannel(int ch) const { return ch_[ch]; }
  const std::array<uint16_t, NUM_CH> &GetChannel() const { return ch_; }
  uint16_t GetQDC(int ch) const { return qdc_[ch]; }
  const std::array<uint16_t, NUM_CH> &GetQDC() const { return qdc_; }
  uint16_t GetWaveNum(int ch) const { return waveNum_[ch]; }
  const std::array<uint16_t, NUM_CH> &GetWaveNum() const { return waveNum_; }
  int GetHeader(int ch) const { return header_[ch]; }
  const std::array<int, NUM_CH> &GetHeader() const { return header_; }
  const std::vector<int16_t> &GetWaveData(int ch) const { return waveData_[ch]; }
  const std::array<std::vector<int16_t>, NUM_CH> &GetWaveData() const { return waveData_; }
  bool IsTrigger(int ch) const { return trigger_[ch]; }
  const std::array<bool, NUM_CH> &GetTrigger() const { return trigger_; }
  uint16_t GetWaveCompress(int ch) const { return waveCompress_[ch]; }
  const std::array<uint16_t, NUM_CH> &GetWaveCompress() const { return waveCompress_; }
  uint32_t GetTriggerID() const { return triggerid_; }
  bool GetIsWaveList(int ch) const { return isWaveList_[ch]; }
  const std::array<bool, NUM_CH> &GetIsWaveList() const { return isWaveList_; }
  int GetVerbose() const { return verbose_; }
  int GetVersion() const { return version_; }
};

#endif //GroupedDppData_hh