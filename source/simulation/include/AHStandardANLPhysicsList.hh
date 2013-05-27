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

#ifndef COMPTONSOFT_AHStandardANLPhysicsList_H
#define COMPTONSOFT_AHStandardANLPhysicsList_H 1

#include <string>
#include "VANLPhysicsList.hh"

namespace comptonsoft
{


/**
 * ANL Geant4 physics list module for X-ray and soft gamma-ray simulations.
 * This is a standard physics list module for ASTRO-H.
 *
 * @author Hirokazu Odaka
 * @date 2011-xx-xx
 * @date 2012-05-30
 */
class AHStandardANLPhysicsList : public anlgeant4::VANLPhysicsList
{
  DEFINE_ANL_MODULE(AHStandardANLPhysicsList, 1.1);
public:
  AHStandardANLPhysicsList();
  
  anl::ANLStatus mod_startup(); 

  G4VUserPhysicsList* create();

private:
  std::string m_PhysicsOption;
  double m_DefaultCut;
};

}

#endif /* COMPTONSOFT_AHStandardANLPhysicsList_H */
