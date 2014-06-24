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

#ifndef ANLGEANT4_VPickUpData_H
#define ANLGEANT4_VPickUpData_H 1

#include "BasicModule.hh"

class G4Event;
class G4Track;
class G4Step;
class G4Run;

class G4RunManager;
class G4UserStackingAction;

namespace anlgeant4
{

/**
 * Virtual PickUpData module
 * @author *, Shin Watanabe, Hirokazu Odaka
 * @date 2012-05-30
 */
class VPickUpData : public anl::BasicModule
{
  DEFINE_ANL_MODULE(VPickUpData, 4.0);
public:
  VPickUpData();

  anl::ANLStatus mod_startup();
  
  virtual void RunAct_begin(const G4Run*) {}
  virtual void RunAct_end(const G4Run*)   {}
  virtual void EventAct_begin(const G4Event* ) {}
  virtual void EventAct_end(const G4Event* )   {}
  virtual void TrackAct_begin(const G4Track* ) {}
  virtual void TrackAct_end(const G4Track* )   {}
  virtual void StepAct(const G4Step*, G4Track*) {}

  bool StepActOn() { return m_StepActOn; }
  void SetStepActOn(bool on) { m_StepActOn = on; }

  void SetStackingAction(G4UserStackingAction* v) { m_StackingAction = v; }
  
  virtual void CreateUserActions();
  void RegisterUserActions(G4RunManager* run_manager);
  
private:
  bool m_StepActOn;
  G4UserStackingAction* m_StackingAction;
};

}

#endif /* ANLGEANT4_VPickUpData_H */
