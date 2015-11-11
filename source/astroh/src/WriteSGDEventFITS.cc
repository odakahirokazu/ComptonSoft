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
#include "ChannelID.hh"
#include "SGDEvent.hh"
#include "SGDEventFITS.hh"
#include "DetectorReadoutModule.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace
{

int16_t getASICID(int readoutModuleID, int section)
{
  constexpr uint16_t NumTrayInADB = 7;
  const uint16_t ADBNo = readoutModuleID/NumTrayInADB;
  const uint16_t TrayNo = readoutModuleID%NumTrayInADB;;
  const uint16_t ASICIndex = section;
  const uint16_t asic_id = (ADBNo<<8) + (TrayNo<<4) + ASICIndex;
  return static_cast<int16_t>(asic_id);
}

int16_t getRemappedASICID(int readoutModuleID, int section)
{
  constexpr uint16_t NumTrayInADB = 7;
  constexpr uint16_t NumASICsInADB = 52;
  constexpr int ASICIndexMap[] = {0, 8, 16, 24, 32, 40, 46};
  const uint16_t ADBNo = readoutModuleID/NumTrayInADB;
  const uint16_t TrayNo = readoutModuleID%NumTrayInADB;;
  const int ASICIndex = ADBNo * NumASICsInADB + ASICIndexMap[TrayNo] + section;
  return static_cast<int16_t>(ASICIndex+1);
}

int16_t getRemappedReadoutID(int remappedASICID, int readoutID)
{
  constexpr int NumChannelsInASIC = 64;
  return (remappedASICID-1)*NumChannelsInASIC + readoutID + 1;
}

void fillHits(int64_t occurrenceID,
              const std::vector<comptonsoft::DetectorHit_sptr>& hits,
              astroh::sgd::Event* event)
{
  event->setOccurrenceID(occurrenceID);
  for (auto& hit: hits) {
    const int readoutModuleID = hit->ReadoutModuleID();
    const int section = hit->ReadoutChannelSection();
    const int channelID = hit->ReadoutChannelIndex();
    const int16_t ASIC_ID = getASICID(readoutModuleID, section);
    const int16_t ASIC_ID_remapped = getRemappedASICID(readoutModuleID, section);
    const int16_t channelID_remapped = getRemappedReadoutID(ASIC_ID_remapped, channelID);
    event->pushReadoutData(ASIC_ID,
                           channelID,
                           channelID_remapped,
                           hit->PHA(),
                           hit->EPI());
  }
}

} /* anonymous namespace */


namespace comptonsoft
{

WriteSGDEventFITS::WriteSGDEventFITS()
  : m_Filename("event.fits"),
    m_HitCollection(nullptr),
    m_EventWriter(new astroh::sgd::EventFITSWriter)
{
}

WriteSGDEventFITS::~WriteSGDEventFITS() = default;

ANLStatus WriteSGDEventFITS::mod_startup()
{
  register_parameter(&m_Filename, "filename");

  return AS_OK;
}

ANLStatus WriteSGDEventFITS::mod_init()
{
  VCSModule::mod_init();

  GetANLModule("CSHitCollection", &m_HitCollection);
  EvsDef("WriteSGDEventFITS:Fill");

  if (!(m_EventWriter->open(m_Filename))) {
    return AS_QUIT_ERR;
  }

  return AS_OK;
}

ANLStatus WriteSGDEventFITS::mod_ana()
{
  const int64_t eventID = m_HitCollection->EventID();

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
