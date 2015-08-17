/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#ifndef COMPTONSOFT_HXIEventTreeIO_H
#define COMPTONSOFT_HXIEventTreeIO_H 1

#include <cstdint>
#include "HXIEvent.hh"

class TTree;

namespace comptonsoft {

/**
 * 
 * @author Hirokazu Odaka
 * @date 2015-02-09
 */
class HXIEventTreeIO
{
public:
  HXIEventTreeIO();
  virtual ~HXIEventTreeIO();

  virtual void setTree(TTree* tree)
  { tree_ = tree; }

  virtual void defineBranches();
  virtual void setBranchAddresses();

  void fillEvent(const HXIEvent& event);
  HXIEvent retrieveEvent(int64_t entry);
  
private:
  TTree* tree_;

  /*
   * tree contents
   */
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
  uint16_t RawASICDataSize_ = 0;
	uint8_t RAW_ASIC_DATA_[HXIEvent::MAX_SIZE_RAW_ASIC_DATA];
	uint32_t PROC_STATUS_ = 0;
  uint8_t STATUS_ = 0;

  // SFF contents (ASICs)
  uint8_t ASIC_ID_[HXIEvent::MAX_NUM_ASIC];
	uint8_t ASIC_ID_RMAP_[HXIEvent::MAX_NUM_ASIC];
	uint8_t ASIC_CHIP_[HXIEvent::MAX_NUM_ASIC];
	uint8_t ASIC_TRIG_[HXIEvent::MAX_NUM_ASIC];
	uint8_t ASIC_SEU_[HXIEvent::MAX_NUM_ASIC];
	uint32_t Readout_FLAG_[HXIEvent::MAX_NUM_ASIC];
	int16_t NUM_Readout_[HXIEvent::MAX_NUM_ASIC];
	int16_t ASIC_REF_[HXIEvent::MAX_NUM_ASIC];
	int16_t ASIC_CMN_[HXIEvent::MAX_NUM_ASIC];

  // SFF contents (readout channels)
  uint16_t NumberOfAllHitChannels_ = 0;
  uint8_t Readout_ASIC_ID_[HXIEvent::MAX_NUM_READOUT];
	uint8_t Readout_ID_[HXIEvent::MAX_NUM_READOUT];
	int16_t Readout_ID_RMAP_[HXIEvent::MAX_NUM_READOUT];
	int16_t PHA_[HXIEvent::MAX_NUM_READOUT];
	float EPI_[HXIEvent::MAX_NUM_READOUT];
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HXIEventTreeIO_H */
