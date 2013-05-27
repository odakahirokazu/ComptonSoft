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

// CSHitCollection.cc
// 2008-08-28 Hirokazu Odaka

#include "CSHitCollection.hh"
#include "DetectorHit.hh"

using namespace comptonsoft;
using namespace anl;


ANLStatus CSHitCollection::mod_ana()
{
  hitvec_tg.clear();
  hitvec_notg.clear();

  return AS_OK;
}


ANLStatus CSHitCollection::mod_endrun()
{
  hitvec_tg.clear();
  hitvec_notg.clear();
  
  return AS_OK;
}


void CSHitCollection::InsertHit(DetectorHit_sptr hit)
{
  int timeGroup = hit->getTimeGroup();
  if (timeGroup == comptonsoft::NOTIMEGROUP) {
    hitvec_notg.push_back(hit);
  }
  else {
    int timeGroupSize = hitvec_tg.size();
    if (timeGroup >= timeGroupSize) {
      hitvec_tg.resize(timeGroup+1);
    }
    hitvec_tg[timeGroup].push_back(hit);
  }
}
