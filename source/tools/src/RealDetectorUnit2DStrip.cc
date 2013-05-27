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

// RealDetectorUnit2DStrip.cc
//
// 2008-08-25 Hirokazu Odaka
//

#include "RealDetectorUnit2DStrip.hh"
#include "DetectorHit.hh"
#include "HXISGDFlagDef.hh"

namespace comptonsoft {

RealDetectorUnit2DStrip::RealDetectorUnit2DStrip()
  : m_PriorityToAnodeSide(false)
{
  m_CathodeSideHits.clear();
  m_AnodeSideHits.clear();
}


RealDetectorUnit2DStrip::~RealDetectorUnit2DStrip()
{
}


int RealDetectorUnit2DStrip::analyzeHits()
{
  int mode = AnalysisMode();
  RealDetectorUnit::analyzeHits();

  if (mode == 0) {
    ;
  }
  else if (mode == 1) {
    analyzeMaxPHAHit();
  }
  else if (mode == 2) {
    analyzeSingleHit();
  }
  else if (mode == 3) {
    cluster();
    analyzeMaxPHAHit();
  }
  else if (mode == 4) {
    cluster();
    analyzeSingleHit();
  }
  else if (mode == 5) {
    cluster();
  }
  else if (mode == 100) {
    prepareDoubleSideHits();
  }

  return NumAnalyzedHits();
}


void RealDetectorUnit2DStrip::clearAnalyzedHits()
{
  RealDetectorUnit::clearAnalyzedHits();
  m_CathodeSideHits.clear();
  m_AnodeSideHits.clear();
}


void RealDetectorUnit2DStrip::cluster()
{
  std::vector<DetectorHit_sptr>::iterator it1;
  std::vector<DetectorHit_sptr>::iterator it2;
  
  for (it1 = m_AnalyzedHits.begin(); it1 != m_AnalyzedHits.end(); ++it1) {
    it2 = it1;
    ++it2;

    while ( it2 != m_AnalyzedHits.end() ) {
      if ( (*it2)->areAdjacent(**it1) ) {
	(*it1)->addAdjacent(**it2);
	it2 = m_AnalyzedHits.erase(it2);
      }
      else {
	++it2;
      }
    }
  }
}


void RealDetectorUnit2DStrip::prepareDoubleSideHits()
{
  m_CathodeSideHits.clear();
  m_AnodeSideHits.clear();

  std::vector<DetectorHit_sptr>::iterator it;
  std::vector<DetectorHit_sptr>::iterator itEnd = m_AnalyzedHits.end();

  for (it = m_AnalyzedHits.begin(); it != itEnd; ++it) {
    if ( (*it)->isAnodeSide() ) {
      m_AnodeSideHits.push_back(*it);
    }
    else {
      m_CathodeSideHits.push_back(*it);
    }
  }
}


void RealDetectorUnit2DStrip::analyzeMaxPHAHit()
{
  prepareDoubleSideHits();

  if (m_CathodeSideHits.size() < 1 || m_AnodeSideHits.size() < 1) {
    clearAnalyzedHits();
    return;
  }

  // P-side scan
  std::vector<DetectorHit_sptr>::iterator itHitCathodeSide = m_CathodeSideHits.begin();
  std::vector<DetectorHit_sptr>::iterator itHitCathodeSideEnd = m_CathodeSideHits.end();
  std::vector<DetectorHit_sptr>::iterator itHitCathodeSideMax = itHitCathodeSide;
  double ePIMaxCathodeSide = 0.0;
   
  while ( itHitCathodeSide != itHitCathodeSideEnd ) {
    if ((*itHitCathodeSide)->getPI() > ePIMaxCathodeSide) {
      ePIMaxCathodeSide = (*itHitCathodeSide)->getPI();
      itHitCathodeSideMax = itHitCathodeSide;
    }
    itHitCathodeSide++;
  }

  // N-side scan
  std::vector<DetectorHit_sptr>::iterator itHitAnodeSide = m_AnodeSideHits.begin();
  std::vector<DetectorHit_sptr>::iterator itHitAnodeSideEnd = m_AnodeSideHits.end();
  std::vector<DetectorHit_sptr>::iterator itHitAnodeSideMax = itHitAnodeSide;
  double ePIMaxAnodeSide = 0.0;
  
  while ( itHitAnodeSide != itHitAnodeSideEnd ) {
    if ((*itHitAnodeSide)->getPI() > ePIMaxAnodeSide) {
      ePIMaxAnodeSide = (*itHitAnodeSide)->getPI();
      itHitAnodeSideMax = itHitAnodeSide;
    }
    itHitAnodeSide++;
  }
  
  double posx, posy, posz;
  double lposx, lposy, lposz;
  StripPair spCathodeSide = (*itHitCathodeSideMax)->getStrip();
  StripPair spAnodeSide = (*itHitAnodeSideMax)->getStrip();
  StripPair spComb = spCathodeSide.Combine(spAnodeSide);
  Position(spComb.X(), spComb.Y(), &posx, &posy, &posz);
  LocalPosition(spComb.X(), spComb.Y(), &lposx, &lposy, &lposz);
    
  DetectorHit_sptr aHit(new DetectorHit(PriorityToAnodeSide() ? 
					(**itHitAnodeSideMax) : 
					(**itHitCathodeSideMax)));
  aHit->setPos(posx, posy, posz);
  aHit->setLocalPos(lposx, lposy, lposz);
  aHit->setStrip(spComb.X(), spComb.Y());

  clearAnalyzedHits();
  insertAnalyzedHit(aHit);
}


void RealDetectorUnit2DStrip::analyzeSingleHit()
{
  prepareDoubleSideHits();

  if (m_CathodeSideHits.size() != 1 || m_AnodeSideHits.size() != 1) {
    clearAnalyzedHits();
    prepareDoubleSideHits();
    return;
  }

  // P-side scan
  std::vector<DetectorHit_sptr>::iterator itHitCathodeSide = m_CathodeSideHits.begin();
  std::vector<DetectorHit_sptr>::iterator itHitCathodeSideMax = itHitCathodeSide;

  // N-side scan
  std::vector<DetectorHit_sptr>::iterator itHitAnodeSide = m_AnodeSideHits.begin();
  std::vector<DetectorHit_sptr>::iterator itHitAnodeSideMax = itHitAnodeSide;

  double posx, posy, posz;
  double lposx, lposy, lposz;
  StripPair spCathodeSide = (*itHitCathodeSideMax)->getStrip();
  StripPair spAnodeSide = (*itHitAnodeSideMax)->getStrip();
  StripPair spComb = spCathodeSide.Combine(spAnodeSide);
  Position(spComb.X(), spComb.Y(), &posx, &posy, &posz);
  LocalPosition(spComb.X(), spComb.Y(), &lposx, &lposy, &lposz);
  
  DetectorHit_sptr aHit(new DetectorHit(PriorityToAnodeSide() ? 
					(**itHitAnodeSideMax) : 
					(**itHitCathodeSideMax)));
  aHit->setPos(posx, posy, posz);
  aHit->setLocalPos(lposx, lposy, lposz);
  aHit->setStrip(spComb.X(), spComb.Y());
  
  clearAnalyzedHits();
  insertAnalyzedHit(aHit);
}

}
