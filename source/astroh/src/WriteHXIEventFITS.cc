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

#include "WriteHXIEventFITS.hh"
#include "InitialInformation.hh"
#include "AstroUnits.hh"
#include "ChannelID.hh"
#include "HXIEvent.hh"
#include "HXIEventFITS.hh"
#include "DetectorReadoutModule.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace
{

void fillHits(int64_t occurrenceID,
              const std::vector<comptonsoft::DetectorHit_sptr>& hits,
              astroh::hxi::Event* event)
{
  event->setOccurrenceID(occurrenceID);
  for (auto& hit: hits) {
    const int readoutModuleID = hit->ReadoutModuleID();
    const int section = hit->ReadoutChannelSection();
    const int channelID = hit->ReadoutChannelIndex();
    const int16_t ASIC_ID = astroh::hxi::Event::makeASICID(readoutModuleID, section);
    const int16_t channelID_remapped = astroh::hxi::Event::makeRemappedReadoutID(ASIC_ID, channelID);
    event->pushReadoutData(ASIC_ID,
                           channelID,
                           channelID_remapped,
                           hit->PHA(),
                           hit->EPI()/keV);
  }
}

} /* anonymous namespace */


namespace comptonsoft
{

WriteHXIEventFITS::WriteHXIEventFITS()
  : m_Filename("event.fits"),
    m_HitCollection(nullptr),
    m_InitialInfo(nullptr),
    m_EventWriter(new astroh::hxi::EventFITSWriter)
{
}

WriteHXIEventFITS::~WriteHXIEventFITS() = default;

ANLStatus WriteHXIEventFITS::mod_startup()
{
  register_parameter(&m_Filename, "filename");

  return AS_OK;
}

ANLStatus WriteHXIEventFITS::mod_init()
{
  VCSModule::mod_init();

  GetANLModule("CSHitCollection", &m_HitCollection);
  if (ModuleExist("InitialInformation")) {
    GetANLModuleIF("InitialInformation", &m_InitialInfo);
  }

  EvsDef("WriteHXIEventFITS:Fill");

  if (!(m_EventWriter->open(m_Filename))) {
    return AS_QUIT_ERR;
  }

  return AS_OK;
}

ANLStatus WriteHXIEventFITS::mod_ana()
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
      astroh::hxi::Event event;
      int32_t occurrenceID = eventID + 1;
      if (timeGroup>0) {
        occurrenceID = -1*timeGroup;
      }
      
      fillHits(occurrenceID, hits, &event);
      m_EventWriter->fillEvent(event);
      EvsSet("WriteHXIEventFITS:Fill");
    }
  }

  return AS_OK;
}

ANLStatus WriteHXIEventFITS::mod_exit()
{
  m_EventWriter->close();
  return AS_OK;
}

} /* namespace comptonsoft */
