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

#include "ReadDataFile_NB0.hh"

#include <iostream>
#include <iomanip>

#include "ReadoutModule.hh"
#include "MultiChannelData.hh"

using namespace anl;

namespace comptonsoft
{

ReadDataFile_NB0::ReadDataFile_NB0()
  : m_NewFrame(true),
    m_EventLength(1024)
{
}

ANLStatus ReadDataFile_NB0::mod_define()
{
  register_parameter(&m_EventLength, "event_length");

  return ReadDataFile::mod_define();
}

ANLStatus ReadDataFile_NB0::mod_initialize()
{
  ReadDataFile::mod_initialize();

  // check file open
  bool check = checkFiles();
  if (!check) return AS_QUIT_ERROR;

  define_evs("ReadDataFile:ReadHK");

  return AS_OK;
}

ANLStatus ReadDataFile_NB0::mod_begin_run()
{
  if (wasLastFile()) {
    std::cout << "The file list seems empty." << std::endl;
    return AS_QUIT;
  }
  
  std::string filename = nextFile();
  m_fin.open(filename.c_str());
  if (!m_fin) {
    std::cout << "ReadDataFile: cannot open " << filename << std::endl;
    return AS_QUIT;
  }
  return AS_OK;
}

ANLStatus ReadDataFile_NB0::mod_analyze()
{
  unsigned int tmp;

 read_start:
  if (m_NewFrame) {
    m_NewFrame = false;
    m_fin.read((char*)&tmp, sizeof(int));

    if (m_fin.eof()) {
      std::cout << "ReadDataFile: reach end of file" << std::endl;
      m_fin.close();
      m_NewFrame = true;

      if (wasLastFile()) {
        std::cout << "ReadDataFile: the last file was processed." << std::endl;
        return AS_QUIT;
      }
      else {
        std::string filename = nextFile();
        m_fin.open(filename.c_str());
        if (!m_fin) {
          std::cout << "ReadDataFile: cannot open " << filename << std::endl;
          return AS_QUIT;
        }
        std::cout << "ReadDataFile: open next file." << std::endl;
        goto read_start;
      }
    }

    if ((tmp&0x00FFFFFF) != 0x00efcdab) {
      m_NewFrame = true;
      return AS_SKIP;
    }
    
    if ((tmp&0xFF000000) == 0x03000000) {
      if (readHK()) {
        m_NewFrame = true;
        set_evs("ReadDataFile:ReadHK");
        return AS_OK;
      }
      else {
        m_NewFrame = true;
        return AS_SKIP_ERROR;
      }
    }
    else if ((tmp&0xFF000000) == 0x02000000) {
      if (readFrame()) {
        m_PtrFrame = m_FrameBuf;
      }
      else {
        m_NewFrame = true;
        return AS_SKIP_ERROR;
      }
    }
    else {
      std::cout << "wrong character." << std::endl;
      return AS_QUIT_ERROR;
    }
  }

  m_PtrFrame = readEvent(m_PtrFrame);
  return ReadDataFile::mod_analyze();
}

bool ReadDataFile_NB0::readHK()
{
  m_fin.read((char*)m_HKBuf, HK_LENGTH*sizeof(int));
  if (m_HKBuf[HK_LENGTH-1]==0x2301FFFF) {
    ;
  }
  else {
    std::cout << "error in reading HK data packet" << std::endl;
    return false;
  }

  return true;
}

bool ReadDataFile_NB0::readFrame()
{
  m_fin.read((char*)m_FrameBuf, FRAME_LENGTH*sizeof(int));
  if (m_FrameBuf[FRAME_LENGTH-1]==0x2301FFFF) {
    m_UnixTime = m_FrameBuf[FRAME_LENGTH-2];
  }
  else {
    std::cout << "error in reading frame data packet" << std::endl;
    return false;
  }

  return true;
}

uint32_t* ReadDataFile_NB0::readEvent(uint32_t* pEvent)
{
  const size_t EventLength = m_EventLength;
  uint32_t* p = pEvent;
  
  size_t j = 0;
  while ((*p & 0xFFFF0000)!=0x3c3c0000 && j<FRAME_LENGTH) {
    ++p;
    ++j;
  }
  
  size_t i=0;
  unsigned int* pBit = m_DataBitBuf;
  while ((*p & 0x0000FFFF)!=0x00007777 && i<EventLength && j<FRAME_LENGTH) {
    if (i==0) {
      ;
    }
    else if (i==1) {
      m_TI = *p;
    }
    else if (i==2) {
      m_LiveTime = *p;
    }
    else if (i==3) {
      m_IntegralLiveTime = *p;
    }
    else if (i==4) {
      m_TrigHitPat = *p;
      m_ADCClkCnt = 0;
    }
    else {
      for (int ib=0; ib<32; ++ib) {
        *(pBit++) = (*p >> (31-ib)) & 0x00000001;
      }
    }
    
    ++p;
    ++j;
    ++i;
  }

  decodeASICData();

  if (j>=FRAME_LENGTH) {
    p = m_FrameBuf;
    m_NewFrame = true;
  }
  
  return p;
}

void ReadDataFile_NB0::decodeASICData()
{
  const uint32_t* p = m_DataBitBuf;
  
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& readoutModule: detectorManager->getReadoutModules()) {
    for (auto& section: readoutModule->Sections()) {
      MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      const int nCh = mcd->NumberOfChannels();
      mcd->resetRawADCVector();
      
      std::vector<unsigned int> indexVec;
      p += 5;
      for (int l=0; l<nCh; l++) {
        uint32_t flag = *(p++);
        if (flag) {
          indexVec.push_back(l);
        }
      }
      ++p;
      
      const int ADCRes = 10;
      uint16_t ref = 0;
      for (int b=0; b<ADCRes; b++) {
        ref += *(p++) << b;
      }
      mcd->setReferenceLevel(ref);
      
      const int hitnum = indexVec.size();
      for (int i=0; i<hitnum; i++) {
        uint16_t adc = 0;
        for (int b=0; b<ADCRes; b++) {
          adc += *(p++) << b;
        }
        mcd->setRawADC(indexVec[i], adc);
      }
      
      uint16_t cmn = 0;
      for (int b=0; b<ADCRes; b++) {
        cmn += *(p++) << b;
      }
      mcd->setCommonModeNoise(cmn);
      
      ++p;
    }
  }
}

} /* namespace comptonsoft */
