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

#include "ReadHXIEventFITS.hh"
#include "AstroUnits.hh"
#include "TChain.h"
#include "ChannelID.hh"
#include "HXIEvent.hh"
#include "HXIEventFITS.hh"
#include "ReadoutModule.hh"
#include "MultiChannelData.hh"

using namespace anl;

namespace unit = anlgeant4::unit;

namespace
{

comptonsoft::ReadoutBasedChannelID getReadoutID(uint8_t ASIC_ID)
{
  const uint8_t ASICNo = ASIC_ID >> 4;
  const uint8_t TrayNo = ASIC_ID & 0xf;
  return comptonsoft::ReadoutBasedChannelID(TrayNo, ASICNo);
}

bool is_pseudo_triggered(astroh::hxi::EventFlags f)
{
  return (f.getTriggerPattern() & (0x1u<<6));
}

bool is_pseudo_effective(astroh::hxi::EventFlags f)
{
  if (f.getFastBGO()!=0) { return false; }
  if (f.getHitPattern()!=0) { return false; }
  return is_pseudo_triggered(f);
}

} /* anonymous namespace */


namespace comptonsoft
{

ReadHXIEventFITS::ReadHXIEventFITS()
  : anlgeant4::InitialInformation(false),
    m_VetoEnable(true), m_NumEvents(0), m_Index(0)
{
  add_alias("InitialInformation");
}

ReadHXIEventFITS::~ReadHXIEventFITS() = default;

ANLStatus ReadHXIEventFITS::mod_startup()
{
  register_parameter(&m_Filename, "filename");

  return AS_OK;
}

ANLStatus ReadHXIEventFITS::mod_init()
{
  VCSModule::mod_init();

  m_EventReader.reset(new astroh::hxi::EventFITSReader);
  m_EventReader->open(m_Filename);
  m_NumEvents = m_EventReader->NumberOfRows();
  std::cout << "Total events: " << m_NumEvents << std::endl;

  EvsDef("ReadHXIEventFITS:PseudoTrigger");
  EvsDef("ReadHXIEventFITS:PseudoEffective");
  EvsDef("ReadHXIEventFITS:ShieldTrigger");
  
  return AS_OK;
}

ANLStatus ReadHXIEventFITS::mod_ana()
{
  if (m_Index==m_NumEvents) {
    return AS_QUIT;
  }

  astroh::hxi::Event event;
  const long int row = m_Index + 1;
  m_EventReader->restoreEvent(row, event);
  setEventID(event.getOccurrenceID());

  const double eventTime = event.getTime() * unit::second;
  const astroh::hxi::EventFlags eventFlags = event.getFlags();

  if (is_pseudo_triggered(eventFlags)) {
    EvsSet("ReadHXIEventFITS:PseudoTrigger");
  }
  if (is_pseudo_effective(eventFlags)) {
    EvsSet("ReadHXIEventFITS:PseudoEffective");
  }

  if (eventFlags.getHitPattern() || eventFlags.getFastBGO()) {
    EvsSet("ReadHXIEventFITS:ShieldTrigger");
    if (m_VetoEnable) {
      m_Index++;
      return AS_SKIP;
    }
  }

#if 0
  const uint32_t localTime = event.getLocalTime();
  const uint32_t liveTime = event.getLiveTime();
#endif

  DetectorSystem* detectorManager = getDetectorManager();

  const size_t NumHitASICs = event.LengthOfASICData();
  for (size_t i=0; i<NumHitASICs; i++) {
    const uint8_t ASICID = event.getASICIDVector()[i];
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
    const uint8_t ASICID = event.getReadoutASICIDVector()[i];
    const ReadoutBasedChannelID ReadoutID = getReadoutID(ASICID);
      
    const int ChannelID = event.getReadoutChannelIDVector()[i];
    const uint16_t ADCValue = event.getPHAVector()[i];
    const float EPI = event.getEPIVector()[i] * unit::keV;

    MultiChannelData* ASICData = detectorManager->getMultiChannelData(ReadoutID);
    ASICData->setDataValid(ChannelID, 1);
    ASICData->setRawADC(ChannelID, ADCValue);
    ASICData->setEPI(ChannelID, EPI);
  }

  m_Index++;
  return AS_OK;
}

} /* namespace comptonsoft */
