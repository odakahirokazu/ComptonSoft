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

#include "EventSelection.hh"

#include <fstream>
#include <iterator>
#include "FlagDefinition.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"

using namespace anlnext;

namespace comptonsoft
{

EventSelection::EventSelection()
  : m_DiscardTimeGroupZero(false),
    m_DiscardTimeGroupNonZero(false),
    m_OffEnabled(true),
    m_VetoEnabled(true),
    m_TriggerEnabled(false),
    m_HitCollection(nullptr)
{
}

ANLStatus EventSelection::mod_define()
{
  register_parameter(&m_DiscardTimeGroupZero, "discard_time_group_zero");
  register_parameter(&m_DiscardTimeGroupNonZero, "discard_time_group_nonzero");
  register_parameter(&m_OffEnabled, "enable_off");
  register_parameter(&m_VetoEnabled, "enable_veto");
  register_parameter(&m_TriggerEnabled, "enable_trigger");

  return AS_OK;
}

ANLStatus EventSelection::mod_initialize()
{
  VCSModule::mod_initialize();
  get_module_NC("CSHitCollection", &m_HitCollection);
  define_evs("EventSelection:Veto");
  define_evs("EventSelection:Trigger");
  return AS_OK;
}

ANLStatus EventSelection::mod_analyze()
{
  DetectorSystem* detectorManager = getDetectorManager();
  const DetectorGroup& AntiDetectorGroup
    = detectorManager->getDetectorGroup("Anti");
  const DetectorGroup& OffDetectorGroup
    = detectorManager->getDetectorGroup("Off");
  const DetectorGroup& TriggerDetectorGroup
    = detectorManager->getDetectorGroup("Trigger");
  const DetectorGroup& LowZDetectorGroup
    = detectorManager->getDetectorGroup("LowZ");
  const DetectorGroup& HighZDetectorGroup
    = detectorManager->getDetectorGroup("HighZ");

  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);

    if (timeGroup==0) {
      if (m_DiscardTimeGroupZero) {
        hits.clear();
        continue;
      }
    }
    else {
      if (m_DiscardTimeGroupNonZero) {
        hits.clear();
        continue;
      }
    }

    if (m_OffEnabled) {
      std::vector<DetectorHit_sptr>::iterator it = hits.begin();
      while (it != hits.end()) {
        const int detectorID = (*it)->DetectorID();
        if (OffDetectorGroup.isMember(detectorID)) {
          it = hits.erase(it);
        }
        else {
          ++it;
        }
      }
    }
    
    bool veto = false;
    bool trigger = false;
    for (DetectorHit_sptr& hit: hits) {
      hit->clearFlags(flag::AntiCoincidence|flag::Trigger|flag::LowZHit|flag::HighZHit);
      const int detectorID = hit->DetectorID();

      if (AntiDetectorGroup.isMember(detectorID)) {
        veto = true;
        hit->addFlags(flag::AntiCoincidence);
        if (timeGroup==0) {
          set_evs("EventSelection:Veto");
        }
      }

      if (TriggerDetectorGroup.isMember(detectorID)) {
        trigger = true;
        hit->addFlags(flag::Trigger);
        if (timeGroup==0) {
          set_evs("EventSelection:Trigger");
        }
      }

      if (LowZDetectorGroup.isMember(detectorID)) {
        hit->addFlags(flag::LowZHit);
      }

      if (HighZDetectorGroup.isMember(detectorID)) {
        hit->addFlags(flag::HighZHit);
      }

      hit->clearFlags(flag::FluorescenceHit);
      const double energy = hit->Energy();
      bool fluorescence = false;
      for (auto& range: m_FluoresenceRanges) {
        const double e0 = std::get<0>(range);
        const double e1 = std::get<1>(range);
        if (e0<=energy && energy<=e1) {
          fluorescence = true;
          break;
        }
      }
      if (fluorescence) {
        hit->addFlags(flag::FluorescenceHit);
      }
    }

    if (m_VetoEnabled && veto) {
      hits.clear();
    }

    if (m_TriggerEnabled && !trigger) {
      hits.clear();
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
