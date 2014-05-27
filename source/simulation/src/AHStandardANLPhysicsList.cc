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

#include "AHStandardANLPhysicsList.hh"
#include "G4SystemOfUnits.hh"
#include "AHG4PhysicsList.hh"

using namespace anl;
using namespace comptonsoft;


AHStandardANLPhysicsList::AHStandardANLPhysicsList()
  : m_PhysicsOption(""),
    m_DefaultCut(0.0001*cm)
{
  add_alias(module_name());
}


ANLStatus AHStandardANLPhysicsList::mod_startup()
{
  anlgeant4::VANLPhysicsList::mod_startup();
  register_parameter(&m_PhysicsOption, "Physics option");
  register_parameter(&m_DefaultCut, "Cut value", cm, "cm");
  
  return AS_OK;
}


G4VUserPhysicsList* AHStandardANLPhysicsList::create()
{
  G4VUserPhysicsList* physList = new AHG4PhysicsList(m_PhysicsOption);
  physList->SetDefaultCutValue(m_DefaultCut);
  return physList;
}
