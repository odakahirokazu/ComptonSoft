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

#ifndef ANLGEANT4_VANLPhysicsList_H
#define ANLGEANT4_VANLPhysicsList_H

#include "BasicModule.hh"

class G4VUserPhysicsList;

namespace anlgeant4
{

/**
 * Virtual physics list module
 * @author Shin Watanabe, Hirokazu Odaka
 * @date xxxx-xx-xx
 * @date 2012-05-30
 */
class VANLPhysicsList : public anl::BasicModule
{
  DEFINE_ANL_MODULE(VANLPhysicsList, 4.0);
public:
  VANLPhysicsList()
  {
    add_alias("VANLPhysicsList");
  }
  
  virtual G4VUserPhysicsList* create() = 0;
};

}

#endif /* ANLGEANT4_VANLPhysicsList_H */
