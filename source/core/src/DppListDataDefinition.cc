#include "DppListDataDefinition.hh"
// #include "TerminalColoring.hh"
#include <iomanip>
#include <iostream>

namespace ngUtil {
DppListDataDefinition::DppListDataDefinition(int version) {
  waveData_.reserve(MAX_WAVENUM);
  version_ = version;
  if (version_ == 2) {
    sizeWaveHeader_ = SIZE_WAVE_HEADER_V2;
  }
  else if (version_ == 1) {
    sizeWaveHeader_ = SIZE_WAVE_HEADER;
  }
  else {
    std::cerr << "Unknown version(" << version_ << ")" << std::endl;
  }
}
int DppListDataDefinition::Interpret(const std::deque<char> &data, const std::array<uint16_t, NUM_CH> &wave_num_array) {
  waveData_.clear();
  waveNum_ = 0;
  waveCompress_ = 0;
  triggerid_ = 0;
  trigger_ = 0;
  header_ = 0;
  isValid_ = false;
  isFatal_ = false;
  if (data.size() < SIZE_LIST_DATA) {
    if (verbose_ > 0)
      std::cerr << "Data size(" << data.size() << ") is too small (required " << SIZE_LIST_DATA << ")" << std::endl;
    return 0;
  }
  if (verbose_ > 3) {
    std::cout << "data: ";
    for (int i = 0; i < SIZE_LIST_DATA; ++i) {
      std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(data[i] & 0xff) << std::setfill(' ') << " ";
    }
    std::cout << std::endl;
  }
  const char wav = (data[0] >> 7) & 0x1;
  if (verbose_ > 3)
    std::cout << "wav: " << std::hex << static_cast<int>(wav) << std::dec << std::endl;
  if (wav == 0) {
    isWaveList_ = false;
    return InterpretList(data);
  }
  else if (wav == 1) {
    isWaveList_ = true;
    return InterpretListWave(data, wave_num_array);
  }
  else {
    std::cerr << "Unknown data format." << std::endl;
    return 0;
  }
}
int DppListDataDefinition::InterpretList(const std::deque<char> &data) {
  isValid_ = false;
  if (data.size() < 10) {
    if (verbose_ > 3)
      std::cerr << "Data size is too small." << std::endl;
    isValid_ = false;
    return 0;
  }
  long long tm = (static_cast<long long>(data[0]) << 47) & 0x7F0000000000LL;
  for (int i = 1; i < 6; ++i) {
    tm += (static_cast<long long>(data[i]) << (47 - 8 * i)) & (0xFFLL << (47 - 8 * i));
  }
  tm += (static_cast<long long>(data[6]) >> 1) & 0x7FLL;
  realTime_ = tm;
  realTimePrecise_ = ((static_cast<int>(data[6]) << 7) & 0x80) + ((static_cast<int>(data[7]) >> 1) & 0x7F);
  if (verbose_ > 3)
    std::cout << "real time: " << realTime_ << " " << static_cast<int>(realTimePrecise_) << std::endl;
  ch_ = ((static_cast<int>(data[7]) << 3) & 0x8) + ((static_cast<int>(data[8]) >> 5) & 0x7);
  if (verbose_ > 3)
    std::cout << "channel: " << ch_ << std::endl;
  if (ch_ > MAX_CHANNEL - 1 || ch_ < 0) {
    std::cerr << "Channel number(" << ch_ << ") is invalid." << std::endl;
    isValid_ = false;
    isFatal_ = true;
    return 0;
  }
  qdc_ = ((static_cast<unsigned int>(data[8]) << 8) & 0x1F00) + static_cast<unsigned int>(data[9]);
  if (verbose_ > 3)
    std::cout << "QDC: " << qdc_ << std::endl;
  isValid_ = true;
  return SIZE_LIST_DATA;
}
int DppListDataDefinition::InterpretListWave(const std::deque<char> &data, const std::array<uint16_t, NUM_CH> &wave_num_array) {
  int ret_list = InterpretList(data);
  const size_t sz = data.size();
  isValid_ = false;
  if (ret_list <= 0) {
    return 0;
  }
  if (sz < 12) {
    if (verbose_ > 0)
      std::cerr << "Data size(" << sz << ") is too small (required " << SIZE_LIST_DATA + sizeWaveHeader_ + waveNum_ * 2 << ")" << std::endl;
    isValid_ = false;
    return 0;
  }
  waveNum_ = ((static_cast<int>(data[10]) << 8) & 0xff00) + (static_cast<int>(data[11]) & 0xff);
  if (verbose_ > 2)
    std::cout << "waveform number: " << waveNum_ << std::endl;
  if (verbose_ > 4) {
    std::cout << "data[10]: " << std::setfill('0') << std::setw(4) << std::hex << static_cast<int>(static_cast<int>(data[10]) & 0xff) << std::setfill(' ') << " data[11]: " << static_cast<int>(static_cast<int>(data[11]) & 0xff) << std::dec << std::setfill(' ') << std::endl;
  }
  if (waveNum_ > MAX_WAVENUM) {
    if ((strictFlag_ & 1) == 1) {
      std::cerr << "Waveform size(" << waveNum_ << ") is too large." << std::endl;
      std::cerr << std::hex << static_cast<int>(data[10]) << " " << static_cast<int>(data[11]) << std::dec << std::endl;
      isValid_ = false;
      isFatal_ = true;
      return 0;
    }
    else if (waveNum_ != wave_num_array[ch_] * 8) { // 8 comes from the configuration.
      std::cerr << terminal::yellow << "Warning: " << terminal::reset << "Waveform number(" << waveNum_ << ") is not consistent with the configuration(" << wave_num_array[ch_] * 8 << ")" << std::endl;
      waveNum_ = wave_num_array[ch_] * 8;
      hasWarning_ = true;
    }
  }
  if (sz < static_cast<size_t>(SIZE_LIST_DATA + sizeWaveHeader_ + waveNum_ * 2)) {
    if (verbose_ > 2)
      std::cerr << "Data size(" << sz << ") is too small (required " << static_cast<size_t>(SIZE_LIST_DATA + sizeWaveHeader_ + waveNum_ * 2) << ")" << std::endl;
    isValid_ = false;
    return 0;
  }
  if (verbose_ > 0)
    std::cout << "Data Size Correct" << std::endl;
  if (version_ == 1) {
    header_ = ((static_cast<int>(data[12]) << 24) & 0xff000000) + ((static_cast<int>(data[13]) << 16) & 0x00ff0000) + ((static_cast<int>(data[14]) << 8) & 0x0000ff00) + (static_cast<int>(data[15]) & 0x000000ff);
    if (verbose_ > 3)
      std::cout << "header: " << std::hex << header_ << std::dec << std::endl;
    if (!CheckHeader(ch_, header_)) {
      std::cerr << terminal::red << "Error: " << terminal::reset << "Header (" << std::hex << header_ << std::dec << ") is wrong" << std::endl;
      isValid_ = false;
      isFatal_ = true;
      return SIZE_LIST_DATA + sizeWaveHeader_ + waveNum_ * 2;
    }
    trigger_ = 0;
    waveCompress_ = 1;
    triggerid_ = 0;
  }
  else {
    triggerid_ = ((static_cast<long long>(data[12]) << 24) & 0xff000000) + (static_cast<long long>(data[13] << 16) & 0x00ff0000) + (static_cast<long long>(data[14] << 8) & 0x0000ff00) + (static_cast<long long>(data[15]) & 0x000000ff);
    trigger_ = ((data[16] & 0xe0) >> 7) == 1;
    waveCompress_ = ((static_cast<int>(data[16]) & 0x7f) << 8) + ((static_cast<int>(data[17]) & 0xff));
    header_ = 0;
    if (verbose_ > 4) {
      std::cout << "Trigger ID: " << triggerid_ << std::endl;
      std::cout << "Trigger: " << trigger_ << std::endl;
      std::cout << "Wave Compress: " << waveCompress_ << std::endl;
    }
  }
  waveData_.clear();
  for (int i = 0; i < waveNum_; ++i) {
    const int16_t wave_value = static_cast<int>(((static_cast<unsigned int>(data[SIZE_LIST_DATA + sizeWaveHeader_ + 2 * i]) << 8) & 0xFF00) + (static_cast<unsigned int>(data[SIZE_LIST_DATA + sizeWaveHeader_ + 2 * i + 1]) & 0x00FF) - 16384);
    if (wave_value < -8191 || wave_value > 8191) {
      if ((strictFlag_ & 2) == 2) {
        std::cerr << terminal::red << "Error: " << terminal::reset << "Waveform value(" << wave_value << ") is out of range in index " << i << std::endl;
        isValid_ = false;
        isFatal_ = true;
      }
      else {
        std::cerr << terminal::yellow << "Warning: " << terminal::reset << "Waveform value(" << wave_value << ") is out of range." << std::endl;
        hasWarning_ = true;
      }
    }
    waveData_.push_back(wave_value);
  }
  if (!isFatal_) {
    isValid_ = true;
  }
  return SIZE_LIST_DATA + sizeWaveHeader_ + waveNum_ * 2;
}
bool DppListDataDefinition::CheckHeader(int channel, int header) {
  if (channel > MAX_CHANNEL - 1) {
    std::cerr << "Channel number(" << channel << ") is too large." << std::endl;
    return false;
  }
  if (header != CHANNEL_HEADER_START + channel) {
    std::cerr << "Header (0x" << std::hex << header << std::dec << ") is not consistent with channel information (" << channel << ")" << std::endl;
    return false;
  }
  return true;
}
} // namespace ngUtil