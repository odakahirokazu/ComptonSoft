/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Shin Watanabe, Hirokazu Odaka                      *
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

#include "VUserActionAssembly.hh"

#include "Geant4Body.hh"

#include "UserActionAssemblyRunAction.hh"
#include "UserActionAssemblyEventAction.hh"
#include "UserActionAssemblyTrackingAction.hh"
#include "UserActionAssemblySteppingAction.hh"

using namespace anlnext;

namespace anlgeant4
{

VUserActionAssembly::VUserActionAssembly()
{
}

VUserActionAssembly::~VUserActionAssembly() = default;

ANLStatus VUserActionAssembly::mod_initialize()
{
  Geant4Body* geant4body = nullptr;
  get_module_NC("Geant4Body", &geant4body);
  geant4body->register_user_action(this);

  return AS_OK;
}

std::unique_ptr<VUserActionAssembly> VUserActionAssembly::createUserActionAssembly() const
{
  std::unique_ptr<anlnext::BasicModule> m = this->clone();
  return std::unique_ptr<VUserActionAssembly>(static_cast<VUserActionAssembly*>(m.release()));
}

} /* namespace anlgeant4 */
