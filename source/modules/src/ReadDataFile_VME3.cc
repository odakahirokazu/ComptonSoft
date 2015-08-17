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

#include "ReadDataFile_VME3.hh"

#include <iostream>
#include <iomanip>

#include "DetectorReadoutModule.hh"
#include "MultiChannelData.hh"

using namespace anl;

namespace comptonsoft
{

ReadDataFile_VME3::ReadDataFile_VME3()
  : m_ReadPacketSize(0), m_DeadTime(0)
{
}

ANLStatus ReadDataFile_VME3::mod_startup()
{
  m_ReadPacketSize = HEADER_SIZE;

  return ReadDataFile::mod_startup();
}

ANLStatus ReadDataFile_VME3::mod_init()
{
  ReadDataFile::mod_init();

  // check file open
  bool check = checkFiles();
  if (!check) return AS_QUIT_ERR;
  
  int readPacketSize = HEADER_SIZE;
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& readoutModule: detectorManager->getReadoutModules()) {
    readPacketSize += (DATA_HEADER_LENGTH * sizeof(short));
    for (auto& section: readoutModule->getReadoutSections()) {
      MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      readPacketSize += ( mcd->NumberOfChannels() * sizeof(short) );
    }
    readPacketSize += (DATA_FOOTER_LENGTH * sizeof(short));
  }
  readPacketSize += FOOTER_SIZE;

  std::cout << std::endl;
  std::cout << "Data size: " << readPacketSize << std::endl;

  if (readPacketSize > READ_BUF_SIZE) {
    std::cout << "Read buffer is too small." << std::endl;
    return AS_QUIT;
  }

  m_ReadPacketSize = readPacketSize;

  return AS_OK;
}

ANLStatus ReadDataFile_VME3::mod_bgnrun()
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

ANLStatus ReadDataFile_VME3::mod_ana()
{
  static unsigned char buf[READ_BUF_SIZE];
  
 read_start:
  m_fin.read((char*)buf, m_ReadPacketSize);

  if (m_fin.eof()) {
    std::cout << "ReadDataFile: reach end of file." << std::endl;
    m_fin.close();

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

  unsigned char* p = buf;

  // read Header of one event
  //p += 4;
  p += 2;
	
  unsigned short deadtime = 0;
  deadtime += static_cast<unsigned short>(*(p++))<<0;
  deadtime += static_cast<unsigned short>(*(p++))<<8;
  m_DeadTime = deadtime;
	
  int unixtime = 0;
  unixtime += static_cast<int>(*(p++))<<0;
  unixtime += static_cast<int>(*(p++))<<8;
  unixtime += static_cast<int>(*(p++))<<16;
  unixtime += static_cast<int>(*(p++))<<24;
  setTime(unixtime);

  ULong64_t eventid = 0;
  eventid += static_cast<ULong64_t>(*(p++))<<0;
  eventid += static_cast<ULong64_t>(*(p++))<<8;
  eventid += static_cast<ULong64_t>(*(p++))<<16;
  eventid += static_cast<ULong64_t>(*(p++))<<24;
  eventid += static_cast<ULong64_t>(*(p++))<<32;
  eventid += static_cast<ULong64_t>(*(p++))<<40;
  eventid += static_cast<ULong64_t>(*(p++))<<48;
  eventid += static_cast<ULong64_t>(*(p++))<<56;

  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& readoutModule: detectorManager->getReadoutModules()) {
    // read Header of one module
    for (int i=0; i<DATA_HEADER_LENGTH; i++) {
      unsigned short int tmp;
      tmp = (static_cast<unsigned short int>(*(p+1))<<8) + *p;
      p+=2;
    }

    for (auto& section: readoutModule->getReadoutSections()) {
      MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      int nCh = mcd->NumberOfChannels();
      for (int i=0; i<nCh; i++) {
        unsigned short int adc_value;
        adc_value = (static_cast<unsigned short int>(*(p+1))<<8) + *p;
        adc_value = adc_value & 0x0FFFu;
        int rawadc = static_cast<int>(adc_value);
        mcd->setRawADC(i, rawadc);
        p+=2;
      }
    }

    // read Footer of one module
    for (int i=0; i<DATA_FOOTER_LENGTH; i++) {
      unsigned short int tmp;
      tmp = (static_cast<unsigned short int>(*(p+1))<<8) + *p;
      p+=2;
    }
  }

  // read Footer of one event
  for (int i=0; i<FOOTER_SIZE; i++) {
    p++;
  }

  return ReadDataFile::mod_ana();
}

} /* namespace comptonsoft */
