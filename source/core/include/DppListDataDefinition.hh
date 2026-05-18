#ifndef DppListDataDefinition_hh
#define DppListDataDefinition_hh 1
#include "DppParameters.hh"
#include <deque>
#include <vector>
namespace ngUtil {
/**s
 * @brief Class for ROOT I/O of Grouped Dpp List Data 
 * @author Shota Arai
 * @date 2024/11/XX First version
 * @date 2025/01/12 This class is not used in the current version
 */
class DppListDataDefinition {
private:
  static constexpr int MAX_WAVENUM = 8000;
  static constexpr int SIZE_LIST_DATA = 10;
  static constexpr int SIZE_WAVE_HEADER = 6;
  static constexpr int SIZE_WAVE_HEADER_V2 = 8;
  static constexpr int CHANNEL_HEADER_START = 0x57415630;
  static constexpr int MAX_CHANNEL = 8;
  uint32_t eventid_ = 0;
  uint64_t realTime_ = 0;
  uint8_t realTimePrecise_ = 0;
  uint16_t ch_ = 0;
  uint16_t qdc_ = 0;
  uint16_t waveNum_ = 0;
  int header_ = 0;
  std::vector<int16_t> waveData_;
  bool isWaveList_ = true;
  bool isValid_ = false;
  bool isFatal_ = false;
  bool hasWarning_ = false;
  uint32_t triggerid_ = 0;
  bool trigger_ = 0;
  uint16_t waveCompress_ = 0;
  int verbose_ = 0;
  int version_ = 2;
  int sizeWaveHeader_ = SIZE_WAVE_HEADER_V2;
  /// @brief Flag for strictness of the data interpretation
  /// @details Default: Basic error (channel, header, consistency of the data size). These are essential for the data interpretation.
  /// 1th bit: wavenum, which is allowed between 1 and MAX_WAVENUM (8000).
  /// 2th bit: wave data value range, which is allowed between -8191 and 8191.
  unsigned int strictFlag_ = 1;

public:
  DppListDataDefinition(int version = 2);
  ~DppListDataDefinition() = default;
  DppListDataDefinition(const DppListDataDefinition &r) = default;

private:
  int InterpretList(const std::deque<char> &data);
  int InterpretListWave(const std::deque<char> &data, const std::array<uint16_t, NUM_CH> &wave_num_array);

public:
  void SetStrictFlag(unsigned int strictFlag) { strictFlag_ = strictFlag; }
  int Interpret(const std::deque<char> &data, const std::array<uint16_t, NUM_CH> &wave_num_array);
  void SetVerbose(int verbose) { verbose_ = verbose; }
  void SetEventID(uint64_t eventid) { eventid_ = eventid; }
  uint64_t GetEventID() const { return eventid_; }
  uint64_t GetRealTime() const { return realTime_; }
  void SetRealTime(uint64_t realTime) { realTime_ = realTime; }
  uint8_t GetRealTimePrecise() const { return realTimePrecise_; }
  void SetRealTimePrecise(uint8_t realTimePrecise) { realTimePrecise_ = realTimePrecise; }
  uint16_t GetChannel() const { return ch_; }
  void SetChannel(uint16_t ch) { ch_ = ch; }
  uint16_t GetQDC() const { return qdc_; }
  void SetQDC(uint16_t qdc) { qdc_ = qdc; }
  uint16_t GetWaveNum() const { return waveNum_; }
  void SetWaveNum(uint16_t waveNum) { waveNum_ = waveNum; }
  int GetHeader() const { return header_; }
  void SetHeader(int header) { header_ = header; }
  const std::vector<int16_t> &GetWaveData() const { return waveData_; }
  void SetWaveData(const std::vector<int16_t> &waveData) { waveData_ = waveData; }
  void SetWaveData(const std::array<int16_t, MAX_WAVENUM> &waveData) {
    waveData_.assign(waveData.begin(), waveData.end());
  }
  bool IsWaveList() const { return isWaveList_; }
  void SetWaveList(bool isWaveList) { isWaveList_ = isWaveList; }
  bool IsValid() const { return isValid_; }
  void SetValid(bool isValid) { isValid_ = isValid; }
  bool IsFatal() const { return isFatal_; }
  void SetFatal(bool isFatal) { isFatal_ = isFatal; }
  bool HasWarning() const { return hasWarning_; }
  void SetWarning(bool hasWarning) { hasWarning_ = hasWarning; }
  uint32_t GetTriggerID() const { return triggerid_; }
  void SetTriggerID(uint32_t triggerid) { triggerid_ = triggerid; }
  bool GetTrigger() const { return trigger_; }
  void SetTrigger(bool trigger) { trigger_ = trigger; }
  uint16_t GetWaveCompress() const { return waveCompress_; }
  void SetWaveCompress(uint16_t waveCompress) { waveCompress_ = waveCompress; }
  static bool CheckHeader(int channel, int header);
  int GetVersion() const { return version_; }
};

} // namespace ngUtil

#endif //DppListDataDefinition_hh