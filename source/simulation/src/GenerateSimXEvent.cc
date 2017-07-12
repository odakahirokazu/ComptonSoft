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

#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"
#include "SimXIF.hh"

using namespace anl;

namespace comptonsoft
{

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

ANLStatus GenerateSimXEvent::mod_initialize()
{
  BasicModule::mod_initialize();

  get_module_NC("CSHitCollection", &m_HitCollection);
  get_module_NC("SimXIF", &m_SimXIF);
  define_evs("GenerateSimXEvent:Fill");
  
  return AS_OK;
}

ANLStatus GenerateSimXEvent::mod_analyze()
{
  const std::vector<DetectorHit_sptr>& hitVec 
    = m_HitCollection->getHits();
  int num = hitVec.size();
  seqnum = 0;
  totalhit = num;

  flag = 0;

  if (num>0) set_evs("GenerateSimXEvent:Fill");

  for (int i=0; i<num; i++) {
    flag = 0;
    
    realposx = hitVec[i]->RealPositionX();
    realposy = hitVec[i]->RealPositionY();
    realposz = hitVec[i]->RealPositionZ();
    localposx = hitVec[i]->LocalPositionX();
    localposy = hitVec[i]->LocalPositionY();
    localposz = hitVec[i]->LocalPositionZ();
    posx = hitVec[i]->PositionX();
    posy = hitVec[i]->PositionY();
    posz = hitVec[i]->PositionZ();

    edep  = hitVec[i]->EnergyDeposit();
    e_pha = hitVec[i]->PHA();
    e_pi  = hitVec[i]->EPI();

    time1 = hitVec[i]->Time();

    process = hitVec[i]->Process();
    grade = hitVec[i]->Grade();

    detid = hitVec[i]->DetectorID();
    stripx = hitVec[i]->PixelX();
    stripy = hitVec[i]->PixelY();
    chip = hitVec[i]->ReadoutSection();
    channel = hitVec[i]->ReadoutChannel();

    time_group = hitVec[i]->TimeGroup();
    
    flag |= hitVec[i]->Flags();
     
    m_SimXIF->addEvent(time1, e_pi, stripx, stripy, detid);

    seqnum++;
  }

  eventid++;

  return AS_OK;
}

ANLStatus GenerateSimXEvent::mod_finalize()
{
  if (m_SimXIF) { m_SimXIF->outputEvents(); }
  return AS_OK;
}

} /* namespace comptonsoft */
