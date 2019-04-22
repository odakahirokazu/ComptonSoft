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

#include "DefineFrame.hh"

#include <cmath>
#include "InitialInformation.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

DefineFrame::DefineFrame()
{
}

DefineFrame::~DefineFrame() = default;

ANLStatus DefineFrame::mod_define()
{
  define_parameter("time_start", &mod_class::m_Time0, unit::s, "s");
  define_parameter("frame_exposure", &mod_class::m_FrameExposure, unit::s, "s");

  return AS_OK;
}

ANLStatus DefineFrame::mod_initialize()
{
  VCSModule::mod_initialize();
  get_module_IFNC("InitialInformation", &m_InitialInfo);
  get_module_NC("CSHitCollection", &m_HitCollection);

  return AS_OK;
}

ANLStatus DefineFrame::mod_begin_run()
{
  m_EventIndexInThisFrame = 0;
  m_CurrentEventID = m_InitialInfo->EventID();

  return AS_OK;
}

ANLStatus DefineFrame::mod_analyze()
{
  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  if (NumTimeGroups!=1) {
    std::cout << "Number of time group is not 1." << std::endl;
    return AS_QUIT_ERROR;
  }

  const int64_t eventID = m_InitialInfo->EventID();

  std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(0);
  if (hits.size()==0) {
    return AS_OK;
  }
  const DetectorHit_sptr& hit0 = hits[0];
  const double realTime = hit0->RealTime();
  const int frameID = std::floor((realTime-m_Time0)/m_FrameExposure);

  if (frameID == m_CurrentFrameID) {
    if (eventID != m_CurrentEventID) {
      m_EventIndexInThisFrame++;
      m_CurrentEventID = eventID;
    }
  }
  else {
    m_EventIndexInThisFrame = 0;
    m_CurrentEventID = eventID;
  }
  
  m_InitialInfo->setEventID(frameID);
  for (DetectorHit_sptr hit: hits) {
    hit->setEventID(frameID);
    hit->setTimeGroup(m_EventIndexInThisFrame);
  }

  m_CurrentFrameID = frameID;

  return AS_OK;
}

} /* namespace comptonsoft */
