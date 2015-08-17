/** @file SGDEventTree.hh
 * containes SGD event tree classes.
 * 
 * @author Soki Sakurai
 * @author Hirokazu Odaka
 * 
 * @date 2013-09-10 | S. Sakurai | Create.
 * @date 2014-09-04 | H. Odaka | new structure.
 * @date 2015-07-07 | H. Odaka | review.
 *
 */

#ifndef ASTROH_SGD_EVENTTREE_H
#define ASTROH_SGD_EVENTTREE_H

#include <cstdint>
#include <cstddef>
#include <array>
#include "TTree.h"
#include "SGDEvent.hh"

namespace astroh {

namespace sgd {

class EventTreeIOHelper
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
  explicit EventTreeIOHelper(TTree* tree);
  ~EventTreeIOHelper();

  void fillEvent(const sgd::Event& event);
  void restoreEvent(Long64_t entry, sgd::Event& event);
  std::shared_ptr<sgd::Event> getEvent(Long64_t entry);

protected:
  void makeLeaves();
  void setAddresses();

private:
  TTree* _tree = nullptr;

  double TIME_ = 0.0;
  double S_TIME_ = 0.0;
  uint16_t ADU_CNT_ = 0;
  uint32_t L32TI_ = 0;
  uint32_t OCCURRENCE_ID_ = 0;
  uint32_t LOCAL_TIME_ = 0;
  uint64_t FLAGS_;
  uint32_t LIVETIME_ = 0;
  int16_t NUM_ASIC_ = 0;

  uint16_t RawASICDataSize_;
  std::array<uint8_t, MaxSizeOfRawASICDATA> RAW_ASIC_DATA_;
  uint32_t PROC_STATUS_ = 0;

  std::array<uint16_t, TotalNumberOfASICs> ASIC_ID_;
  std::array<uint8_t, TotalNumberOfASICs> ASIC_ID_RMAP_;
  std::array<uint8_t, TotalNumberOfASICs> ASIC_CHIP_;
  std::array<uint8_t, TotalNumberOfASICs> ASIC_TRIG_;
  std::array<uint8_t, TotalNumberOfASICs> ASIC_SEU_;
  std::array<uint64_t, TotalNumberOfASICs> READOUT_FLAG_;
  std::array<uint16_t, TotalNumberOfASICs> NUM_READOUT_;
  std::array<uint16_t, TotalNumberOfASICs> ASIC_REF_;
  std::array<uint16_t, TotalNumberOfASICs> ASIC_CMN_;

  uint16_t NumberOfAllHitChannels_;
  std::array<uint16_t, TotalNumberOfChannelsInCC> READOUT_ASIC_ID_;
  std::array<uint8_t, TotalNumberOfChannelsInCC> READOUT_ID_;
  std::array<uint16_t, TotalNumberOfChannelsInCC> READOUT_ID_RMAP_;
  std::array<uint16_t, TotalNumberOfChannelsInCC> PHA_;
  std::array<uint16_t, TotalNumberOfChannelsInCC> EPI_;
};


class EventTreeWriter : public EventTreeIOHelper
{
public:
  explicit EventTreeWriter(TTree* tree)
    : EventTreeIOHelper(tree)
  {
    makeLeaves();
  }
};


class EventTreeReader : public EventTreeIOHelper
{
public:
  explicit EventTreeReader(TTree* tree)
    : EventTreeIOHelper(tree)
  {
    setAddresses();
  }
};

} // namespace sgd

} // namespace astroh

#endif /* ASTROH_SGD_EVENTTREE_H */
