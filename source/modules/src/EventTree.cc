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

#include "EventTree.hh"
#include "OneASICData.hh"
#include "ReadDataFile.hh"

using namespace comptonsoft;
using namespace anl;

EventTree::EventTree()
  : m_ReadDataModule(0), m_EventTree(0), m_Time(0), m_EventId(0)
{
}


ANLStatus EventTree::mod_init()
{
  VCSModule::mod_init();

  GetANLModule("ReadDataFile", &m_ReadDataModule);
  return AS_OK;
}


ANLStatus EventTree::mod_his()
{
  VCSModule::mod_his();

  TTree::SetMaxTreeSize(4000000000u);

  m_EventTree = new TTree("eventtree", "Data");
  m_EventTree->Branch("eventid", &m_EventId, "eventid/l");
  m_EventTree->Branch("time", &m_Time, "time/I");

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
      
      short int* buf1 = new short int[nCh];
      m_ADCBufferVec.push_back(buf1);
      sprintf(name, "ADC_%02d_%d", (*itDet)->getID(), j);
      sprintf(leafList, "ADC_%02d_%d[%d]/S", (*itDet)->getID(), j, nCh);
      m_EventTree->Branch(name, buf1, leafList);

      float* buf2 = new float[nCh];
      m_PHABufferVec.push_back(buf2);
      sprintf(name, "PHA_%02d_%d", (*itDet)->getID(), j);
      sprintf(leafList, "PHA_%02d_%d[%d]/F", (*itDet)->getID(), j, nCh);
      m_EventTree->Branch(name, buf2, leafList);

      j++;
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}


ANLStatus EventTree::mod_ana()
{
  m_EventId = m_ReadDataModule->EventID();
  m_Time = m_ReadDataModule->Time();

  std::vector<short int*>::iterator itBufADC = m_ADCBufferVec.begin();
  std::vector<float*>::iterator itBufPHA = m_PHABufferVec.begin();

  short int* pADC;
  float* pPHA;

  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      pADC = *itBufADC;
      pPHA = *itBufPHA;
      for (int k=0; k<nCh; k++) {
	int rawadc = (*itChip)->getRawADC(k);
	pADC[k] = static_cast<short int>(rawadc);

	double pha = (*itChip)->getPHA(k);
	pPHA[k] = static_cast<float>(pha);
      }
      itChip++;
      itBufADC++;
      itBufPHA++;
    }
    itDet++;
  }

  m_EventTree->Fill();

  return AS_OK;
}


ANLStatus EventTree::mod_exit()
{
  std::vector<short int*>::iterator itBufADC = m_ADCBufferVec.begin();
  std::vector<short int*>::iterator itBufADCEnd = m_ADCBufferVec.end();

  while (itBufADC != itBufADCEnd) {
    delete[] (*itBufADC);
    itBufADC++;
  }
  m_ADCBufferVec.clear();
  
  std::vector<float*>::iterator itBufPHA = m_PHABufferVec.begin();
  std::vector<float*>::iterator itBufPHAEnd = m_PHABufferVec.end();

  while (itBufPHA != itBufPHAEnd) {
    delete[] (*itBufPHA);
    itBufPHA++;
  }
  m_PHABufferVec.clear();

  return AS_OK;
}

