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

#include "WriteSGDEventFITS.hh"
#include "InitialInformation.hh"
#include "AstroUnits.hh"
#include "ChannelID.hh"
#include "SGDEvent.hh"
#include "SGDEventFITS.hh"
#include "ReadoutModule.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace unit = anlgeant4::unit;

namespace
{

int16_t getASICID(int readoutModuleID, int section)
{
  constexpr uint16_t NumTrayInADB = 7;
  const uint16_t ADBNo = readoutModuleID/NumTrayInADB;
  const uint16_t TrayNo = readoutModuleID%NumTrayInADB;
  const uint16_t ASICIndex = section;
  return astroh::sgd::Event::makeASICID(ADBNo, TrayNo, ASICIndex);
}

void fillHits(int64_t occurrenceID,
              const std::vector<comptonsoft::DetectorHit_sptr>& hits,
              astroh::sgd::Event* event)
{
  event->setOccurrenceID(occurrenceID);
  astroh::sgd::EventFlags flags = event->getFlags();
  for (auto& hit: hits) {
    const int readoutModuleID = hit->ReadoutModuleID();
    const int section = hit->ReadoutSection();
    const int channelID = hit->ReadoutChannel();
    const int16_t ASIC_ID = getASICID(readoutModuleID, section);
    const int16_t ASIC_ID_remapped = astroh::sgd::Event::convertToRemapedASICID(ASIC_ID);
    const int16_t channelID_remapped = astroh::sgd::Event::makeRemappedReadoutID(ASIC_ID_remapped, channelID);
    event->pushReadoutData(ASIC_ID,
                           channelID,
                           channelID_remapped,
                           hit->PHA(),
                           hit->EPI()/unit::keV);
    if (hit->SelfTriggered()) {
      const int triggerIndex = readoutModuleID;
      flags.addTriggerPatternByIndex(triggerIndex);
    }
  }
  event->setFlags(flags);
}

} /* anonymous namespace */


namespace comptonsoft
{

WriteSGDEventFITS::WriteSGDEventFITS()
  : m_Filename("event.fits"),
    m_SGDID(0),
    m_CCID(0),
    m_HitCollection(nullptr),
    m_InitialInfo(nullptr),
    m_EventWriter(new astroh::sgd::EventFITSWriter)
{
}

WriteSGDEventFITS::~WriteSGDEventFITS() = default;

ANLStatus WriteSGDEventFITS::mod_startup()
{
  register_parameter(&m_Filename, "filename");
  register_parameter(&m_SGDID, "sgd");
  register_parameter(&m_CCID, "cc");

  return AS_OK;
}

ANLStatus WriteSGDEventFITS::mod_init()
{
  VCSModule::mod_init();

  GetModule("CSHitCollection", &m_HitCollection);
  if (ModuleExist("InitialInformation")) {
    GetModuleIF("InitialInformation", &m_InitialInfo);
  }

  EvsDef("WriteSGDEventFITS:Fill");

  m_EventWriter->setSGDIDs(m_SGDID, m_CCID);
  if (!(m_EventWriter->open(m_Filename))) {
    return AS_QUIT_ERR;
  }

  return AS_OK;
}

ANLStatus WriteSGDEventFITS::mod_ana()
{
  int64_t eventID = -1;
  if (m_InitialInfo) {
    eventID = m_InitialInfo->EventID();
  }
  else {
    eventID = get_event_loop_index();
  }

  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    const std::vector<DetectorHit_sptr>& hits
      = m_HitCollection->getHits(timeGroup);
    if (hits.size() > 0) {
      astroh::sgd::Event event;
      int32_t occurrenceID = eventID + 1;
      if (timeGroup>0) {
        occurrenceID = -1*timeGroup;
      }
      
      fillHits(occurrenceID, hits, &event);
      m_EventWriter->fillEvent(event);
      EvsSet("WriteSGDEventFITS:Fill");
    }
  }

  return AS_OK;
}

ANLStatus WriteSGDEventFITS::mod_exit()
{
  m_EventWriter->close();
  return AS_OK;
}

} /* namespace comptonsoft */
