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

#ifndef COMPTONSOFT_ComptonEvent_H
#define COMPTONSOFT_ComptonEvent_H 1

#include <iostream>
#include <vector>
#include "cs_globals.hh"
#include "DetectorHit_sptr.hh"
#include "TwoHitComptonEvent.hh"

namespace comptonsoft {


/**
 * A class of a Compton event which contains multiple simultaneous hits in a stacked detector system.
 * @author Hirokazu Odaka
 * @date 2011-02-16
 */
class ComptonEvent
{
public:
  ComptonEvent();
  virtual ~ComptonEvent(){}

  /** 
   * clear all hits in the hit vector
   */
  void clearHits() { m_Hits.clear(); }
  
  /** 
   * add (push_back) a hit to the hit vector
   */
  void addHit(DetectorHit_sptr hit) { m_Hits.push_back(hit); }

  void setHits(const std::vector<DetectorHit_sptr> v)
  { m_Hits = v; }

  std::vector<DetectorHit_sptr> getHits() const { return m_Hits; }

  /** 
   * @return the begin iterator of the hit vector
   */
  std::vector<DetectorHit_sptr>::const_iterator HitsBegin() const
  { return m_Hits.begin(); }

  /** 
   * @return the end iterator of the hit vector
   */
  std::vector<DetectorHit_sptr>::const_iterator HitsEnd() const
  { return m_Hits.end(); }

  const DetectorHit_sptr getHit(int index) { return m_Hits[index]; }
  int NumberOfHits() const { return m_Hits.size(); }

  /**
   * perform Compton event reconstruction.
   * @param maxHits maximum number of hits to analyze.
   * @return true if the reconstruction is successful.
   */
  bool reconstruct(int maxHits);
  bool extractEventsByTotalEnergy(double e0, double e1, int maxHits=4);

  bool reconstruct2Hits(const std::vector<comptonsoft::DetectorHit_sptr>& hits);
  bool reconstruct3HitsSiSiCdTe(const std::vector<comptonsoft::DetectorHit_sptr>& hits);
  bool reconstruct3HitsSiCdTeCdTe(const std::vector<comptonsoft::DetectorHit_sptr>& hits);
  bool reconstruct3HitsSiCdTeCdTeByFOM(const std::vector<comptonsoft::DetectorHit_sptr>& hits);

  bool checkTotalEnergy(double e0, double e1) const;
  bool checkDeltaTheta(double dtheta0, double dtheta1) const;
  bool checkDeltaTheta(double dtheta) const
  { return checkDeltaTheta(dtheta, dtheta); }

  const TwoHitComptonEvent& getReconstructedEvent() const
  { return m_ReconstructedEvent; }
  void setReconstructedEvent(const TwoHitComptonEvent& val)
  { m_ReconstructedEvent = val; }

  void setFlag(unsigned int v) { m_ReconstructedEvent.setFlag(v); }
  unsigned int getFlag() const { return m_ReconstructedEvent.getFlag(); }
  void addFlag(unsigned int v) { m_ReconstructedEvent.setFlag(v | getFlag()); }

  double TotalEnergy() const { return m_ReconstructedEvent.TotalEnergy(); }
  double EnergyOfRecoilElectron() const
  { return m_ReconstructedEvent.getH1Energy(); }
  double EnergyOfScatteredPhoton() const
  { return m_ReconstructedEvent.getH2Energy(); }
  double CosThetaE() const { return m_ReconstructedEvent.CosThetaE(); }
  double ThetaE() const { return m_ReconstructedEvent.ThetaE(); }
  vector3_t ConeAxis() const { return m_ReconstructedEvent.ConeAxis(); }
  vector3_t ConeVertex() const { return m_ReconstructedEvent.ConeVertex(); }
  double ThetaG() const { return m_ReconstructedEvent.ThetaG(); }
  double DeltaTheta() const { return m_ReconstructedEvent.DeltaTheta(); } 

  void setSourceDirection(double x, double y, double z)
  { m_ReconstructedEvent.setSourceDirection(x, y, z); }

  void setSourcePosition(double x, double y, double z)
  { m_ReconstructedEvent.setSourcePosition(x, y, z); }

  bool SourceDistant() const
  { return m_ReconstructedEvent.SourceDistant(); }
  vector3_t getSourceDirection() const
  { return m_ReconstructedEvent.getSourceDirection(); }
  vector3_t getSourcePosition() const
  { return m_ReconstructedEvent.getSourcePosition(); }

  int HitPatternID() const { return m_HitPatternID; }
  void setHitPatternID(int v) { m_HitPatternID = v; }

  bool FluorescenceCut() const { return m_FluorescenceCut; }
  void setFluorescenceCut(bool v) { m_FluorescenceCut = v; }
  bool checkFluorescence(DetectorHit_sptr fl, DetectorHit_sptr abs) const;

private:
  std::vector<DetectorHit_sptr> m_Hits;
  TwoHitComptonEvent m_ReconstructedEvent;
  int m_HitPatternID;
  bool m_FluorescenceCut;
};

}

#endif /* COMPTONSOFT_ComptonEvent_H */
