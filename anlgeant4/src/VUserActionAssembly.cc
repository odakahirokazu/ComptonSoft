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

#include "G4RunManager.hh"

#include "UserActionAssemblyRunAction.hh"
#include "UserActionAssemblyEventAction.hh"
#include "UserActionAssemblyTrackingAction.hh"
#include "UserActionAssemblySteppingAction.hh"
#include "ANLG4ActionInitialization.hh"

using namespace anlnext;

namespace anlgeant4
{

VUserActionAssembly::VUserActionAssembly()
{
  add_alias("VUserActionAssembly", ModuleAccess::ConflictOption::yield);
  add_alias("UserActionAssembly", ModuleAccess::ConflictOption::yield);
}

VUserActionAssembly::~VUserActionAssembly() = default;

void VUserActionAssembly::registerUserActions(G4RunManager* run_manager)
{
  std::list<VUserActionAssembly*> userActions;
  userActions.push_back(this);
  
  UserActionAssemblyRunAction* runAction = new UserActionAssemblyRunAction(userActions);
  UserActionAssemblyEventAction* eventAction = new UserActionAssemblyEventAction(userActions);
  UserActionAssemblyTrackingAction* trackingAction = new UserActionAssemblyTrackingAction(userActions);

  run_manager->SetUserAction(runAction);
  run_manager->SetUserAction(eventAction);
  run_manager->SetUserAction(trackingAction);

  if (isSteppingActionEnabled()) {
    UserActionAssemblySteppingAction* steppingAction = new UserActionAssemblySteppingAction(userActions);
    run_manager->SetUserAction(steppingAction);
  }
  
  printSummary();
}

void VUserActionAssembly::registerUserActions(ANLG4ActionInitialization* action_init, bool for_master)
{
  std::list<VUserActionAssembly*> userActions;
  userActions.push_back(this);
  
  UserActionAssemblyRunAction* runAction = new UserActionAssemblyRunAction(userActions);
  action_init->RegisterUserAction(runAction);

  if (!for_master) {
    UserActionAssemblyEventAction* eventAction = new UserActionAssemblyEventAction(userActions);
    UserActionAssemblyTrackingAction* trackingAction = new UserActionAssemblyTrackingAction(userActions);

    action_init->RegisterUserAction(eventAction);
    action_init->RegisterUserAction(trackingAction);

    if (isSteppingActionEnabled()) {
      UserActionAssemblySteppingAction* steppingAction = new UserActionAssemblySteppingAction(userActions);
      action_init->RegisterUserAction(steppingAction);
    }
  }
  
  printSummary();
}

void VUserActionAssembly::printSummary()
{
  std::cout << "\n";
  std::cout << "Registration of user-defined actions\n";
  if (isSteppingActionEnabled()) {
    std::cout << "  stepping actions are enabled.\n";
  }
  else {
    std::cout << "  stepping actions are disabled.\n";
  }
  std::cout << "Master  : " << this->module_id() << " ";
  std::cout << "\n";
  std::cout << std::endl;
}

} /* namespace anlgeant4 */