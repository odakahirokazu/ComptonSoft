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

// DetectorHit.cc
//
// 2008-08-22 Hirokazu Odaka
// 2009-04-14 Hirokazu Odaka rename (from HXISGD2DStripHit)
//

#include "DetectorHit.hh"

#include "HXISGDFlagDef.hh"

namespace comptonsoft {

#if DetectorHit_BoostPool
boost::pool<> DetectorHitAllocator(sizeof(DetectorHit));
#endif

#define ENERGY_WEIGHT 1

const DetectorHit& DetectorHit::operator+=(const DetectorHit& r)
{
  if (edep > 0. && r.edep > 0.) {
#if ENERGY_WEIGHT
    posx = (posx * edep + r.posx * r.edep)/(edep+r.edep);
    posy = (posy * edep + r.posy * r.edep)/(edep+r.edep);
    posz = (posz * edep + r.posz * r.edep)/(edep+r.edep);
    localposx = (localposx * edep + r.localposx * r.edep)/(edep+r.edep);
    localposy = (localposy * edep + r.localposy * r.edep)/(edep+r.edep);
    localposz = (localposz * edep + r.localposz * r.edep)/(edep+r.edep);
    realposx = (realposx * edep + r.realposx * r.edep)/(edep+r.edep);
    realposy = (realposy * edep + r.realposy * r.edep)/(edep+r.edep);
    realposz = (realposz * edep + r.realposz * r.edep)/(edep+r.edep);
#else
    posx = 0.5*(posx+r.posx);
    posy = 0.5*(posy+r.posy);
    posz = 0.5*(posz+r.posz);
    localposx = 0.5*(localposx+r.localposx);
    localposy = 0.5*(localposy+r.localposy);
    localposz = 0.5*(localposz+r.localposz);
    realposx = 0.5*(realposx+r.realposx);
    realposy = 0.5*(realposy+r.realposy);
    realposz = 0.5*(realposz+r.realposz);
#endif
  }
  else if (r.edep > 0.) {
    posx = r.posx;
    posy = r.posy;
    posz = r.posz;
    localposx = r.localposx;
    localposy = r.localposy;
    localposz = r.localposz;
    realposx = r.realposx;
    realposy = r.realposy;
    realposz = r.realposz;
  }
  
  edep += r.edep;
  e_pha += r.e_pha;
  e_pi += r.e_pi;

  if (time > r.time) { time = r.time; }
  if (time_group > r.time_group) { time_group = r.time_group; }

  process |= r.process;
  flag |= flag;
  
  return *this;
}


bool DetectorHit::areAdjacent(const DetectorHit& r, int mode) const
{
  if (instrument_id == r.instrument_id
      && detector_id == r.detector_id && time_group == r.time_group) {
    if (stripx >=0 && stripy >= 0 && r.stripx >= 0 && r.stripx >= 0) {
      // pad detector
      if (mode == 1) {
	return ( (stripx == r.stripx && ( stripy-r.stripy == 1 || stripy-r.stripy == -1 ) )
		 || (stripy == r.stripy && ( stripx-r.stripx == 1 || stripx-r.stripx == -1 ) ) );
      }
      else if (mode == 2) {
	return ( ( -1 <= stripx-r.stripx && stripx-r.stripx <= 1 )
		 && ( -1 <= stripy-r.stripy && stripy-r.stripy <= 1 ) );
      }
    }
    else if (stripx ==-1 && r.stripx == -1) {
      // strip detector (y-side)
      return (stripy-r.stripy == 1 || stripy-r.stripy == -1);
    }
    else if (stripy ==-1 && r.stripy == -1) {
      // strip detector (x-side)
      return (stripx-r.stripx == 1 || stripx-r.stripx == -1);
    }
  }
  
  return false;
}


void DetectorHit::addAdjacent(const DetectorHit& r)
{
  if (edep > 0. && r.edep > 0.) {
#if ENERGY_WEIGHT
    posx = (posx * edep + r.posx * r.edep)/(edep+r.edep);
    posy = (posy * edep + r.posy * r.edep)/(edep+r.edep);
    posz = (posz * edep + r.posz * r.edep)/(edep+r.edep);
    localposx = (localposx * edep + r.localposx * r.edep)/(edep+r.edep);
    localposy = (localposy * edep + r.localposy * r.edep)/(edep+r.edep);
    localposz = (localposz * edep + r.localposz * r.edep)/(edep+r.edep);
    realposx = (realposx * edep + r.realposx * r.edep)/(edep+r.edep);
    realposy = (realposy * edep + r.realposy * r.edep)/(edep+r.edep);
    realposz = (realposz * edep + r.realposz * r.edep)/(edep+r.edep);
#else
    posx = 0.5*(posx+r.posx);
    posy = 0.5*(posy+r.posy);
    posz = 0.5*(posz+r.posz);
    localposx = 0.5*(localposx+r.localposx);
    localposy = 0.5*(localposy+r.localposy);
    localposz = 0.5*(localposz+r.localposz);
    realposx = 0.5*(realposx+r.realposx);
    realposy = 0.5*(realposy+r.realposy);
    realposz = 0.5*(realposz+r.realposz);
#endif
  }
  else if (r.edep > 0.) {
    posx = r.posx;
    posy = r.posy;
    posz = r.posz;
    localposx = r.localposx;
    localposy = r.localposy;
    localposz = r.localposz;
    realposx = r.realposx;
    realposy = r.realposy;
    realposz = r.realposz;
  }

  edep += r.edep;
  e_pha += r.e_pha;
  e_pi += r.e_pi;

  if (time > r.time) { time = r.time; }
  if (time_group > r.time_group) { time_group = r.time_group; }

  process |= r.process;
  flag |= r.flag;
  flag |= comptonsoft::CLUSTERED;
    
  if(edep < r.edep) {
    stripx = r.stripx;
    stripy = r.stripy;
  }
}

}
