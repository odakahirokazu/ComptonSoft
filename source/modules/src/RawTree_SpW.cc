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

#include "RawTree_SpW.hh"
#include "OneASICData.hh"
#include "ReadDataFile_SpW2.hh"

using namespace comptonsoft;
using namespace anl;


RawTree_SpW::RawTree_SpW()
  : m_ReadDataModule(0),
    m_RawTree_SpW(0), m_Time(0), m_EventId(0), m_DeltaT(0)
{
}


ANLStatus RawTree_SpW::mod_init()
{
  GetANLModule("ReadDataFile_SpW2", &m_ReadDataModule);
  return AS_OK;
}


ANLStatus RawTree_SpW::mod_his()
{
  VCSModule::mod_his();

  TTree::SetMaxTreeSize(4000000000u);
  
  m_RawTree_SpW = new TTree("rawtree", "Raw Data");
  m_RawTree_SpW->Branch("eventid", &m_EventId, "eventid/l");
  m_RawTree_SpW->Branch("time", &m_Time, "time/I");
  m_RawTree_SpW->Branch("delta_t", &m_DeltaT, "delta_t/i");

  char name[256];
  char leafList[256];

  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    int j=0;
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      short int* buf = new short int[nCh];
      m_BufferVec.push_back(buf);
      sprintf(name, "ADC_%02d_%d", (*itDet)->getID(), j);
      sprintf(leafList, "ADC_%02d_%d[%d]/S", (*itDet)->getID(), j, nCh);
      m_RawTree_SpW->Branch(name, buf, leafList);
      j++;
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}


ANLStatus RawTree_SpW::mod_ana()
{
  m_EventId = m_ReadDataModule->EventID();
  m_Time = m_ReadDataModule->Time();
  m_DeltaT = m_ReadDataModule->DeltaTime();

  std::vector<short int*>::iterator itBuf = m_BufferVec.begin();
  short int* p;
  
  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      p = *itBuf;
      for (int k=0; k<nCh; k++) {
	int rawadc = (*itChip)->getRawADC(k);
	p[k] = static_cast<short int>(rawadc);
      }
      itChip++;
      itBuf++;
    }
    itDet++;
  }

  m_RawTree_SpW->Fill();

  return AS_OK;
}


ANLStatus RawTree_SpW::mod_exit()
{
  std::vector<short int*>::iterator itBuf = m_BufferVec.begin();
  std::vector<short int*>::iterator itBufEnd = m_BufferVec.end();

  while (itBuf != itBufEnd) {
    delete[] (*itBuf);
    itBuf++;
  }
  m_BufferVec.clear();

  return AS_OK;
}
