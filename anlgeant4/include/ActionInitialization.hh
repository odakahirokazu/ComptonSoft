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

#ifndef ANLGEANT4_ActionInitialization_H
#define ANLGEANT4_ActionInitialization_H 1

#include <list>
#include "G4VUserActionInitialization.hh"
#include "VANLPrimaryGen.hh"
#include "VUserActionAssembly.hh"

namespace anlgeant4
{

/**
 * UserActionInitialization
 * @author Hirokazu Odaka
 * @date 2026-04-15
 */
class ActionInitialization : public G4VUserActionInitialization
{
public:
  ActionInitialization();
  virtual ~ActionInitialization();

  void registerUserAction(VANLPrimaryGen* primary_gen);
  void registerUserAction(VUserActionAssembly* uaa);

  void BuildForMaster() const override;
  void Build() const override;

private:
  VANLPrimaryGen* primary_generation_module_ = nullptr;
  std::list<VUserActionAssembly*> user_action_assemblies_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_ActionInitialization_H */
