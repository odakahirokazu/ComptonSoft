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

#include "ReadDataFile_SpW2.hh"
#include <iostream>
#include <iomanip>
#include "ReadoutModule.hh"
#include "MultiChannelData.hh"

using namespace anl;

namespace comptonsoft
{

ReadDataFile_SpW2::ReadDataFile_SpW2()
  : m_ReadPacketSize(0), m_DeltaTime(0)
{
}

ANLStatus ReadDataFile_SpW2::mod_define()
{
  m_ReadPacketSize = HEADER_SIZE;

  define_evs("ReadDataFile_SpW2:Error1");
  define_evs("ReadDataFile_SpW2:Error2");
  define_evs("ReadDataFile_SpW2:Error3");
  define_evs("ReadDataFile_SpW2:Error4");
  define_evs("ReadDataFile_SpW2:Error5");

  return ReadDataFile::mod_define();
}

ANLStatus ReadDataFile_SpW2::mod_initialize()
{
  ReadDataFile::mod_initialize();

  // check file open
  bool check = checkFiles();
  if (!check) return AS_QUIT_ERROR;
  
  int readPacketSize = HEADER_SIZE;
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& readoutModule: detectorManager->getReadoutModules()) {
    readPacketSize += (DATA_HEADER_LENGTH * sizeof(short));
    for (auto& section: readoutModule->Sections()) {
      MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      readPacketSize += ( mcd->NumberOfChannels() * sizeof(short) );
    }
  }

  std::cout << std::endl;
  std::cout << "Data size: " << readPacketSize << std::endl;

  if (readPacketSize > READ_BUF_SIZE) {
    std::cout << "Read buffer is too small." << std::endl;
    return AS_QUIT;
  }

  m_ReadPacketSize = readPacketSize;

  return AS_OK;
}

ANLStatus ReadDataFile_SpW2::mod_begin_run()
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

ANLStatus ReadDataFile_SpW2::mod_analyze()
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
  p += 4;

  int errcode = static_cast<int>(*p);
  if (errcode == 1) {
    set_evs("ReadDataFile_SpW2:Error1");
  }
  else if (errcode == 2) {
    set_evs("ReadDataFile_SpW2:Error2");
  }
  else if (errcode == 3) {
    set_evs("ReadDataFile_SpW2:Error3");
  }
  else if (errcode == 4) {
    set_evs("ReadDataFile_SpW2:Error4");
  }
  else if (errcode == 5) {
    set_evs("ReadDataFile_SpW2:Error5");
  }
  
  if (errcode != 0 && errcode != 2) {
    return AS_SKIP;
  }

  p += 4;
  
  int unixtime = 0;
  unixtime += static_cast<int>(*(p++))<<24;
  unixtime += static_cast<int>(*(p++))<<16;
  unixtime += static_cast<int>(*(p++))<<8;
  unixtime += static_cast<int>(*(p++))<<0;
  setTime(unixtime);

  p = buf + HEADER_SIZE;

  int deltaTime = 0x10000;
  
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& readoutModule: detectorManager->getReadoutModules()) {
    // read Header of one module
    for (int i=0; i<DATA_HEADER_LENGTH; i++) {
      unsigned short int tmp;
      tmp = (static_cast<unsigned short int>(*(p+1))<<8) + *p;

      if (i==3) {
        if (tmp < deltaTime) {
          deltaTime = tmp;
        }
      }
      p+=2;
    }

    // read data body
    for (auto& section: readoutModule->Sections()) {
      MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      int nCh = mcd->NumberOfChannels();
      for (int i=0; i<nCh; i++) {
        unsigned int data;
        data = (static_cast<unsigned short int>(*(p+1))<<8) + *p;
        data = data & 0x0FFFu;
        int rawadc;
        if ((data&0x800u) == 0x800u) {
          // data is negative
          rawadc = static_cast<signed int>(data+0xFFFFF000u);
        }
        else {
          rawadc = data;
        }
        
        mcd->setRawADC(i, rawadc);
        p+=2;
      }
    }
  }

  m_DeltaTime = deltaTime;
   
  return ReadDataFile::mod_analyze();
}

} /* namespace comptonsoft */
