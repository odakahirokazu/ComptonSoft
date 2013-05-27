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

#include "HXIEventSelection.hh"
#include <boost/format.hpp>
#include "DetectorHit.hh"

using namespace anl;
using namespace comptonsoft;


ANLStatus HXIEventSelection::mod_init()
{
  VCSModule::mod_init();
  GetANLModuleNC("CSHitCollection", &hit_collection);

  for (std::vector<RealDetectorUnit*>::iterator it=GetDetectorVector().begin(); it!=GetDetectorVector().end(); ++it) {
    int detid = (*it)->getID();
    EvsDef((boost::format("HXIEventSelection:detection:%04d") % detid).str());
    EvsDef((boost::format("HXIEventSelection:1hit:%04d") % detid).str());
  }

  EvsDef("HXIEventSelection:1hit");
  EvsDef("HXIEventSelection:1hit:Si");
  EvsDef("HXIEventSelection:1hit:CdTe");
  EvsDef("HXIEventSelection:2hit");
  EvsDef("HXIEventSelection:2hit:Si-Si");
  EvsDef("HXIEventSelection:2hit:Si-CdTe");
  EvsDef("HXIEventSelection:2hit:CdTe-CdTe");
  EvsDef("HXIEventSelection:2hit:Si-CdTe:fluorescence");
  EvsDef("HXIEventSelection:2hit:Si-CdTe:not-fluorescence");
  EvsDef("HXIEventSelection:2hit:CdTe-CdTe:fluorescence");
  EvsDef("HXIEventSelection:2hit:CdTe-CdTe:not-fluorescence");

  return AS_OK;
}


ANLStatus HXIEventSelection::mod_ana()
{
  typedef std::vector<DetectorHit_sptr> HitVector;
  
  HitVector& hitVec = hit_collection->GetHits(comptonsoft::NOTIMEGROUP);
  for (HitVector::iterator it=hitVec.begin(); it!=hitVec.end(); ++it) {
    int detid = (*it)->getDetectorID();
    EvsSet((boost::format("HXIEventSelection:detection:%04d") % detid).str());
  }

  if (hitVec.size() == 1) {
    EvsSet("HXIEventSelection:1hit");
    const_DetectorHit_sptr hit = hitVec[0];
    if (hit->isFlag(SI_HIT)) EvsSet("HXIEventSelection:1hit:Si");
    if (hit->isFlag(CDTE_HIT)) EvsSet("HXIEventSelection:1hit:CdTe");
    int detid = hit->getDetectorID();
    EvsSet((boost::format("HXIEventSelection:1hit:%04d") % detid).str());
  }
  else if (hitVec.size() == 2) {
    EvsSet("HXIEventSelection:2hit");
    const_DetectorHit_sptr hit0 = hitVec[0];
    const_DetectorHit_sptr hit1 = hitVec[1];
    if (hit0->isFlag(SI_HIT) && hit1->isFlag(SI_HIT)) {
      EvsSet("HXIEventSelection:2hit:Si-Si");
    }
    else if (hit0->isFlag(SI_HIT) && hit1->isFlag(CDTE_HIT)) {
      EvsSet("HXIEventSelection:2hit:Si-CdTe");
      if (hit0->isFlag(CDTE_FLUOR)) {
        EvsSet("HXIEventSelection:2hit:Si-CdTe:fluorescence");
      }
      else {
        EvsSet("HXIEventSelection:2hit:Si-CdTe:not-fluorescence");
      }
    }
    else if (hit1->isFlag(SI_HIT) && hit0->isFlag(CDTE_HIT)) {
      EvsSet("HXIEventSelection:2hit:Si-CdTe");
      if (hit1->isFlag(CDTE_FLUOR)) {
        EvsSet("HXIEventSelection:2hit:Si-CdTe:fluorescence");
      }
      else {
        EvsSet("HXIEventSelection:2hit:Si-CdTe:not-fluorescence");
      }
    }
    else if (hit1->isFlag(CDTE_HIT) && hit0->isFlag(CDTE_HIT)) {
      EvsSet("HXIEventSelection:2hit:CdTe-CdTe");
      if (hit0->isFlag(CDTE_FLUOR) || hit1->isFlag(CDTE_FLUOR)) {
        EvsSet("HXIEventSelection:2hit:CdTe-CdTe:fluorescence");
      }
      else {
        EvsSet("HXIEventSelection:2hit:CdTe-CdTe:not-fluorescence");
      }
    }
  }
  
  return AS_OK;
}
