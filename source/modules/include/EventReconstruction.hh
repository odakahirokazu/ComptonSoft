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

#include "BasicModule.hh"

#include <vector>
#include "CSTypes.hh"
#include "DetectorHit_sptr.hh"
#include "VCSModule.hh"

namespace comptonsoft {

class BasicComptonEvent;
class VEventReconstructionAlgorithm;
class CSHitCollection;

/**
 * Event reconstruction.
 * @author Hirokazu Odaka
 * @date 2008-12-15
 * @date 2014-11-25
 * @date 2015-10-10 | derived from VCSModule
 */
class EventReconstruction : public VCSModule
{
  DEFINE_ANL_MODULE(EventReconstruction, 2.1)
public:
  EventReconstruction();
  ~EventReconstruction() = default;

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;
  anl::ANLStatus mod_end_run() override;

  const BasicComptonEvent& getComptonEvent() const
  { return *m_ComptonEvent; }
  
  int HitPatternFlag(int index) const { return m_HitPatternFlags[index]; }
  void clearAllHitPatternEVS();

  bool SourceDistant() const { return m_SourceDistant; }
  vector3_t SourceDirection() const { return m_SourceDirection; }
  vector3_t SourcePosition() const { return m_SourcePosition; }

protected:
  std::string ReconstructionMethodName() const { return m_ReconstructionMethodName; }
  
  void assignSourceInformation();

  void initializeHitPatternData();
  void resetHitPatternFlags();
  void determineHitPatterns(const std::vector<DetectorHit_sptr>& hitvec);
  void determineHitPatterns(const std::vector<int>& idvec);
  void retrieveHitPatterns();
  void printHitPatternData();
  
  void resetComptonEvent();
  void resetComptonEvent(BasicComptonEvent* event);
  
private:
  int m_MaxHits;
  std::string m_ReconstructionMethodName;
  bool m_SourceDistant;
  vector3_t m_SourceDirection;
  vector3_t m_SourcePosition;

  CSHitCollection* m_HitCollection;

  std::unique_ptr<BasicComptonEvent> m_ComptonEvent;
  std::unique_ptr<VEventReconstructionAlgorithm> m_Reconstruction;

  std::vector<int> m_HitPatternFlags;
  std::vector<int> m_HitPatternCounts;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_EventReconstruction_H */
