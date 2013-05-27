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

// RealDetectorUnitScintillator.cc
//
// 2008-09-17 Hirokazu Odaka
//

#include "RealDetectorUnitScintillator.hh"
#include "DetectorHit.hh"

namespace comptonsoft {

RealDetectorUnitScintillator::RealDetectorUnitScintillator()
{
}


RealDetectorUnitScintillator::~RealDetectorUnitScintillator()
{
}


int RealDetectorUnitScintillator::analyzeHits()
{
  int mode = AnalysisMode();
  RealDetectorUnit::analyzeHits();

  if (mode == 0) { return NumAnalyzedHits(); }

  determinePosition();

  if (mode == 1) { return NumAnalyzedHits(); }

  return NumAnalyzedHits();
}


void RealDetectorUnitScintillator::determinePosition()
{
  std::vector<DetectorHit_sptr>::iterator it;
  std::vector<DetectorHit_sptr>::iterator itEnd = m_AnalyzedHits.end();

  for (it = m_AnalyzedHits.begin(); it != itEnd; it++) {
    (*it)->setPos(getCenterPositionX(),
		  getCenterPositionY(),
		  getCenterPositionZ());
    (*it)->setLocalPos(0., 0., 0.);
  }
}

}
