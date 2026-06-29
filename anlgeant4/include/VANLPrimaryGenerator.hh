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

#ifndef ANLGEANT4_VANLPrimaryGenerator_H
#define ANLGEANT4_VANLPrimaryGenerator_H 1

#include <anlnext/BasicModule.hh>

class G4VUserPrimaryGeneratorAction;

namespace anlgeant4
{

/**
 * Virtual primary generator module
 * @author Shin Watanabe, Hirokazu Odaka
 * @date xxxx-xx-xx
 * @date 2011-xx-xx
 * @date 2026-04-15 | 5.0 | register this action to Geant4Body
 * @date 2026-06-20 | 6.0 | Geant4 MT
 */
class VANLPrimaryGenerator : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(VANLPrimaryGenerator, 7.0);
public:
  VANLPrimaryGenerator();

  virtual G4VUserPrimaryGeneratorAction* create() = 0;

  anlnext::ANLStatus mod_initialize() override;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VANLPrimaryGenerator_H */
