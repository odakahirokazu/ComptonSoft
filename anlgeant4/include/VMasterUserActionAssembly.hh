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

#ifndef ANLGEANT4_VMasterUserActionAssembly_H
#define ANLGEANT4_VMasterUserActionAssembly_H 1

#include "VUserActionAssembly.hh"
#include <list>

class G4UserStackingAction;

namespace anlgeant4
{

class VAppendableUserActionAssembly;

/**
 * Virtual master UserActionAssembly module
 * @author Hirokazu Odaka
 * @date 2017-06-29
 */
class VMasterUserActionAssembly : public VUserActionAssembly
{
  DEFINE_ANL_MODULE(VMasterUserActionAssembly, 5.0);
public:
  VMasterUserActionAssembly();
  virtual ~VMasterUserActionAssembly();

  void registerUserActions(G4RunManager* run_manager) override;

  bool hasStackingAction() const { return (stackingAction_!=nullptr); }
  void appendUserActions(VAppendableUserActionAssembly* user_action_assembly);

protected:
  void setStackingAction(G4UserStackingAction* v) { stackingAction_ = v; }
  void printSummary() override;

private:
  virtual void createUserActions() {}

private:
  G4UserStackingAction* stackingAction_ = nullptr;
  std::list<VAppendableUserActionAssembly*> userActionsAppended_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VUserActionAssembly_H */
