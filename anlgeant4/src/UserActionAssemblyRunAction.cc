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

#include "UserActionAssemblyRunAction.hh"
#include "VUserActionAssembly.hh"

namespace anlgeant4
{

UserActionAssemblyRunAction::UserActionAssemblyRunAction(const std::vector<VUserActionAssembly*>& user_actions)
  : user_actions_(user_actions)
{
}

UserActionAssemblyRunAction::~UserActionAssemblyRunAction() = default;

void UserActionAssemblyRunAction::BeginOfRunAction(const G4Run* run)
{
  for (VUserActionAssembly* ua: user_actions_) {
    ua->run_action_at_beginning(run);
  }
}

void UserActionAssemblyRunAction::EndOfRunAction(const G4Run* run)
{
  for (VUserActionAssembly* ua: user_actions_) {
    ua->run_action_at_end(run);
  }
}

} /* namespace anlgeant4 */
