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

#include "ReadoutModule.hh"
#include "MultiChannelData.hh"

using namespace anl;

namespace comptonsoft
{

ANLStatus ReadDataFile_NT::mod_initialize()
{
  ReadDataFile::mod_initialize();

  m_Tree = new TChain("eventtree");
  while (!wasLastFile()) {
    m_Tree->Add(nextFile().c_str());
  }
  
  m_NEvents = m_Tree->GetEntries();
  std::cout << "Total Event : " << m_NEvents << std::endl;
  
  int numASICs = 0;
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& readoutModule: detectorManager->getReadoutModules()) {
    numASICs += readoutModule->NumberOfSections();
  }
  std::cout << "Total readout ASICs : " << numASICs << std::endl;
  m_ADC.reset(new std::unique_ptr<uint16_t[]>[numASICs]);

  int iASIC = 0;
  for (auto& readoutModule: detectorManager->getReadoutModules()) {
    for (auto& section: readoutModule->Sections()) {
      MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      int numChannels = mcd->NumberOfChannels();
      m_ADC[iASIC].reset(new uint16_t[numChannels]);
      std::ostringstream oss;
      oss << "adc" << iASIC;
      std::string branchName = oss.str();
      m_Tree->SetBranchAddress(branchName.c_str(), m_ADC[iASIC].get());
      std::cout << branchName << " " << numChannels << std::endl;
   
      iASIC++;
    }
  }

  m_Tree->SetBranchAddress("unixtime", &m_UnixTime);

  return AS_OK;
}

ANLStatus ReadDataFile_NT::mod_analyze()
{
  static unsigned int iEvent = 0;
  if(iEvent == m_NEvents) {
    return AS_QUIT;
  }
  
  m_Tree->GetEntry(iEvent);

  setTime(m_UnixTime);

  DetectorSystem* detectorManager = getDetectorManager();
  int iASIC = 0;
  for (auto& readoutModule: detectorManager->getReadoutModules()) {
    for (auto& section: readoutModule->Sections()) {
      MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      int numChannels = mcd->NumberOfChannels();
      for (int i=0; i<numChannels; i++) {
        uint16_t data = m_ADC[iASIC][i];
        mcd->setRawADC(i, data);
      }
      iASIC++;
    }
  }
  
  iEvent++;
  return ReadDataFile::mod_analyze();
}

} /* namespace comptonsoft */
