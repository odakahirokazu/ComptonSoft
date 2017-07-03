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
#include "FlagDefinition.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace comptonsoft
{

ANLStatus HXIEventSelection::mod_init()
{
  VCSModule::mod_init();
  GetModuleNC("CSHitCollection", &m_HitCollection);

  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    int detid = detector->getID();
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
  auto& hits = m_HitCollection->getHits();
  for (auto& hit: hits) {
    int detid = hit->DetectorID();
    EvsSet((boost::format("HXIEventSelection:detection:%04d") % detid).str());
  }

  if (hits.size() == 1) {
    EvsSet("HXIEventSelection:1hit");
    const_DetectorHit_sptr hit = hits[0];
    if (hit->isFlags(flag::LowZHit)) EvsSet("HXIEventSelection:1hit:Si");
    if (hit->isFlags(flag::HighZHit)) EvsSet("HXIEventSelection:1hit:CdTe");
    int detid = hit->DetectorID();
    EvsSet((boost::format("HXIEventSelection:1hit:%04d") % detid).str());
  }
  else if (hits.size() == 2) {
    EvsSet("HXIEventSelection:2hit");
    const_DetectorHit_sptr hit0 = hits[0];
    const_DetectorHit_sptr hit1 = hits[1];
    if (hit0->isFlags(flag::LowZHit) && hit1->isFlags(flag::LowZHit)) {
      EvsSet("HXIEventSelection:2hit:Si-Si");
    }
    else if (hit0->isFlags(flag::LowZHit) && hit1->isFlags(flag::HighZHit)) {
      EvsSet("HXIEventSelection:2hit:Si-CdTe");
      if (hit0->isFlags(flag::FluorescenceHit)) {
        EvsSet("HXIEventSelection:2hit:Si-CdTe:fluorescence");
      }
      else {
        EvsSet("HXIEventSelection:2hit:Si-CdTe:not-fluorescence");
      }
    }
    else if (hit1->isFlags(flag::LowZHit) && hit0->isFlags(flag::HighZHit)) {
      EvsSet("HXIEventSelection:2hit:Si-CdTe");
      if (hit1->isFlags(flag::FluorescenceHit)) {
        EvsSet("HXIEventSelection:2hit:Si-CdTe:fluorescence");
      }
      else {
        EvsSet("HXIEventSelection:2hit:Si-CdTe:not-fluorescence");
      }
    }
    else if (hit1->isFlags(flag::HighZHit) && hit0->isFlags(flag::HighZHit)) {
      EvsSet("HXIEventSelection:2hit:CdTe-CdTe");
      if (hit0->isFlags(flag::FluorescenceHit) || hit1->isFlags(flag::FluorescenceHit)) {
        EvsSet("HXIEventSelection:2hit:CdTe-CdTe:fluorescence");
      }
      else {
        EvsSet("HXIEventSelection:2hit:CdTe-CdTe:not-fluorescence");
      }
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
