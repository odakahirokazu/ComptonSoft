/*************************************************************************
 *                                                                       *
 * Copyright (c) 2014 Hirokazu Odaka                                     *
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

#include "ReadSGDEventTree.hh"
#include "TChain.h"
#include "ChannelID.hh"
#include "SGDEvent.hh"
#include "SGDEventTree.hh"
#include "DetectorReadoutModule.hh"
#include "MultiChannelData.hh"

using namespace anl;

namespace
{

comptonsoft::ReadoutChannelID getReadoutID(uint16_t ASIC_ID)
{
  const uint16_t ADBNo = ASIC_ID>>8;
  const uint16_t TrayNo = (ASIC_ID & 0xf0)>>4;
  const uint16_t ASICIndex = ASIC_ID & 0xf;
  constexpr size_t NumTrayInADB = 7;
  const size_t TrayIndex = NumTrayInADB*ADBNo + TrayNo;
  return comptonsoft::ReadoutChannelID(TrayIndex, ASICIndex);
}

}

namespace comptonsoft
{

ReadSGDEventTree::ReadSGDEventTree()
  : anlgeant4::InitialInformation(false),
    m_NumEvents(0), m_Index(0)
{
  add_alias("InitialInformation");
}

ReadSGDEventTree::~ReadSGDEventTree() = default;

ANLStatus ReadSGDEventTree::mod_startup()
{
  register_parameter(&m_FileNames, "file_list", "seq", "event.root");
  register_parameter(&m_TreeNames, "tree_list", "seq", "event_tree");

  return AS_OK;
}

ANLStatus ReadSGDEventTree::mod_init()
{
  VCSModule::mod_init();

  if (m_FileNames.size()==0) {
    std::cout << "Event file to be read is not specified." << std::endl;
    return AS_QUIT;
  }
  
  if (m_TreeNames.size()==0) {
    m_TreeNames.resize(m_FileNames.size(), "event_tree");
  }

  TChain* tree = new TChain(m_TreeNames[0].c_str());
  for(size_t i=0; i<m_FileNames.size(); i++) {
    tree->AddFile(m_FileNames[i].c_str(),
                  TChain::kBigNumber,
                  m_TreeNames[i].c_str());
  }
  
  m_NumEvents = tree->GetEntries();
  std::cout << "Total events: " << m_NumEvents << std::endl;

  m_EventReader.reset(new astroh::sgd::EventTreeReader(tree));
  
  return AS_OK;
}

ANLStatus ReadSGDEventTree::mod_ana()
{
  if (m_Index==m_NumEvents) {
    return AS_QUIT;
  }

  astroh::sgd::Event event;
  m_EventReader->restoreEvent(m_Index, event);
  setEventID(event.getOccurrenceID());

  // const double time0 = event.getTime();
  // const uint32_t localTime = event.getLocalTime();
  // const astroh::sgd::EventFlag eventFlag = event.getFlag();
  // const uint32_t liveTime = event.getLiveTime();

  DetectorSystem* detectorManager = getDetectorManager();
  const size_t NumHitASICs = event.LengthOfASICData();
  for (size_t i=0; i<NumHitASICs; i++) {
    const uint16_t ASICID = event.getASICIDVector()[i];
    const ReadoutChannelID ReadoutID = getReadoutID(ASICID);
    const uint16_t CommonModeNoise = event.getCommonModeNoiseVector()[i];
    const uint16_t Reference = event.getReferenceLevelVector()[i];
    MultiChannelData* ASICData = detectorManager->getMultiChannelData(ReadoutID);
    ASICData->setCommonModeNoise(CommonModeNoise);
    ASICData->setReferenceLevel(Reference);
  }

  const size_t NumHits = event.LengthOfReadoutData();
  for (size_t i=0; i<NumHits; i++) {
    const int ASICID = event.getReadoutASICIDVector()[i];
    const ReadoutChannelID ReadoutID = getReadoutID(ASICID);
      
    const int ChannelID = event.getReadoutChannelIDVector()[i];
    const uint16_t ADCValue = event.getPHAVector()[i];
    const uint16_t EPI = event.getEPIVector()[i];

    MultiChannelData* ASICData = detectorManager->getMultiChannelData(ReadoutID);
    ASICData->setDataValid(ChannelID, 1);
    ASICData->setRawADC(ChannelID, ADCValue);
    ASICData->setEPI(ChannelID, EPI);
  }

  m_Index++;
  return AS_OK;
}

} /* namespace comptonsoft */
