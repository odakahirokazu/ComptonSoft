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

#include "GenerateSimXEvent.hh"

#include "globals.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"
#include "SimXIF.hh"

using namespace anl;
using namespace comptonsoft;


GenerateSimXEvent::GenerateSimXEvent()
  : m_HitCollection(0),
    eventid(0), seqnum(0), totalhit(0),
    ini_energy(0.0),
    realposx(0.0), realposy(0.0), realposz(0.0),
    localposx(0.0), localposy(0.0), localposz(0.0),
    posx(0.0), posy(0.0), posz(0.0),
    edep(0.0), e_pha(0.0), e_pi(0.0),
    time1(0.0),
    process(0), grade(0),
    detid(-1), stripx(-1), stripy(-1),
    chip(-1), channel(-1),
    time_group(0), flag(0),
    m_SimXIF(0)
{
}


ANLStatus GenerateSimXEvent::mod_init()
{
  BasicModule::mod_init();

  GetANLModuleNC("CSHitCollection", &m_HitCollection);
  GetANLModuleNC("SimXIF", &m_SimXIF);
  EvsDef("GenerateSimXEvent:Fill");
  
  return AS_OK;
}


ANLStatus GenerateSimXEvent::mod_ana()
{
  const std::vector<DetectorHit_sptr>& hitVec 
    = m_HitCollection->GetHits();
  int num = hitVec.size();
  seqnum = 0;
  totalhit = num;

  flag = 0;

  if (num>0) EvsSet("GenerateSimXEvent:Fill");

  for (int i=0; i<num; i++) {
    flag = 0;
    
    realposx = hitVec[i]->getRealPosX();
    realposy = hitVec[i]->getRealPosY();
    realposz = hitVec[i]->getRealPosZ();
    localposx = hitVec[i]->getLocalPosX();
    localposy = hitVec[i]->getLocalPosY();
    localposz = hitVec[i]->getLocalPosZ();
    posx = hitVec[i]->getPosX();
    posy = hitVec[i]->getPosY();
    posz = hitVec[i]->getPosZ();

    edep  = hitVec[i]->getEdep();
    e_pha = hitVec[i]->getPHA();
    e_pi  = hitVec[i]->getPI();

    time1 = hitVec[i]->getTime();

    process = hitVec[i]->getProcess();
    grade = hitVec[i]->getGrade();

    detid = hitVec[i]->getDetectorID();
    stripx = hitVec[i]->getStripX();
    stripy = hitVec[i]->getStripY();
    chip = hitVec[i]->getChipID();
    channel = hitVec[i]->getChannel();

    time_group = hitVec[i]->getTimeGroup();
    
    flag |= hitVec[i]->getFlag();
     
    m_SimXIF->addEvent(time1, e_pi, stripx, stripy, detid);

    seqnum++;
  }

  eventid++;

  return AS_OK;
}


ANLStatus GenerateSimXEvent::mod_exit()
{
  if (m_SimXIF) m_SimXIF->outputEvents();
  
  return AS_OK;
}
