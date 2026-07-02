/** @file SGDEventFITS.hh
 * containes SGD event FITS classes.
 * 
 * @author Hirokazu Odaka
 * 
 * @date 2015-07-08 | H. Odaka
 *
 */

#ifndef ASTROH_SGD_EVENTFITS_H
#define ASTROH_SGD_EVENTFITS_H

#include <cstdint>
#include <cstddef>
#include <memory>
#include <array>
#include <string>
#include "SGDEvent.hh"

namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

namespace astroh {

namespace sgd {

class EventFITSIOHelper
{
private:
  template<typename T1, typename T2, size_t N>
  void copyVectorToArray(const std::vector<T1>& vec, std::array<T2, N>& array) const
  {
    const size_t size = vec.size();
    for (size_t i=0; i<size; i++) {
      array[i] = vec[i];
    }
  }

public:
  EventFITSIOHelper();
  ~EventFITSIOHelper();

  void setSGDIDs(int unit, int cc)
  {
    unitID_ = unit;
    ccID_ = cc;
  }

  bool createFITSFile(const std::string& filename);
  void initializeFITSTable(long int numberOfRows=0l);
  void fillEvent(const sgd::Event& event);
  
  bool openFITSFile(const std::string& filename);
  long int NumberOfRows();
  void restoreEvent(long int row, sgd::Event& event);
  std::shared_ptr<sgd::Event> getEvent(long int row);

  void closeFITSFile();

private:
  void initializeFITSHeader();

private:
  int unitID_ = 0;
  int ccID_ = 0;
  cfitsio::fitsfile* fitsFile_ = nullptr;
  long int rowIndex_ = 1; // starts from 1.

  // FFF contents
  std::array<double, 1> TIME_;
  std::array<double, 1> S_TIME_;
  std::array<uint8_t, 1> ADU_CNT_;
  std::array<uint32_t, 1> L32TI_;
  std::array<int32_t, 1> OCCURRENCE_ID_;
  std::array<uint32_t, 1> LOCAL_TIME_;
  std::array<uint8_t, 1> CATEGORY_;
  std::array<uint8_t, 64> FLAGS_;
  std::array<uint8_t, 1> FLAG_LCHKMIO_;
  std::array<uint8_t, 3> FLAG_CCBUSY_;
  std::array<uint8_t, 3> FLAG_HITPAT_CC_;
  std::array<uint8_t, 4> FLAG_HITPAT_;
  std::array<uint8_t, 4> FLAG_FASTBGO_;
  std::array<uint8_t, 1> FLAG_SEU_;
  std::array<uint8_t, 1> FLAG_LCHK_;
  std::array<uint8_t, 1> FLAG_CALMODE_;
  std::array<uint8_t, 31> FLAG_TRIGPAT_;
  std::array<uint8_t, 1> FLAG_TRIG_;
  std::array<uint32_t, 1> LIVETIME_;
  std::array<uint8_t, 1> NUM_ASIC_;
  std::array<uint8_t, MaxSizeOfRawASICDATA> RAW_ASIC_DATA_;
  std::array<uint32_t, 1> PROC_STATUS_;
  std::array<uint8_t, 1> STATUS_;

  // SFF contents (ASICs)
  std::array<int16_t, NumberOfASICs> ASIC_ID_;
  std::array<uint8_t, NumberOfASICs> ASIC_ID_RMAP_;
  std::array<uint8_t, NumberOfASICs> ASIC_CHIP_;
  std::array<uint8_t, NumberOfASICs> ASIC_TRIG_;
  std::array<uint8_t, NumberOfASICs> ASIC_SEU_;
  std::array<uint64_t, NumberOfASICs> READOUT_FLAG_;
  std::array<int16_t, NumberOfASICs> NUM_READOUT_;
  std::array<int16_t, NumberOfASICs> ASIC_REF_;
  std::array<int16_t, NumberOfASICs> ASIC_CMN_;

  // SFF contents (readout channels)
  std::array<int16_t, NumberOfChannelsInCC> READOUT_ASIC_ID_;
  std::array<uint8_t, NumberOfChannelsInCC> READOUT_ID_;
  std::array<int16_t, NumberOfChannelsInCC> READOUT_ID_RMAP_;
  std::array<int16_t, NumberOfChannelsInCC> PHA_;
  std::array<float, NumberOfChannelsInCC> EPI_;
};


class EventFITSWriter
{
public:
  EventFITSWriter();
  ~EventFITSWriter();

  bool open(const std::string& filename);
  void fillEvent(const sgd::Event& event);
  void close();

  void setSGDIDs(int unit, int cc)
  { io_->setSGDIDs(unit, cc); }

private:
  std::unique_ptr<EventFITSIOHelper> io_;
};


class EventFITSReader
{
public:
  EventFITSReader();
  ~EventFITSReader();

  bool open(const std::string& filename);
  long int NumberOfRows();
  void restoreEvent(long int row, sgd::Event& event);
  std::shared_ptr<sgd::Event> getEvent(long int row);
  void close();

private:
  std::unique_ptr<EventFITSIOHelper> io_;
};

} // namespace sgd

} // namespace astroh

#endif /* ASTROH_SGD_EVENTFITS_H */
