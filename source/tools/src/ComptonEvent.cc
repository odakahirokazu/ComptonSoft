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

// 2011-02-14 Hirokazu Odaka
// 2011-02-16 Hirokazu Odaka

#include "ComptonEvent.hh"
#include "DetectorHit.hh"
#include "HXISGDFlagDef.hh"

namespace comptonsoft {

ComptonEvent::ComptonEvent()
  : m_HitPatternID(0), m_FluorescenceCut(false)
{
}


bool ComptonEvent::reconstruct(int maxHits)
{
  m_HitPatternID = 0;
  m_FluorescenceCut = false;

  if (NumberOfHits()>maxHits) {
    return false;
  }

  if (NumberOfHits()==2) {
    return reconstruct2Hits(m_Hits);
  }
  else if (NumberOfHits()==3) {
    bool hit0Si = (m_Hits[0]->getFlag() & comptonsoft::SI_HIT);
    bool hit1Si = (m_Hits[1]->getFlag() & comptonsoft::SI_HIT);
    bool hit2Si = (m_Hits[2]->getFlag() & comptonsoft::SI_HIT);
    bool hit0CdTe = (m_Hits[0]->getFlag() & comptonsoft::CDTE_HIT);
    bool hit1CdTe = (m_Hits[1]->getFlag() & comptonsoft::CDTE_HIT);
    bool hit2CdTe = (m_Hits[2]->getFlag() & comptonsoft::CDTE_HIT);

    std::vector<DetectorHit_sptr> hits(3);

    if (hit0Si && hit1Si && hit2CdTe) {
      hits[0] = m_Hits[0];
      hits[1] = m_Hits[1];
      hits[2] = m_Hits[2];
      return reconstruct3HitsSiSiCdTe(hits);
    }
    else if (hit0Si && hit1CdTe && hit2Si) {
      hits[0] = m_Hits[0];
      hits[1] = m_Hits[2];
      hits[2] = m_Hits[1];
      return reconstruct3HitsSiSiCdTe(hits);
    }
    else if (hit0CdTe && hit1Si && hit2Si) {
      hits[0] = m_Hits[1];
      hits[1] = m_Hits[2];
      hits[2] = m_Hits[0];
      return reconstruct3HitsSiSiCdTe(hits);
    }
    else if (hit0Si && hit1CdTe && hit2CdTe) {
      hits[0] = m_Hits[0];
      hits[1] = m_Hits[1];
      hits[2] = m_Hits[2];
      // return reconstruct3HitsSiCdTeCdTe(hits);
      return reconstruct3HitsSiCdTeCdTeByFOM(hits);
    }
    else if (hit0CdTe && hit1Si && hit2CdTe) {
      hits[0] = m_Hits[1];
      hits[1] = m_Hits[0];
      hits[2] = m_Hits[2];
      // return reconstruct3HitsSiCdTeCdTe(hits);
      return reconstruct3HitsSiCdTeCdTeByFOM(hits);
    }
    else if (hit0CdTe && hit1CdTe && hit2Si) {
      hits[0] = m_Hits[2];
      hits[1] = m_Hits[0];
      hits[2] = m_Hits[1];
      // return reconstruct3HitsSiSiCdTe(hits);
      return reconstruct3HitsSiCdTeCdTeByFOM(hits);
    }
    else {
      // to be implemented
      return false;
    }
  }
  else {
    // to be implemented
    return false;
  }

  return false;
}


bool ComptonEvent::extractEventsByTotalEnergy(double e0, double e1, int maxHits)
{
  if (NumberOfHits() > maxHits) {
    return false;
  }

  double totalE = 0.0;
  std::vector<DetectorHit_sptr>::iterator it;
  for (it=m_Hits.begin(); it!=m_Hits.end(); it++) {
    totalE += (*it)->getPI();
  }
  
  if (totalE < e0) {
    m_Hits.clear();
    return false;
  }

  if (totalE <= e1) {
    return true;
  }

  std::vector< std::vector<int> > hitListVec;
  const int NumHit = m_Hits.size();
  for (int i=0; i<NumHit; i++) {
    if (i==0) {
      std::vector<int> hitList1(1, i);
      std::vector<int> hitList0;
      hitListVec.push_back(hitList1);
      hitListVec.push_back(hitList0);
    }
    else {
      int nList = hitListVec.size();
      for (int l=0; l<nList; l++) {
        std::vector<int> hitList1(hitListVec[l]);
        hitList1.push_back(i);
        hitListVec.push_back(hitList1);
      }
    }
  }

  std::vector<double> energyVec;
  std::vector< std::vector<int> > hitListVecSelected;
  int nList = hitListVec.size();
  for (int l=0; l<nList; l++) {
    std::vector<int>& hitList = hitListVec[l];
    int nhit = hitList.size();
    double sum = 0.0;
    for (int k=0; k<nhit; k++) {
      sum += m_Hits[hitList[k]]->getPI();
    }
    if (e0<=sum && sum<=e1) {
      hitListVecSelected.push_back(hitList);
      energyVec.push_back(sum);
    }
  }

  int numHitMin = 0x8000;
  int iSelected = -1;
  for (unsigned int i=0; i<hitListVecSelected.size(); i++) {
    int nhit = hitListVecSelected[i].size();
    if (nhit < numHitMin) {
      numHitMin = nhit;
      iSelected = i;
    }
  }

  if (iSelected == -1) {
    return false;
  }

  std::vector<int>& hitListSelected = hitListVecSelected[iSelected];
  std::vector<DetectorHit_sptr> tmpHits(m_Hits);
  m_Hits.clear();
  for (unsigned int k=0; k<hitListSelected.size(); k++) {
    m_Hits.push_back(tmpHits[hitListSelected[k]]);
  }

  return true;
}


bool ComptonEvent::checkTotalEnergy(double e0, double e1) const
{
  double e = TotalEnergy();
  return (e0<=e && e<=e1);
}


bool ComptonEvent::checkDeltaTheta(double dtheta0, double dtheta1) const
{
  double dt = DeltaTheta();
  return (dtheta0<=dt && dt<=dtheta1);
}


bool ComptonEvent::reconstruct2Hits(const std::vector<DetectorHit_sptr>& hits)
{
  bool hit0Si = (hits[0]->getFlag() & comptonsoft::SI_HIT);
  bool hit1Si = (hits[1]->getFlag() & comptonsoft::SI_HIT);
  bool hit0CdTe = (hits[0]->getFlag() & comptonsoft::CDTE_HIT);
  bool hit1CdTe = (hits[1]->getFlag() & comptonsoft::CDTE_HIT);

  m_ReconstructedEvent.setH1Id(0);
  m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
  m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
  m_ReconstructedEvent.setH1Time(hits[0]->getTime());
  m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                hits[0]->getPosY(),
                                hits[0]->getPosZ());
  m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
  
  m_ReconstructedEvent.setH2Id(1);
  m_ReconstructedEvent.setH2DetId(hits[1]->getDetectorID());
  m_ReconstructedEvent.setH2Process(hits[1]->getProcess());
  m_ReconstructedEvent.setH2Time(hits[1]->getTime());
  m_ReconstructedEvent.setH2Pos(hits[1]->getPosX(),
                                hits[1]->getPosY(),
                                hits[1]->getPosZ());
  m_ReconstructedEvent.setH2Energy(hits[1]->getPI());
  
  const double ELECTRON_MASS = 511.0 * keV;
  if (m_ReconstructedEvent.TotalEnergy() < 0.5*ELECTRON_MASS) {
    if (!m_ReconstructedEvent.EnergyOrder()) {
      m_ReconstructedEvent.Swap();
    }
  }
  else {
    if (hit0Si && hit1CdTe) {
      ;
    }
    else if (hit1Si && hit0CdTe) {
      m_ReconstructedEvent.Swap();
    }
    else {
      double dtheta1 = m_ReconstructedEvent.DeltaTheta();
      m_ReconstructedEvent.Swap();
      double dtheta2 = m_ReconstructedEvent.DeltaTheta();
      if (std::abs(dtheta1) < std::abs(dtheta2)) {
        m_ReconstructedEvent.Swap();
      }
    }
  }

  int hitIDCompton = m_ReconstructedEvent.getH1Id();
  int hitIDAbsorption = m_ReconstructedEvent.getH2Id();
  if ((hit0Si&&hit1CdTe) || (hit0CdTe&&hit1Si) || (hit0CdTe&&hit1CdTe)) {
    m_FluorescenceCut = checkFluorescence(hits[hitIDCompton], hits[hitIDAbsorption]);
  }
  else {
    m_FluorescenceCut = false;
  }

  return true;
}


bool ComptonEvent::reconstruct3HitsSiSiCdTe(const std::vector<DetectorHit_sptr>& hits)
{
  if (checkFluorescence(hits[0], hits[2])) {
    // 0: Fl, 1: Comp, 2: Abs
    m_ReconstructedEvent.setH1Id(1);
    m_ReconstructedEvent.setH1DetId(hits[1]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[1]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[1]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[1]->getPosX(),
                                  hits[1]->getPosY(),
                                  hits[1]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[1]->getPI());
    
    m_ReconstructedEvent.setH2Id(2);
    m_ReconstructedEvent.setH2DetId(hits[2]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[2]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[2]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[2]->getPosX(),
                                  hits[2]->getPosY(),
                                  hits[2]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[0]->getPI()+hits[2]->getPI()); 
  }
  else if (checkFluorescence(hits[1], hits[2])) {
    // 0: Comp, 1: Fl, 2: Abs
    m_ReconstructedEvent.setH1Id(0);
    m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[0]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                  hits[0]->getPosY(),
                                  hits[0]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
    
    m_ReconstructedEvent.setH2Id(2);
    m_ReconstructedEvent.setH2DetId(hits[2]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[2]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[2]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[2]->getPosX(),
                                  hits[2]->getPosY(),
                                  hits[2]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
  }
  else {
    // 0: Comp1, 1: Comp2, 2: Abs
    m_ReconstructedEvent.setH1Id(0);
    m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[0]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                  hits[0]->getPosY(),
                                  hits[0]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
    
    m_ReconstructedEvent.setH2Id(1);
    m_ReconstructedEvent.setH2DetId(hits[1]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[1]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[1]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[1]->getPosX(),
                                  hits[1]->getPosY(),
                                  hits[1]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
    
    double dtheta1 = m_ReconstructedEvent.DeltaTheta();
    
    // 0: Comp2, 1: Comp1, 2: Abs
    m_ReconstructedEvent.setH1Id(1);
    m_ReconstructedEvent.setH1DetId(hits[1]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[1]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[1]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[1]->getPosX(),
                                  hits[1]->getPosY(),
                                  hits[1]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[1]->getPI());
    
    m_ReconstructedEvent.setH2Id(0);
    m_ReconstructedEvent.setH2DetId(hits[0]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[0]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[0]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[0]->getPosX(),
                                  hits[0]->getPosY(),
                                  hits[0]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[0]->getPI()+hits[2]->getPI()); 
    
    double dtheta2 = m_ReconstructedEvent.DeltaTheta();
    if (std::abs(dtheta1) < std::abs(dtheta2)) {
      // 0: Comp1, 1: Comp2, 2: Abs
      m_ReconstructedEvent.setH1Id(0);
      m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
      m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
      m_ReconstructedEvent.setH1Time(hits[0]->getTime());
      m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                    hits[0]->getPosY(),
                                    hits[0]->getPosZ());
      m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
      
      m_ReconstructedEvent.setH2Id(1);
      m_ReconstructedEvent.setH2DetId(hits[1]->getDetectorID());
      m_ReconstructedEvent.setH2Process(hits[1]->getProcess());
      m_ReconstructedEvent.setH2Time(hits[1]->getTime());
      m_ReconstructedEvent.setH2Pos(hits[1]->getPosX(),
                                    hits[1]->getPosY(),
                                    hits[1]->getPosZ());
      m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
    }   
  }

  return true;
}


bool ComptonEvent::reconstruct3HitsSiCdTeCdTe(const std::vector<DetectorHit_sptr>& hits)
{
  if (checkFluorescence(hits[1], hits[2])) {
    // 0: Comp, 1: Fl, 2: Abs
    m_ReconstructedEvent.setH1Id(0);
    m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[0]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                  hits[0]->getPosY(),
                                  hits[0]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
    
    m_ReconstructedEvent.setH2Id(2);
    m_ReconstructedEvent.setH2DetId(hits[2]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[2]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[2]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[2]->getPosX(),
                                  hits[2]->getPosY(),
                                  hits[2]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
  }
  else if (checkFluorescence(hits[2], hits[1])) {
    // 0: Comp, 1: Abs, 2: Fl
    m_ReconstructedEvent.setH1Id(0);
    m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[0]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                  hits[0]->getPosY(),
                                  hits[0]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
    
    m_ReconstructedEvent.setH2Id(1);
    m_ReconstructedEvent.setH2DetId(hits[1]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[1]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[1]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[1]->getPosX(),
                                  hits[1]->getPosY(),
                                  hits[1]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
  }
  else {
    // 0: Comp1, 1: Comp2, 2: Abs
    m_ReconstructedEvent.setH1Id(0);
    m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[0]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                  hits[0]->getPosY(),
                                  hits[0]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
    
    m_ReconstructedEvent.setH2Id(1);
    m_ReconstructedEvent.setH2DetId(hits[1]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[1]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[1]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[1]->getPosX(),
                                  hits[1]->getPosY(),
                                  hits[1]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
    
    double dtheta1 = m_ReconstructedEvent.DeltaTheta();
    
    // 0: Comp1, 1: Abs, 2: Comp2
    m_ReconstructedEvent.setH1Id(0);
    m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[0]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                  hits[0]->getPosY(),
                                  hits[0]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
    
    m_ReconstructedEvent.setH2Id(2);
    m_ReconstructedEvent.setH2DetId(hits[2]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[2]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[2]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[2]->getPosX(),
                                  hits[2]->getPosY(),
                                  hits[2]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
    
    double dtheta2 = m_ReconstructedEvent.DeltaTheta();
    if (std::abs(dtheta1) < std::abs(dtheta2)) {
      // 0: Comp1, 1: Comp2, 2: Abs
      m_ReconstructedEvent.setH1Id(0);
      m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
      m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
      m_ReconstructedEvent.setH1Time(hits[0]->getTime());
      m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                    hits[0]->getPosY(),
                                    hits[0]->getPosZ());
      m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
      
      m_ReconstructedEvent.setH2Id(1);
      m_ReconstructedEvent.setH2DetId(hits[1]->getDetectorID());
      m_ReconstructedEvent.setH2Process(hits[1]->getProcess());
      m_ReconstructedEvent.setH2Time(hits[1]->getTime());
      m_ReconstructedEvent.setH2Pos(hits[1]->getPosX(),
                                    hits[1]->getPosY(),
                                    hits[1]->getPosZ());
      m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
    }   
  }

  return true;
}


bool ComptonEvent::reconstruct3HitsSiCdTeCdTeByFOM(const std::vector<DetectorHit_sptr>& hits)
{
  if (checkFluorescence(hits[1], hits[2])) {
    // 0: Comp, 1: Fl, 2: Abs
    m_ReconstructedEvent.setH1Id(0);
    m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[0]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                  hits[0]->getPosY(),
                                  hits[0]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
    
    m_ReconstructedEvent.setH2Id(2);
    m_ReconstructedEvent.setH2DetId(hits[2]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[2]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[2]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[2]->getPosX(),
                                  hits[2]->getPosY(),
                                  hits[2]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
  }
  else if (checkFluorescence(hits[2], hits[1])) {
    // 0: Comp, 1: Abs, 2: Fl
    m_ReconstructedEvent.setH1Id(0);
    m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
    m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
    m_ReconstructedEvent.setH1Time(hits[0]->getTime());
    m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                  hits[0]->getPosY(),
                                  hits[0]->getPosZ());
    m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
    
    m_ReconstructedEvent.setH2Id(1);
    m_ReconstructedEvent.setH2DetId(hits[1]->getDetectorID());
    m_ReconstructedEvent.setH2Process(hits[1]->getProcess());
    m_ReconstructedEvent.setH2Time(hits[1]->getTime());
    m_ReconstructedEvent.setH2Pos(hits[1]->getPosX(),
                                  hits[1]->getPosY(),
                                  hits[1]->getPosZ());
    m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
  }
  else {
    // 0: Comp1, 1: Comp2, 2: Abs
    TwoHitComptonEvent compton12;
    compton12.setSourcePosition(hits[0]->getPosX(),
                                hits[0]->getPosY(),
                                hits[0]->getPosZ());
    compton12.setH1Id(1);
    compton12.setH1DetId(hits[1]->getDetectorID());
    compton12.setH1Process(hits[1]->getProcess());
    compton12.setH1Time(hits[1]->getTime());
    compton12.setH1Pos(hits[1]->getPosX(),
                       hits[1]->getPosY(),
                       hits[1]->getPosZ());
    compton12.setH1Energy(hits[1]->getPI());
    
    compton12.setH2Id(2);
    compton12.setH2DetId(hits[2]->getDetectorID());
    compton12.setH2Process(hits[2]->getProcess());
    compton12.setH2Time(hits[2]->getTime());
    compton12.setH2Pos(hits[2]->getPosX(),
                       hits[2]->getPosY(),
                       hits[2]->getPosZ());
    compton12.setH2Energy(hits[2]->getPI());
    double dtheta12 = compton12.DeltaTheta();

    // 0: Comp1, 1: Abs, 2: Comp2
    TwoHitComptonEvent compton21;
    compton21.setSourcePosition(hits[0]->getPosX(),
                                hits[0]->getPosY(),
                                hits[0]->getPosZ());
    compton21.setH1Id(2);
    compton21.setH1DetId(hits[2]->getDetectorID());
    compton21.setH1Process(hits[2]->getProcess());
    compton21.setH1Time(hits[2]->getTime());
    compton21.setH1Pos(hits[2]->getPosX(),
                       hits[2]->getPosY(),
                       hits[2]->getPosZ());
    compton21.setH1Energy(hits[2]->getPI());
    
    compton21.setH2Id(1);
    compton21.setH2DetId(hits[1]->getDetectorID());
    compton21.setH2Process(hits[1]->getProcess());
    compton21.setH2Time(hits[1]->getTime());
    compton21.setH2Pos(hits[1]->getPosX(),
                       hits[1]->getPosY(),
                       hits[1]->getPosZ());
    compton21.setH2Energy(hits[1]->getPI()); 
    double dtheta21 = compton21.DeltaTheta();
    
    if (std::abs(dtheta12) < std::abs(dtheta21)) {
      // 0: Comp1, 1: Comp2, 2: Abs
      m_ReconstructedEvent.setH1Id(0);
      m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
      m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
      m_ReconstructedEvent.setH1Time(hits[0]->getTime());
      m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                    hits[0]->getPosY(),
                                    hits[0]->getPosZ());
      m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
      
      m_ReconstructedEvent.setH2Id(1);
      m_ReconstructedEvent.setH2DetId(hits[1]->getDetectorID());
      m_ReconstructedEvent.setH2Process(hits[1]->getProcess());
      m_ReconstructedEvent.setH2Time(hits[1]->getTime());
      m_ReconstructedEvent.setH2Pos(hits[1]->getPosX(),
                                    hits[1]->getPosY(),
                                    hits[1]->getPosZ());
      m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
    }
    else {
      // 0: Comp1, 1: Abs, 2: Comp2
      m_ReconstructedEvent.setH1Id(0);
      m_ReconstructedEvent.setH1DetId(hits[0]->getDetectorID());
      m_ReconstructedEvent.setH1Process(hits[0]->getProcess());
      m_ReconstructedEvent.setH1Time(hits[0]->getTime());
      m_ReconstructedEvent.setH1Pos(hits[0]->getPosX(),
                                    hits[0]->getPosY(),
                                    hits[0]->getPosZ());
      m_ReconstructedEvent.setH1Energy(hits[0]->getPI());
    
      m_ReconstructedEvent.setH2Id(2);
      m_ReconstructedEvent.setH2DetId(hits[2]->getDetectorID());
      m_ReconstructedEvent.setH2Process(hits[2]->getProcess());
      m_ReconstructedEvent.setH2Time(hits[2]->getTime());
      m_ReconstructedEvent.setH2Pos(hits[2]->getPosX(),
                                    hits[2]->getPosY(),
                                    hits[2]->getPosZ());
      m_ReconstructedEvent.setH2Energy(hits[1]->getPI()+hits[2]->getPI()); 
    }   
  }

  return true;
}


bool ComptonEvent::checkFluorescence(DetectorHit_sptr fl, DetectorHit_sptr abs) const
{
  const double FluorCutLengthSiCdTe = 10.0 * mm;
  const double FluorCutLengthCdTeCdTe = 7.0 * mm;
  
  if (fl->getFlag() & comptonsoft::CDTE_FLUOR) {
    if ((fl->getFlag() & comptonsoft::SI_HIT) | (abs->getFlag() & comptonsoft::CDTE_HIT)) {
      if (fl->distance(*abs) < FluorCutLengthSiCdTe) return true;
    }
    else if ((fl->getFlag() & comptonsoft::CDTE_HIT) | (abs->getFlag() & comptonsoft::CDTE_HIT)) {
      if (fl->distance(*abs) < FluorCutLengthCdTeCdTe) return true;
    }
  }

  return false;
}

}
