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

#include "ReadDataFile_NT.hh"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "TChain.h"

#include "DetectorReadModule.hh"
#include "OneASICData.hh"

using namespace comptonsoft;
using namespace anl;


ANLStatus ReadDataFile_NT::mod_startup()
{
  unregister_parameter("Data file name");
  register_parameter(&m_FileNameVector, "Data file name", "seq", "data");
  return VCSModule::mod_startup();
}


ANLStatus ReadDataFile_NT::mod_init()
{
  ReadDataFile::mod_init();

  m_Tree = new TChain("eventtree");
  for(std::vector<std::string>::iterator it=m_FileNameVector.begin();
      it!=m_FileNameVector.end(); ++it) {
    m_Tree->Add(it->c_str());
  }
  
  m_NEvent = m_Tree->GetEntries();
  std::cout << "Total Event : " << m_NEvent << std::endl;
  
  int m_NumASIC = 0;
  std::vector<DetectorReadModule*>::iterator itModule = GetDetectorManager()->getReadModuleVector().begin();
  std::vector<DetectorReadModule*>::iterator itModuleEnd = GetDetectorManager()->getReadModuleVector().end();
  while ( itModule != itModuleEnd ) {
    m_NumASIC += (*itModule)->NumASIC();
    itModule++;
  }

  std::cout << "Total readout ASICs : " << m_NumASIC << std::endl;
  m_ADC = new unsigned short int*[m_NumASIC];
  int iASIC = 0;

  itModule = GetDetectorManager()->getReadModuleVector().begin();
  while ( itModule != itModuleEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itModule)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itModule)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      int numChannel = (*itChip)->NumChannel();
      m_ADC[iASIC] = new unsigned short int[numChannel];
      std::ostringstream oss;
      oss << "adc" << iASIC;
      std::string branchName = oss.str();
      m_Tree->SetBranchAddress(branchName.c_str(), m_ADC[iASIC]);

      std::cout << branchName << " " << numChannel << std::endl;
   
      iASIC++;
      itChip++;
    }
    itModule++;
  }

  m_Tree->SetBranchAddress("unixtime", &m_UnixTime);

  return AS_OK;
}


ANLStatus ReadDataFile_NT::mod_ana()
{
  static unsigned int i = 0;
  if(i == m_NEvent) {
    return AS_QUIT;
  }
  
  m_Tree->GetEntry(i);

  setTime(m_UnixTime);
  
  int iASIC = 0;
  std::vector<DetectorReadModule*>::iterator itModule = GetDetectorManager()->getReadModuleVector().begin();
  std::vector<DetectorReadModule*>::iterator itModuleEnd = GetDetectorManager()->getReadModuleVector().end();
  while ( itModule != itModuleEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itModule)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itModule)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      for (int i=0; i<nCh; i++) {
	unsigned int data = m_ADC[iASIC][i];
	(*itChip)->setRawADC(i, data);
      }
      iASIC++;
      itChip++;
    }
    itModule++;
  }
  
  i++;
  return ReadDataFile::mod_ana();
}


ANLStatus ReadDataFile_NT::mod_exit()
{
  for (int i=0; i<m_NumASIC; i++) {
    delete[] m_ADC[i];
  }
  delete[] m_ADC;

  return AS_OK;
}
