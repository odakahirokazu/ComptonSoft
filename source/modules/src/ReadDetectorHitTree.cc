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

#include "ReadDetectorHitTree.hh"

#include "DetectorHit.hh"
#include "RealDetectorUnit.hh"

using namespace comptonsoft;
using namespace anl;


ANLStatus ReadDetectorHitTree::mod_ana()
{
  ANLStatus status = ReadRawHitTree::mod_ana();
  if (status == AS_OK) {
    std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
    std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
    while ( itDet != itDetEnd ) {
      (*itDet)->assignDetectorInfo();
      itDet++;
    }
  }
  return status;
}


void ReadDetectorHitTree::insertHit(int detid, DetectorHit_sptr hit)
{
  RealDetectorUnit* det
    = GetDetectorManager()->getDetectorByID(detid);
  det->insertDetectorHit(hit);
}
