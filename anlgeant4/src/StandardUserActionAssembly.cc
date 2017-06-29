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

#include "StandardUserActionAssembly.hh"
#include "G4Event.hh"
#include "InitialInformation.hh"

using namespace anl;

namespace anlgeant4
{

StandardUserActionAssembly::StandardUserActionAssembly()
  : m_InitialInfo(0)
{
  add_alias("StandardUserActionAssembly");
}

ANLStatus StandardUserActionAssembly::mod_init()
{
  GetANLModuleIFNC("InitialInformation", &m_InitialInfo);
  return AS_OK;
}

void StandardUserActionAssembly::EventActionAtBeginning(const G4Event* anEvent)
{
  m_InitialInfo->setEventID(anEvent->GetEventID());
}

void StandardUserActionAssembly::setInitialTime(double v)
{
  m_InitialInfo->setInitialTime(v);
}

double StandardUserActionAssembly::getInitialTime() const
{
  return m_InitialInfo->InitialTime();
}

} /* namespace anlgeant4 */
