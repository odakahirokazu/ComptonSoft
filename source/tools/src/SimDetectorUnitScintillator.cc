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

// SimDetectorUnitScintillator.cc
//
// 2008-08-25 Hirokazu Odaka
//

#include "SimDetectorUnitScintillator.hh"

#include "globals.hh"

#include "HXISGDFlagDef.hh"
#include "DetectorHit.hh"

namespace comptonsoft {

SimDetectorUnitScintillator::SimDetectorUnitScintillator()
{
  registerDetectorUnit( static_cast<RealDetectorUnit*>(this) );
  
  setNoiseFWHMAll(1.0, 2.57, 0.0); // 10% at 662 keV
}


SimDetectorUnitScintillator::~SimDetectorUnitScintillator()
{
}


void SimDetectorUnitScintillator::simulatePulseHeights()
{
  std::vector<DetectorHit_sptr>::iterator it;
  std::vector<DetectorHit_sptr>::iterator itEnd = m_RawHits.end();

  for (it = m_RawHits.begin(); it != itEnd; it++) {
    double edep = (*it)->getEdep();

    if ((*it)->isProcess(comptonsoft::QUENCHING)) {
      edep *= QuenchingFactor();
    }
    
    DetectorHit_sptr hit(new DetectorHit(**it));
    hit->setStrip(0, 0);
    hit->setPHA(edep);
    
    insertSimulatedHit(hit);
  }
}

}
