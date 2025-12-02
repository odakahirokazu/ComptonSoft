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

#ifndef COMPTONSOFT_EventReconstruction_H
#define COMPTONSOFT_EventReconstruction_H 1

#include <anlnext/BasicModule.hh>

#include <vector>
#include "CSTypes.hh"
#include "DetectorHit_sptr.hh"
#include "BasicComptonEvent.hh"
#include "VCSModule.hh"


namespace anlgeant4 {
class InitialInformation;
}

namespace comptonsoft {

class VEventReconstructionAlgorithm;
class CSHitCollection;

/**
 * Event reconstruction.
 * @author Hirokazu Odaka
 * @date 2008-12-15
 * @date 2014-11-25
 * @date 2015-10-10 | derived from VCSModule
 * @date 2020-07-02 | 3.0 | multiple reconstruction event cases
 * @date 2025-11-28 | 3.1 | add min_hits
 * @date 2025-12-02 | 4.0 | use initial information for calculating ARM (optional)
 */
class EventReconstruction : public VCSModule
{
  DEFINE_ANL_MODULE(EventReconstruction, 3.1)
public:
  EventReconstruction();
  ~EventReconstruction() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  std::size_t NumberOfReconstructedEvents() const
  { return m_ReconstructedEvents.size(); }
  
  const std::vector<BasicComptonEvent_sptr>& getReconstructedEvents() const
  { return m_ReconstructedEvents; }
  std::vector<BasicComptonEvent_sptr>& getReconstructedEvents()
  { return m_ReconstructedEvents; }
  
  int HitPatternFlag(int index) const { return m_HitPatternFlags[index]; }
  void clearAllHitPatternEVS();

  bool SourceDistant() const { return m_SourceDistant; }
  vector3_t SourceDirection() const { return m_SourceDirection; }
  vector3_t SourcePosition() const { return m_SourcePosition; }

protected:
  std::string ReconstructionMethodName() const { return m_ReconstructionMethodName; }
  
  void assignSourceInformation();
  void assignSourceInformation(BasicComptonEvent& event) const;
  void initializeHitPatternData();

  void initializeEvent();

  void determineHitPatterns(const std::vector<DetectorHit_sptr>& hitvec);
  void determineHitPatterns(const std::vector<int>& idvec);
  void retrieveHitPatterns();
  void printHitPatternData();
  
  void pushReconstructedEvent(const BasicComptonEvent_sptr& event);
  
private:
  int m_MinHits;
  int m_MaxHits;
  std::string m_ReconstructionMethodName;
  bool m_UseInitialInfo;
  bool m_SourceDistant;
  vector3_t m_SourceDirection;
  vector3_t m_SourcePosition;
  std::string m_ParameterFile;

  const anlgeant4::InitialInformation* m_InitialInfo;
  CSHitCollection* m_HitCollection;

  std::unique_ptr<BasicComptonEvent> m_BaseEvent;
  std::vector<BasicComptonEvent_sptr> m_ReconstructedEvents;
  std::unique_ptr<VEventReconstructionAlgorithm> m_Reconstruction;

  std::vector<int> m_HitPatternFlags;
  std::vector<int> m_HitPatternCounts;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_EventReconstruction_H */
