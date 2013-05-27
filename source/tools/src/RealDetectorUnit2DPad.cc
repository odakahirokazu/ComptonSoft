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

// RealDetectorUnit2DPad.cc
//
// 2008-08-25 Hirokazu Odaka
//

#include "RealDetectorUnit2DPad.hh"
#include "DetectorHit.hh"

namespace comptonsoft {

RealDetectorUnit2DPad::RealDetectorUnit2DPad()
{
}


RealDetectorUnit2DPad::~RealDetectorUnit2DPad()
{
}


int RealDetectorUnit2DPad::analyzeHits()
{
  int mode = AnalysisMode();
  RealDetectorUnit::analyzeHits();

  if (mode == 0) { return NumAnalyzedHits(); }

  determinePosition();

  if (mode == 1) { return NumAnalyzedHits(); }

  cluster();

  if (mode == 2) { return NumAnalyzedHits(); }

  return NumAnalyzedHits();
}


void RealDetectorUnit2DPad::determinePosition()
{
  std::vector<DetectorHit_sptr>::iterator it;
  std::vector<DetectorHit_sptr>::iterator itEnd = m_AnalyzedHits.end();

  double posx, posy, posz;
  double lposx, lposy, lposz;

  for (it = m_AnalyzedHits.begin(); it != itEnd; ++it) {
    StripPair sp = (*it)->getStrip();
    Position(sp.X(), sp.Y(), &posx, &posy, &posz);
    LocalPosition(sp.X(), sp.Y(), &lposx, &lposy, &lposz);
    (*it)->setPos(posx, posy, posz);
    (*it)->setLocalPos(lposx, lposy, lposz);
  }
}


void RealDetectorUnit2DPad::cluster()
{
  std::vector<DetectorHit_sptr>::iterator it1;
  std::vector<DetectorHit_sptr>::iterator it2;
  
  for (it1 = m_AnalyzedHits.begin(); it1 != m_AnalyzedHits.end(); ++it1) {
    it2 = it1;
    ++it2;

    while ( it2 != m_AnalyzedHits.end() ) {
      if ( (*it2)->areAdjacent(**it1) ) {
	**it1 += **it2;
	it2 = m_AnalyzedHits.erase(it2);
      }
      else {
	++it2;
      }
    }
  }
}

}
