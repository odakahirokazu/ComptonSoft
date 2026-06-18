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

#include "ActionInitialization.hh"

#include <numeric>
#include <algorithm>

#include "G4Threading.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Exception.hh"
#include "UserActionAssemblyRunAction.hh"
#include "UserActionAssemblyEventAction.hh"
#include "UserActionAssemblyTrackingAction.hh"
#include "UserActionAssemblySteppingAction.hh"

using namespace anlnext;

namespace anlgeant4
{

ActionInitialization::ActionInitialization() = default;

ActionInitialization::~ActionInitialization() = default;

void ActionInitialization::registerUserAction(VANLPrimaryGen* primary_gen)
{
  primary_generation_module_ = primary_gen;
}

void ActionInitialization::registerUserAction(VUserActionAssembly* uaa)
{
  user_action_assemblies_original_.push_back(uaa);
}

std::vector<VUserActionAssembly*> ActionInitialization::create_user_action_assemblies() const
{
  std::vector<VUserActionAssembly*> uaa_vector;
  std::vector<std::unique_ptr<VUserActionAssembly>> uaa_vector_owner;
  for (const VUserActionAssembly* uaa: user_action_assemblies_original_) {
    std::unique_ptr<VUserActionAssembly> uaa_created = uaa->createUserActionAssembly();
    uaa_vector.push_back(uaa_created.get());
    uaa_vector_owner.push_back(std::move(uaa_created));
  }

  std::lock_guard<std::mutex> lock(mutex_);
  user_action_assemblies_vector_.push_back(std::move(uaa_vector_owner));

  return uaa_vector;
}

void ActionInitialization::BuildForMaster() const
{
  std::vector<VUserActionAssembly*> user_action_assemblies = create_user_action_assemblies();
  UserActionAssemblyRunAction* runAction = new UserActionAssemblyRunAction(user_action_assemblies);
  SetUserAction(runAction);
}

void ActionInitialization::Build() const
{
  G4VUserPrimaryGeneratorAction* primary_generator = primary_generation_module_->create();
  SetUserAction(primary_generator);

  std::vector<VUserActionAssembly*> user_action_assemblies = create_user_action_assemblies();

  if (!G4Threading::IsMultithreadedApplication()) {
    UserActionAssemblyRunAction* runAction = new UserActionAssemblyRunAction(user_action_assemblies);
    SetUserAction(runAction);
  }

  UserActionAssemblyEventAction* eventAction = new UserActionAssemblyEventAction(user_action_assemblies);
  UserActionAssemblyTrackingAction* trackingAction = new UserActionAssemblyTrackingAction(user_action_assemblies);
  SetUserAction(eventAction);
  SetUserAction(trackingAction);

  const bool stepping_action_effective =
    std::accumulate(user_action_assemblies.begin(),
                    user_action_assemblies.end(),
                    false,
                    [](bool acc, VUserActionAssembly* uaa){ return acc || uaa->isSteppingActionEffective(); });
  if (stepping_action_effective) {
    UserActionAssemblySteppingAction* steppingAction = new UserActionAssemblySteppingAction(user_action_assemblies);
    SetUserAction(steppingAction);
  }

  const int stacking_action_number =
    std::count_if(user_action_assemblies_original_.begin(),
                  user_action_assemblies_original_.end(),
                  [](VUserActionAssembly* uaa){ return uaa->isStackingActionEffective(); });
  if (stacking_action_number==0) {
    ; // do nothing
  }
  else if (stacking_action_number==1) {
    VUserActionAssembly* uaa_stacking_action  =
      *std::find_if(user_action_assemblies_original_.begin(),
                    user_action_assemblies_original_.end(),
                    [](VUserActionAssembly* uaa){ return uaa->isStackingActionEffective(); });
    SetUserAction(uaa_stacking_action->createStackingAction());
  }
  else {
    const std::string msg = "There are multiple stacking actions to be registered. Only one stacking action can be set.";
    G4Exception("ActionInitialization::Build()",
                "CS-ACT-001",
                FatalException,
                msg.c_str());
  }
}

} /* namespace anlgeant4 */
