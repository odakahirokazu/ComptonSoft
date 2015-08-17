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

  void createFITSFile(const std::string& filename);
  void initializeFITSTable(long int numberOfRows=0l);
  void fillEvent(const sgd::Event& event);
  
  void openFITSFile(const std::string& filename);
  long int NumberOfRows();
  void restoreEvent(long int row, sgd::Event& event);
  std::shared_ptr<sgd::Event> getEvent(long int row);

  void closeFITSFile();
  
private:
  cfitsio::fitsfile* fitsFile_ = nullptr;
  long int rowIndex_ = 0; // starts from 1.

  // FFF contents
  std::array<double, 1> TIME_;
  std::array<double, 1> S_TIME_;
  std::array<uint8_t, 1> ADU_CNT_;
  std::array<uint32_t, 1> L32TI_;
  std::array<int32_t, 1> OCCURRENCE_ID_;
  std::array<uint32_t, 1> LOCAL_TIME_;
  std::array<uint8_t, 1> Category_;
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
  std::array<int16_t, TotalNumberOfASICs> ASIC_ID_;
  std::array<uint8_t, TotalNumberOfASICs> ASIC_ID_RMAP_;
  std::array<uint8_t, TotalNumberOfASICs> ASIC_CHIP_;
  std::array<uint8_t, TotalNumberOfASICs> ASIC_TRIG_;
  std::array<uint8_t, TotalNumberOfASICs> ASIC_SEU_;
  std::array<uint64_t, TotalNumberOfASICs> Readout_FLAG_;
  std::array<int16_t, TotalNumberOfASICs> NUM_Readout_;
  std::array<int16_t, TotalNumberOfASICs> ASIC_REF_;
  std::array<int16_t, TotalNumberOfASICs> ASIC_CMN_;

  // SFF contents (readout channels)
  std::array<int16_t, TotalNumberOfChannelsInCC> Readout_ASIC_ID_;
  std::array<uint8_t, TotalNumberOfChannelsInCC> Readout_ID_;
  std::array<int16_t, TotalNumberOfChannelsInCC> Readout_ID_RMAP_;
  std::array<int16_t, TotalNumberOfChannelsInCC> PHA_;
  std::array<float, TotalNumberOfChannelsInCC> EPI_;
};


class EventFITSWriter
{
public:
  EventFITSWriter();
  ~EventFITSWriter();

  void open(const std::string& filename);
  void fillEvent(const sgd::Event& event);
  void close();

private:
  std::unique_ptr<EventFITSIOHelper> io_;
};


class EventFITSReader
{
public:
  EventFITSReader();
  ~EventFITSReader();

  void open(const std::string& filename);
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
