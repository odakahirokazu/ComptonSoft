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

#include "StandardPickUpData.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4VProcess.hh"
#include "InitialInformation.hh"

using namespace anl;
using namespace anlgeant4;

StandardPickUpData::StandardPickUpData()
  : m_StartTime(0.0), m_InitialInfo(0)
{
  add_alias("StandardPickUpData");
  SetStepActOn(false);
}


ANLStatus StandardPickUpData::mod_init()
{
  GetANLModuleIFNC("InitialInformation", &m_InitialInfo);
  return AS_OK;
}


void StandardPickUpData::EventAct_begin(const G4Event* aEvent)
{
  m_InitialInfo->setEventID(aEvent->GetEventID());
  SetStartTime(0.0);
}
