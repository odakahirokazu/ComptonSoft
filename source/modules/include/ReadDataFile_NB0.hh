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

#ifndef COMPTONSOFT_ReadDataFile_NB0_H
#define COMPTONSOFT_ReadDataFile_NB0_H 1

#include "ReadDataFile.hh"
#include <cstdint>

namespace comptonsoft {

/**
 * Read data file: "NabeBinary" format.
 * @author Hirokazu Odaka
 * @date 2011-06-06 | Hirokazu Odaka | based on rawdata2root_vata461 by S. Watanabe
 */
class ReadDataFile_NB0 : public ReadDataFile
{
  DEFINE_ANL_MODULE(ReadDataFile_NB0, 3.2);
public:
  ReadDataFile_NB0();
  ~ReadDataFile_NB0() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_bgnrun();
  anl::ANLStatus mod_ana();
  
  uint32_t UnixTime() const { return m_UnixTime; }
  uint32_t TI() const { return m_TI; }
  uint32_t LiveTime() const { return m_LiveTime; }
  uint32_t IntegralLiveTime() const { return m_IntegralLiveTime; }
  uint16_t TrigHitPat() const { return m_TrigHitPat; }
  uint16_t ADCClkCnt() const { return m_ADCClkCnt; }

private:
  bool readHK();
  bool readFrame();
  uint32_t* readEvent(uint32_t* p);
  void decodeASICData();

private:
  static const int HK_LENGTH = 2061;
  static const size_t FRAME_LENGTH = 8194;
  static const size_t DATABUF_SIZE = 4096;
  
  std::ifstream m_fin;
  uint32_t m_HKBuf[HK_LENGTH];
  uint32_t m_FrameBuf[FRAME_LENGTH];
  bool m_NewFrame;
  uint32_t* m_PtrFrame;
  uint32_t m_DataBitBuf[DATABUF_SIZE];

  int m_EventLength;

  uint32_t m_UnixTime;
  uint32_t m_TI;
  uint32_t m_LiveTime;
  uint32_t m_IntegralLiveTime;
  uint16_t m_TrigHitPat;
  uint16_t m_ADCClkCnt;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadDataFile_NB0_H */
