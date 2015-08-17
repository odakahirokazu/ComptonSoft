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

#include "EventReconstruction.hh"

#include <cstddef>
#include <fstream>
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "BasicComptonEvent.hh"
#include "StandardEventReconstructionAlgorithm.hh"
#include "CSHitCollection.hh"
#include "DetectorGroupManager.hh"

using namespace anl;

namespace comptonsoft
{

EventReconstruction::EventReconstruction()
  : m_MaxHits(2),
    m_SourceDistant(true),
    m_SourceDirection(0.0, 0.0, 1.0), m_SourcePosition(0.0, 0.0, 0.0),
    m_HitCollection(nullptr), m_DetectorGroupManager(nullptr),
    m_ComptonEvent(new BasicComptonEvent),
    m_Reconstruction(new StandardEventReconstructionAlgorithm)
{
  add_alias("EventReconstruction");
}

ANLStatus EventReconstruction::mod_startup()
{
  register_parameter(&m_MaxHits, "max_hits");
  register_parameter(&m_SourceDistant, "source_distant");
  register_parameter(&m_SourceDirection, "source_direction");
  register_parameter(&m_SourcePosition, "source_position", cm, "cm");

  return AS_OK;
}

ANLStatus EventReconstruction::mod_init()
{
  EvsDef("EventReconstruction:OK");
  EvsDef("EventReconstruction:NG");
  
  assignSourceInformation();
  GetANLModuleNC("CSHitCollection", &m_HitCollection);
  initializeHitPatternData();
  m_Reconstruction->setMaxHits(m_MaxHits);
  
  return AS_OK;
}

ANLStatus EventReconstruction::mod_ana()
{
  std::vector<DetectorHit_sptr> hits = m_HitCollection->getHits();
  determineHitPattern(hits);
  bool result = m_Reconstruction->reconstruct(hits, *m_ComptonEvent);
  if (result) {
    EvsSet("EventReconstruction:OK");
  }
  else {
    EvsSet("EventReconstruction:NG");
    return AS_SKIP;
  }
  
  return AS_OK;
}

ANLStatus EventReconstruction::mod_endrun()
{
  std::cout << std::endl;
  printHitPatternData();
  return AS_OK;
}

void EventReconstruction::assignSourceInformation()
{
  if (m_SourceDistant) {
    m_ComptonEvent->setSourceDirection(m_SourceDirection);
  }
  else {
    m_ComptonEvent->setSourcePosition(m_SourcePosition);
  }
}

void EventReconstruction::initializeHitPatternData()
{
  GetANLModule("DetectorGroupManager", &m_DetectorGroupManager);

  const std::vector<HitPattern>& hitPatterns
    = m_DetectorGroupManager->getHitPatterns();
  const std::size_t n = hitPatterns.size();
  m_HitPatternFlags.assign(n, 0);
  m_HitPatternCounts.assign(n, 0);

  for (std::size_t i=0; i<n; i++) {
    std::string evsName = "HitPattern:";
    evsName += hitPatterns[i].ShortName();
    EvsDef(evsName);
  }
}

void EventReconstruction::determineHitPattern(const std::vector<DetectorHit_sptr>& hits)
{
  const std::vector<HitPattern>& hitPatterns
    = m_DetectorGroupManager->getHitPatterns();
  const std::size_t n = hitPatterns.size();
  for (std::size_t i=0; i<n; i++) {
    if (hitPatterns[i].match(hits)) {
      m_HitPatternFlags[i] = 1;
      m_HitPatternCounts[i]++;

      std::string evsName = "HitPattern:";
      evsName += hitPatterns[i].ShortName();
      EvsSet(evsName);
    }
    else {
      m_HitPatternFlags[i] = 0;
    }
  }
}

void EventReconstruction::determineHitPattern(const std::vector<int>& detectorIDVector)
{
  const std::vector<HitPattern>& hitPatterns
    = m_DetectorGroupManager->getHitPatterns();
  const std::size_t n = hitPatterns.size();
  for (std::size_t i=0; i<n; i++) {
    if (hitPatterns[i].match(detectorIDVector)) {
      m_HitPatternFlags[i] = 1;
      m_HitPatternCounts[i]++;
    }
    else {
      m_HitPatternFlags[i] = 0;
    }
  }
}

void EventReconstruction::printHitPatternData()
{
  std::cout << "Hit Pattern (EventReconstruction)" << std::endl;
  const std::vector<HitPattern>& hitPatterns
    = m_DetectorGroupManager->getHitPatterns();
  const std::size_t n = hitPatterns.size();
  for (std::size_t i=0; i<n; i++) {
    std::cout << hitPatterns[i].Name() << "   "
              << m_HitPatternCounts[i] << std::endl;
  }
}

void EventReconstruction::resetComptonEvent(BasicComptonEvent* event)
{
  m_ComptonEvent.reset(event);
}

} /* namespace comptonsoft */
