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

#include "ReadSGDEventFITS.hh"
#include "TChain.h"
#include "ChannelID.hh"
#include "SGDEvent.hh"
#include "SGDEventFITS.hh"
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

ReadSGDEventFITS::ReadSGDEventFITS()
  : anlgeant4::InitialInformation(false),
    m_NumEvents(0), m_Index(0)
{
  add_alias("InitialInformation");
}

ReadSGDEventFITS::~ReadSGDEventFITS() = default;

ANLStatus ReadSGDEventFITS::mod_startup()
{
  register_parameter(&m_Filename, "filename");

  return AS_OK;
}

ANLStatus ReadSGDEventFITS::mod_init()
{
  VCSModule::mod_init();

  m_EventReader.reset(new astroh::sgd::EventFITSReader);
  m_EventReader->open(m_Filename);
  m_NumEvents = m_EventReader->NumberOfRows();
  std::cout << "Total events: " << m_NumEvents << std::endl;
  
  return AS_OK;
}

ANLStatus ReadSGDEventFITS::mod_ana()
{
  if (m_Index==m_NumEvents) {
    return AS_QUIT;
  }

  astroh::sgd::Event event;
  const long int row = m_Index + 1;
  m_EventReader->restoreEvent(row, event);
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
    const float EPI = event.getEPIVector()[i];

    MultiChannelData* ASICData = detectorManager->getMultiChannelData(ReadoutID);
    ASICData->setDataValid(ChannelID, 1);
    ASICData->setRawADC(ChannelID, ADCValue);
    ASICData->setEPI(ChannelID, EPI);
  }

  m_Index++;
  return AS_OK;
}

} /* namespace comptonsoft */
