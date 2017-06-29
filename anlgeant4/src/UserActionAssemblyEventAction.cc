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

#include "UserActionAssemblyEventAction.hh"
#include "VUserActionAssembly.hh"

namespace anlgeant4
{

UserActionAssemblyEventAction::UserActionAssemblyEventAction(const std::list<VUserActionAssembly*>& userActions)
  : userActions_(userActions)
{
}
 
UserActionAssemblyEventAction::~UserActionAssemblyEventAction() = default;

void UserActionAssemblyEventAction::BeginOfEventAction(const G4Event* anEvent)
{
  for (VUserActionAssembly* pud: userActions_) {
    pud->EventActionAtBeginning(anEvent);
  }
}

void UserActionAssemblyEventAction::EndOfEventAction(const G4Event* anEvent)
{
  for (VUserActionAssembly* pud: userActions_) {
    pud->EventActionAtEnd(anEvent);
  }
}

} /* namespace anlgeant4 */
