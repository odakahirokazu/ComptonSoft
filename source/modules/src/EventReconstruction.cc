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
#include <algorithm>
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "BasicComptonEvent.hh"
#include "StandardEventReconstructionAlgorithm.hh"
#include "PhotoAbsorptionEventReconstructionAlgorithm.hh"
#include "FocalPlaneEventReconstructionAlgorithm.hh"
#include "SGDEventReconstructionAlgorithm.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

EventReconstruction::EventReconstruction()
  : m_MaxHits(2),
    m_ReconstructionMethodName("standard"),
    m_SourceDistant(true),
    m_SourceDirection(0.0, 0.0, 1.0), m_SourcePosition(0.0, 0.0, 0.0),
    m_HitCollection(nullptr),
    m_ComptonEvent(new BasicComptonEvent),
    m_Reconstruction(new StandardEventReconstructionAlgorithm)
{
  add_alias("EventReconstruction");
}

ANLStatus EventReconstruction::mod_startup()
{
  register_parameter(&m_MaxHits, "max_hits");
  register_parameter(&m_ReconstructionMethodName, "reconstruction_method");
  register_parameter(&m_SourceDistant, "source_distant");
  register_parameter(&m_SourceDirection, "source_direction");
  register_parameter(&m_SourcePosition, "source_position", unit::cm, "cm");

  return AS_OK;
}

ANLStatus EventReconstruction::mod_init()
{
  VCSModule::mod_init();
  
  EvsDef("EventReconstruction:OK");
  EvsDef("EventReconstruction:NG");
  
  assignSourceInformation();
  GetModuleNC("CSHitCollection", &m_HitCollection);
  initializeHitPatternData();

  if (ReconstructionMethodName()=="standard") {
    m_Reconstruction.reset(new StandardEventReconstructionAlgorithm);
  }
  else if (ReconstructionMethodName()=="photoabsorption") {
    m_Reconstruction.reset(new PhotoAbsorptionEventReconstructionAlgorithm);
  }
  else if (ReconstructionMethodName()=="focal plane") {
    m_Reconstruction.reset(new FocalPlaneEventReconstructionAlgorithm);
  }
  else if (ReconstructionMethodName()=="SGD") {
    m_Reconstruction.reset(new SGDEventReconstructionAlgorithm);
  }
  else {
    std::cout << "Unknown reconstruction method is given: " << ReconstructionMethodName()
              << std::endl;
    return AS_QUIT_ERR;
  }

  m_Reconstruction->setMaxHits(m_MaxHits);
  
  return AS_OK;
}

ANLStatus EventReconstruction::mod_ana()
{
  resetComptonEvent();
  resetHitPatternFlags();

  const std::vector<DetectorHit_sptr> hits = m_HitCollection->getHits();
  determineHitPatterns(hits);
  const bool result = m_Reconstruction->reconstruct(hits, *m_ComptonEvent);
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
  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t n = hitPatterns.size();
  m_HitPatternFlags.assign(n, 0);
  m_HitPatternCounts.assign(n, 0);

  for (std::size_t i=0; i<n; i++) {
    std::string evsName = "HitPattern:";
    evsName += hitPatterns[i].ShortName();
    EvsDef(evsName);
  }
}

void EventReconstruction::resetHitPatternFlags()
{
  const std::size_t n = m_HitPatternFlags.size();
  m_HitPatternFlags.assign(n, 0);
}

void EventReconstruction::determineHitPatterns(const std::vector<DetectorHit_sptr>& hits)
{
  std::vector<int> ids(hits.size());
  std::transform(std::begin(hits), std::end(hits), std::begin(ids),
                 [](DetectorHit_sptr hit){
                   return hit->DetectorID();
                 });
  determineHitPatterns(ids);
}

void EventReconstruction::determineHitPatterns(const std::vector<int>& detectorIDVector)
{
  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t n = hitPatterns.size();
  uint64_t flags(0ul);

  for (std::size_t i=0; i<n; i++) {
    if (hitPatterns[i].match(detectorIDVector)) {
      m_HitPatternFlags[i] = 1;
      m_HitPatternCounts[i]++;

      std::string evsName = "HitPattern:";
      evsName += hitPatterns[i].ShortName();
      EvsSet(evsName);

      const unsigned int bit = hitPatterns[i].Bit();
      flags |= (1ul<<bit);
    }
    else {
      m_HitPatternFlags[i] = 0;
    }
  }

  m_ComptonEvent->setHitPattern(flags);
}

void EventReconstruction::retrieveHitPatterns()
{
  const uint64_t flags = m_ComptonEvent->HitPattern();

  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t n = hitPatterns.size();

  for (std::size_t i=0; i<n; i++) {
    const unsigned int bit = hitPatterns[i].Bit();
    const uint64_t test = 1ul<<bit;
    if (flags & test) {
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

void EventReconstruction::printHitPatternData()
{
  std::cout << "Hit Pattern (EventReconstruction)" << std::endl;
  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t n = hitPatterns.size();
  for (std::size_t i=0; i<n; i++) {
    std::cout << hitPatterns[i].Name() << "   "
              << m_HitPatternCounts[i] << std::endl;
  }
}

void EventReconstruction::resetComptonEvent()
{
  m_ComptonEvent.reset(new BasicComptonEvent);
}

void EventReconstruction::resetComptonEvent(BasicComptonEvent* event)
{
  m_ComptonEvent.reset(event);
}

void EventReconstruction::clearAllHitPatternEVS()
{
  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t n = hitPatterns.size();

  for (std::size_t i=0; i<n; i++) {
    std::string evsName = "HitPattern:";
    evsName += hitPatterns[i].ShortName();
    EvsReset(evsName);
  }
}

} /* namespace comptonsoft */
