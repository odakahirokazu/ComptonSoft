/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
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

#include "VANLPrimaryGenerator.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "Geant4Body.hh"
#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

VANLPrimaryGenerator::VANLPrimaryGenerator()
{
  add_alias("VANLPrimaryGenerator");
}

ANLStatus VANLPrimaryGenerator::mod_initialize()
{
  Geant4Body* geant4body = nullptr;
  get_module_NC("Geant4Body", &geant4body);
  geant4body->register_user_action(this);

  return AS_OK;
}

} /* namespace anlgeant4 */
