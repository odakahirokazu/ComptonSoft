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

#include "DetectorReadModule.hh"
#include "OneASICData.hh"


using namespace comptonsoft;


using namespace anl;

ReadDataFile_SpW2::ReadDataFile_SpW2()
  : m_ReadPacketSize(0), m_DeltaTime(0)
{
}


ANLStatus ReadDataFile_SpW2::mod_startup()
{
  m_ReadPacketSize = HEADER_SIZE;

  EvsDef("ReadDataFile_SpW2:Error1");
  EvsDef("ReadDataFile_SpW2:Error2");
  EvsDef("ReadDataFile_SpW2:Error3");
  EvsDef("ReadDataFile_SpW2:Error4");
  EvsDef("ReadDataFile_SpW2:Error5");

  //  BnkDef("ReadDataFile_SpW2:DeltaT", sizeof(unsigned int));

  return ReadDataFile::mod_startup();
}


ANLStatus ReadDataFile_SpW2::mod_init()
{
  ReadDataFile::mod_init();

  // check file open
  std::list<std::string>::iterator itFileName;
  for (itFileName = m_FileNameList.begin(); itFileName != m_FileNameList.end(); itFileName++) {
    std::ifstream fin;
    fin.open( itFileName->c_str() );
    if (!fin) {
      std::cout << "ReadDataFile: cannot open " << *itFileName << std::endl;
      return AS_QUIT;
    }
    fin.close();
  }
  
  int readPacketSize = HEADER_SIZE;
  std::vector<DetectorReadModule*>::iterator itModule = GetDetectorManager()->getReadModuleVector().begin();
  std::vector<DetectorReadModule*>::iterator itModuleEnd = GetDetectorManager()->getReadModuleVector().end();
  while ( itModule != itModuleEnd ) {
    readPacketSize += (DATA_HEADER_LENGTH * sizeof(short));
    std::vector<OneASICData*>::iterator itChip = (*itModule)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itModule)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      readPacketSize += ( (*itChip)->NumChannel() * sizeof(short) );
      itChip++;
    }
    itModule++;
  }

  std::cout << std::endl;
  std::cout << "Data size: " << readPacketSize << std::endl;

  if (readPacketSize > READ_BUF_SIZE) {
    std::cout << "Read buffer is too small ." << std::endl;
    return AS_QUIT;
  }

  m_ReadPacketSize = readPacketSize;

  return AS_OK;
}


ANLStatus ReadDataFile_SpW2::mod_bgnrun()
{
  m_fin.open( m_FileNameList.front().c_str() );
  if (!m_fin) {
    std::cout << "ReadDataFile: cannot open " << m_FileNameList.front() << std::endl;
    return AS_QUIT;
    
  }

  m_FileNameList.pop_front();

  return AS_OK;
}


ANLStatus ReadDataFile_SpW2::mod_ana()
{
  static unsigned char buf[READ_BUF_SIZE];

 read_start:
  m_fin.read((char*)buf, m_ReadPacketSize);

  if (m_fin.eof()) {
    std::cout << "ReadDataFile: reach end of file" << std::endl;
    m_fin.close();
    
    if (!m_FileNameList.empty()) {
      m_fin.open( m_FileNameList.front().c_str() );
      if (!m_fin) {
	std::cout << "ReadDataFile: cannot open " << m_FileNameList.front() << std::endl;
	return AS_QUIT;
      }
      std::cout << "ReadDataFile: open next file " << std::endl;
      m_FileNameList.pop_front();
      goto read_start;
    }
    else {
      std::cout << "ReadDataFile: data file queue is empty" << std::endl;
      return AS_QUIT;
    }
  }

  unsigned char* p = buf;

  // read Header of one event
  p += 4;

  int errcode = static_cast<int>(*p);
  if (errcode == 1) {
    EvsSet("ReadDataFile_SpW2:Error1");
  }
  else if (errcode == 2) {
    EvsSet("ReadDataFile_SpW2:Error2");
  }
  else if (errcode == 3) {
    EvsSet("ReadDataFile_SpW2:Error3");
  }
  else if (errcode == 4) {
    EvsSet("ReadDataFile_SpW2:Error4");
  }
  else if (errcode == 5) {
    EvsSet("ReadDataFile_SpW2:Error5");
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
  
  std::vector<DetectorReadModule*>::iterator itModule = GetDetectorManager()->getReadModuleVector().begin();
  std::vector<DetectorReadModule*>::iterator itModuleEnd = GetDetectorManager()->getReadModuleVector().end();
  while ( itModule != itModuleEnd ) {
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
    std::vector<OneASICData*>::iterator itChip = (*itModule)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itModule)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
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

	(*itChip)->setRawADC(i, rawadc);
	p+=2;
      }
      itChip++;
    }
    itModule++;
  }

  m_DeltaTime = deltaTime;
   
  return ReadDataFile::mod_ana();
}
