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
#include "G4SystemOfUnits.hh"
#include "TChain.h"
#include "ChannelID.hh"
#include "SGDEvent.hh"
#include "SGDEventFITS.hh"
#include "ReadoutModule.hh"
#include "MultiChannelData.hh"

using namespace anl;

namespace
{

comptonsoft::ReadoutBasedChannelID getReadoutID(uint16_t ASIC_ID)
{
  const uint16_t ADBNo = ASIC_ID>>8;
  const uint16_t TrayNo = (ASIC_ID & 0xf0)>>4;
  const uint16_t ASICIndex = ASIC_ID & 0xf;
  constexpr size_t NumTrayInADB = 7;
  const size_t TrayIndex = NumTrayInADB*ADBNo + TrayNo;
  return comptonsoft::ReadoutBasedChannelID(TrayIndex, ASICIndex);
}

bool is_pseudo_triggered(astroh::sgd::EventFlags f)
{
  return (f.getTriggerPattern() & (0x1u<<29));
}

bool is_pseudo_effective(int cc_id,
                         astroh::sgd::EventFlags f)
{
  if (cc_id==1) {
    if (f.getCCHitPattern()!=0x1) { return false; }
  }
  else if (cc_id==2) {
    if (f.getCCHitPattern()!=0x2) { return false; }
  }
  else if (cc_id==3) {
    if (f.getCCHitPattern()!=0x4) { return false; }
  }

  if (f.getFastBGO()!=0) { return false; }
  if (f.getHitPattern()!=0) { return false; }
  return is_pseudo_triggered(f);
}

} /* anonymous namespace */


namespace comptonsoft
{

ReadSGDEventFITS::ReadSGDEventFITS()
  : anlgeant4::InitialInformation(false),
    m_CCID(0),
    m_NumEvents(0),
    m_Index(0)
{
  add_alias("InitialInformation");
}

ReadSGDEventFITS::~ReadSGDEventFITS() = default;

ANLStatus ReadSGDEventFITS::mod_startup()
{
  register_parameter(&m_Filename, "filename");
  register_parameter(&m_CCID, "cc_id");

  return AS_OK;
}

ANLStatus ReadSGDEventFITS::mod_init()
{
  VCSModule::mod_init();

  m_EventReader.reset(new astroh::sgd::EventFITSReader);
  m_EventReader->open(m_Filename);
  m_NumEvents = m_EventReader->NumberOfRows();
  std::cout << "Total events: " << m_NumEvents << std::endl;

  EvsDef("ReadSGDEventFITS:PseudoTrigger");
  EvsDef("ReadSGDEventFITS:PseudoEffective");
  
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

  const double eventTime = event.getTime() * second;
  const astroh::sgd::EventFlags eventFlags = event.getFlags();

  if (is_pseudo_triggered(eventFlags)) {
    EvsSet("ReadSGDEventFITS:PseudoTrigger");
  }
  if (is_pseudo_effective(m_CCID, eventFlags)) {
    EvsSet("ReadSGDEventFITS:PseudoEffective");
  }

#if 0
  const uint32_t localTime = event.getLocalTime();
  const uint32_t liveTime = event.getLiveTime();
#endif

  DetectorSystem* detectorManager = getDetectorManager();

  const size_t NumHitASICs = event.LengthOfASICData();
  for (size_t i=0; i<NumHitASICs; i++) {
    const uint16_t ASICID = event.getASICIDVector()[i];
    const ReadoutBasedChannelID ReadoutID = getReadoutID(ASICID);
    const uint16_t CommonModeNoise = event.getCommonModeNoiseVector()[i];
    const uint16_t Reference = event.getReferenceLevelVector()[i];
    MultiChannelData* ASICData = detectorManager->getMultiChannelData(ReadoutID);
    ASICData->setTime(eventTime);
    ASICData->setFlags(eventFlags.get());
    ASICData->setCommonModeNoise(CommonModeNoise);
    ASICData->setReferenceLevel(Reference);
  }

  const size_t NumHits = event.LengthOfReadoutData();
  for (size_t i=0; i<NumHits; i++) {
    const uint16_t ASICID = event.getReadoutASICIDVector()[i];
    const ReadoutBasedChannelID ReadoutID = getReadoutID(ASICID);
      
    const int ChannelID = event.getReadoutChannelIDVector()[i];
    const uint16_t ADCValue = event.getPHAVector()[i];
    const float EPI = event.getEPIVector()[i] * keV;

    MultiChannelData* ASICData = detectorManager->getMultiChannelData(ReadoutID);
    ASICData->setDataValid(ChannelID, 1);
    ASICData->setRawADC(ChannelID, ADCValue);
    ASICData->setEPI(ChannelID, EPI);
  }

  m_Index++;
  return AS_OK;
}

} /* namespace comptonsoft */
