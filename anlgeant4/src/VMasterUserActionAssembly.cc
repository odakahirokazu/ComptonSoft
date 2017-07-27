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

#include "VMasterUserActionAssembly.hh"

#include "G4RunManager.hh"

#include "UserActionAssemblyRunAction.hh"
#include "UserActionAssemblyEventAction.hh"
#include "UserActionAssemblyTrackingAction.hh"
#include "UserActionAssemblySteppingAction.hh"
#include "VAppendableUserActionAssembly.hh"

using namespace anlnext;

namespace anlgeant4
{

VMasterUserActionAssembly::VMasterUserActionAssembly()
{
  add_alias("VMasterUserActionAssembly", ModuleAccess::ConflictOption::error);
}

VMasterUserActionAssembly::~VMasterUserActionAssembly() = default;

void VMasterUserActionAssembly::appendUserActions(VAppendableUserActionAssembly* user_action_assembly)
{
  userActionsAppended_.push_back(user_action_assembly);
}

void VMasterUserActionAssembly::registerUserActions(G4RunManager* run_manager)
{
  createUserActions();

  std::list<VUserActionAssembly*> userActions;
  userActions.push_back(this);
  for (VUserActionAssembly* pud: userActionsAppended_) {
    userActions.push_back(pud);
  }
  
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
  
  if (stackingAction_) {
    run_manager->SetUserAction(stackingAction_);
  }

  printSummary();
}

void VMasterUserActionAssembly::printSummary()
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
  if (this->hasStackingAction()) {
    std::cout << "(stacking action registered) ";
  }
  std::cout << "\n";

  for (VUserActionAssembly* uaa: userActionsAppended_) {
    std::cout << "Appended: " << uaa->module_id() << " ";
  }
  std::cout << std::endl;
}

} /* namespace anlgeant4 */
