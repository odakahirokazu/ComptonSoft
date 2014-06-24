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

#include "VPickUpData.hh"

#include "G4RunManager.hh"

#include "PickUpDataRunAction.hh"
#include "PickUpDataEventAction.hh"
#include "PickUpDataTrackingAction.hh"
#include "PickUpDataSteppingAction.hh"

using namespace anl;
using namespace anlgeant4;

VPickUpData::VPickUpData()
  : m_StepActOn(true), m_StackingAction(0)
{
  add_alias("VPickUpData");
  add_alias("PickUpData");
}


ANLStatus VPickUpData::mod_startup()
{
  register_parameter(&m_StepActOn, "stepping_action_enable");
  return AS_OK;
}


void VPickUpData::CreateUserActions()
{
}


void VPickUpData::RegisterUserActions(G4RunManager* run_manager)
{
  CreateUserActions();

  PickUpDataRunAction* runAction = new PickUpDataRunAction(this);
  PickUpDataEventAction* eventAction = new PickUpDataEventAction(this);
  PickUpDataTrackingAction* trackingAction = new PickUpDataTrackingAction(this);

  run_manager->SetUserAction(runAction);
  run_manager->SetUserAction(eventAction);
  run_manager->SetUserAction(trackingAction);

  if (StepActOn()) {
    PickUpDataSteppingAction* steppingAction = new PickUpDataSteppingAction(this);
    run_manager->SetUserAction(steppingAction);
  }
  
  if (m_StackingAction) run_manager->SetUserAction(m_StackingAction);
}
