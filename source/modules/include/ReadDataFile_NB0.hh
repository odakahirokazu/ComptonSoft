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

// ReadDataFile_NB0.hh
// 2007-10-02  Hirokazu Odaka 
// 2007-11-02  Hirokazu Odaka 
// 2008-xx-xx  read dead time by Aono
// 2008-08-31  Hirokazu Odaka 
// 2011-06-06  Hirokazu Odaka 


#ifndef COMPTONSOFT_ReadDataFile_NB0_H
#define COMPTONSOFT_ReadDataFile_NB0_H 1

#include "ReadDataFile.hh"

#include <fstream>
#include <string>

namespace comptonsoft {

class ReadDataFile_NB0 : public ReadDataFile
{
  DEFINE_ANL_MODULE(ReadDataFile_NB0, 3.0);
public:
  ReadDataFile_NB0();
  ~ReadDataFile_NB0() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_bgnrun();
  anl::ANLStatus mod_ana();
  
  unsigned int UnixTime() const { return m_UnixTime; }
  unsigned int TI() const { return m_TI; }
  unsigned int LiveTime() const { return m_LiveTime; }
  unsigned int IntegralLiveTime() const { return m_IntegralLiveTime; }
  unsigned short int TrigHitPat() const { return m_TrigHitPat; }
  unsigned short int ADCClkCnt() const { return m_ADCClkCnt; }

private:
  bool readHK();
  bool readFrame();
  unsigned int* readEvent(unsigned int* p);
  void decodeASICData();

private:
  static const int HK_LENGTH = 2061;
  static const size_t FRAME_LENGTH = 8194;
  static const size_t DATABUF_SIZE = 4096;
  
  std::ifstream m_fin;
  unsigned int m_HKBuf[HK_LENGTH];
  unsigned int m_FrameBuf[FRAME_LENGTH];
  bool m_NewFrame;
  unsigned int* m_PtrFrame;
  unsigned int m_DataBitBuf[DATABUF_SIZE];

  int m_EventLength;

  unsigned int m_UnixTime;
  unsigned int m_TI;
  unsigned int m_LiveTime;
  unsigned int m_IntegralLiveTime;
  unsigned short int m_TrigHitPat;
  unsigned short int m_ADCClkCnt;
};

}

#endif /* COMPTONSOFT_ReadDataFile_NB0_H */
