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

#ifndef ANLGEANT4_VUserActionAssembly_H
#define ANLGEANT4_VUserActionAssembly_H 1

#include <anl/BasicModule.hh>

class G4Event;
class G4Track;
class G4Step;
class G4Run;

class G4RunManager;
class G4UserStackingAction;

namespace anlgeant4
{

/**
 * Virtual UserActionAssembly module
 * @author Hirokazu Odaka
 * @date 2012-05-30 | Hirokazu Odaka | redesign (originally came from VPickUpData by Shin Watanabe)
 * @date 2017-06-28 | Hirokazu Odaka | redesign, rename class and methods
 */
class VUserActionAssembly : public anl::BasicModule
{
  DEFINE_ANL_MODULE(VUserActionAssembly, 5.0);
public:
  VUserActionAssembly();
  virtual ~VUserActionAssembly();

  virtual void RunActionAtBeginning(const G4Run*) {}
  virtual void RunActionAtEnd(const G4Run*) {}

  virtual void EventActionAtBeginning(const G4Event*) {}
  virtual void EventActionAtEnd(const G4Event*) {}

  virtual void TrackActionAtBeginning(const G4Track*) {}
  virtual void TrackActionAtEnd(const G4Track*) {}

  virtual void SteppingAction(const G4Step*) {}

  bool isSteppingActionEnabled() const { return steppingActionEnabled_; }

  virtual void registerUserActions(G4RunManager* run_manager);

protected:
  void enableSteppingAction() { steppingActionEnabled_ = true; }
  void disableSteppingAction() { steppingActionEnabled_ = false; }

  virtual void printSummary();

private:
  bool steppingActionEnabled_ = true;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VUserActionAssembly_H */
