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

#include <anlnext/BasicModule.hh>
#include <memory>


class G4Run;
class G4Event;
class G4Track;
class G4Step;
class G4UserStackingAction;


namespace anlgeant4
{

/**
 * Virtual UserActionAssembly module
 * @author Hirokazu Odaka
 * @date 2012-05-30 | Hirokazu Odaka | redesign (originally came from VPickUpData by Shin Watanabe)
 * @date 2017-06-28 | Hirokazu Odaka | redesign, rename class and methods
 * @date 2026-04-15 | Hirokazu Odaka | redesign
 */
class VUserActionAssembly : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(VUserActionAssembly, 6.0);
  ENABLE_PARALLEL_RUN();
public:
  VUserActionAssembly();
  virtual ~VUserActionAssembly();

  anlnext::ANLStatus mod_initialize() override;

  virtual void run_action_at_beginning(const G4Run*) {}
  virtual void run_action_at_end(const G4Run*) {}

  virtual void event_action_at_beginning(const G4Event*) {}
  virtual void event_action_at_end(const G4Event*) {}

  virtual void track_action_at_beginning(const G4Track*) {}
  virtual void track_action_at_end(const G4Track*) {}

  virtual bool is_stepping_action_effective() const { return false; }
  virtual void stepping_action(const G4Step*) {}

  virtual bool is_stacking_action_effective() const { return false; }
  virtual G4UserStackingAction* create_stacking_action() const { return nullptr; }

  virtual std::unique_ptr<VUserActionAssembly> create_user_action_assembly() const;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VUserActionAssembly_H */
