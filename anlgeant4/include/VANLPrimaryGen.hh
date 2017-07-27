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

#ifndef ANLGEANT4_VANLPrimaryGen_H
#define ANLGEANT4_VANLPrimaryGen_H 1

#include <anlnext/BasicModule.hh>

class G4VUserPrimaryGeneratorAction;

namespace anlgeant4
{

/**
 * Virtual primary generator module
 * @author Shin Watanabe, Hirokazu Odaka
 * @date xxxx-xx-xx
 * @date 2011-xx-xx
 */
class VANLPrimaryGen : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(VANLPrimaryGen, 4.0);
public: 
  VANLPrimaryGen()
  {
    add_alias("VANLPrimaryGen");
  }
  
  virtual G4VUserPrimaryGeneratorAction* create() = 0;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VANLPrimaryGen_H */
