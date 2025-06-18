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
#include "HY2017EventReconstructionAlgorithm.hh"
#include "HY2020EventReconstructionAlgorithm.hh"
#include "MTNNEventReconstructionAlgorithm.hh"
#include "TangoAlgorithm.hh"
#include "OberlackAlgorithm.hh"
#include "CSHitCollection.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

EventReconstruction::EventReconstruction()
  : m_MaxHits(2),
    m_ReconstructionMethodName("standard"),
    m_SourceDistant(true),
    m_SourceDirection(0.0, 0.0, 1.0),
    m_SourcePosition(0.0, 0.0, 0.0),
    m_ParameterFile(""),
    m_HitCollection(nullptr),
    m_BaseEvent(new BasicComptonEvent),
    m_Reconstruction(new StandardEventReconstructionAlgorithm)
{
  add_alias("EventReconstruction");
}

ANLStatus EventReconstruction::mod_define()
{
  define_parameter("max_hits", &mod_class::m_MaxHits);
  define_parameter("reconstruction_method", &mod_class::m_ReconstructionMethodName);
  define_parameter("source_distant", &mod_class::m_SourceDistant);
  define_parameter("source_direction", &mod_class::m_SourceDirection);
  define_parameter("source_position", &mod_class::m_SourcePosition, unit::cm, "cm");
  define_parameter("parameter_file", &mod_class::m_ParameterFile);

  return AS_OK;
}

ANLStatus EventReconstruction::mod_initialize()
{
  VCSModule::mod_initialize();
  
  define_evs("EventReconstruction:OK");
  define_evs("EventReconstruction:NG");
  
  assignSourceInformation();
  get_module_NC("CSHitCollection", &m_HitCollection);
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
  else if (ReconstructionMethodName()=="HY2017") {
    m_Reconstruction.reset(new HY2017EventReconstructionAlgorithm);
  }
  else if (ReconstructionMethodName()=="HY2020") {
    m_Reconstruction.reset(new HY2020EventReconstructionAlgorithm);
  }
  else if (ReconstructionMethodName()=="TANGO") {
    m_Reconstruction.reset(new TangoAlgorithm);
  }
  else if (ReconstructionMethodName()=="Oberlack") {
    m_Reconstruction.reset(new OberlackAlgorithm);
  }
  else if (ReconstructionMethodName()=="MTNN") {
    m_Reconstruction.reset(new MTNNEventReconstructionAlgorithm);
  }
  else {
    std::cout << "Unknown reconstruction method is given: " << ReconstructionMethodName()
              << std::endl;
    return AS_QUIT_ERROR;
  }

  m_Reconstruction->setMaxHits(m_MaxHits);

  if (m_ParameterFile != "") {
    m_Reconstruction->setParameterFile(m_ParameterFile);
    const bool paramLoaded = m_Reconstruction->readParameterFile();
    if (!paramLoaded) {
      return AS_QUIT_ERROR;
    }
  }

  return AS_OK;
}

ANLStatus EventReconstruction::mod_analyze()
{
  initializeEvent();

  const std::vector<DetectorHit_sptr> hits = m_HitCollection->getHits();
  determineHitPatterns(hits);
  const bool result = m_Reconstruction->reconstruct(hits, *m_BaseEvent, m_ReconstructedEvents);

  if (result) {
    set_evs("EventReconstruction:OK");
  }
  else {
    set_evs("EventReconstruction:NG");
    return AS_SKIP;
  }
  
  return AS_OK;
}

ANLStatus EventReconstruction::mod_end_run()
{
  std::cout << std::endl;
  printHitPatternData();
  return AS_OK;
}

void EventReconstruction::assignSourceInformation()
{
  assignSourceInformation(*m_BaseEvent);
}

void EventReconstruction::assignSourceInformation(BasicComptonEvent& event) const
{
  if (m_SourceDistant) {
    event.setSourceDirection(m_SourceDirection);
  }
  else {
    event.setSourcePosition(m_SourcePosition);
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
    define_evs(evsName);
  }
}

void EventReconstruction::initializeEvent()
{
  m_BaseEvent->setHitPattern(0u);
  m_HitPatternFlags.assign(m_HitPatternFlags.size(), 0);
  m_ReconstructedEvents.clear();
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
      set_evs(evsName);

      const unsigned int bit = hitPatterns[i].Bit();
      flags |= (1ul<<bit);
    }
    else {
      m_HitPatternFlags[i] = 0;
    }
  }

  m_BaseEvent->setHitPattern(flags);
}

void EventReconstruction::retrieveHitPatterns()
{
  if (NumberOfReconstructedEvents() == 0) { return; }

  const uint64_t flags = m_ReconstructedEvents[0]->HitPattern();

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
      set_evs(evsName);
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

void EventReconstruction::pushReconstructedEvent(const BasicComptonEvent_sptr& event)
{
  m_ReconstructedEvents.push_back(event);
}

void EventReconstruction::clearAllHitPatternEVS()
{
  const std::vector<HitPattern>& hitPatterns
    = getDetectorManager()->getHitPatterns();
  const std::size_t n = hitPatterns.size();

  for (std::size_t i=0; i<n; i++) {
    std::string evsName = "HitPattern:";
    evsName += hitPatterns[i].ShortName();
    reset_evs(evsName);
  }
}

} /* namespace comptonsoft */
