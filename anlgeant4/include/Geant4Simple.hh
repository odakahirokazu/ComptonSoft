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

#ifndef ANLGEANT4_Geant4Simple_H
#define ANLGEANT4_Geant4Simple_H 1

#include <string>
#include "BasicModule.hh"
#include "globals.hh"

namespace CLHEP
{
class HepRandomEngine;
}

class G4RunManager;

namespace anlgeant4
{

/**
 * Simple Geant4 run manager 
 * @author Hirokazu Odaka
 */
class Geant4Simple : public anl::BasicModule
{
  DEFINE_ANL_MODULE(Geant4Simple, 1.1);
public: 
  Geant4Simple();
  ~Geant4Simple();
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_exit();

protected:
  virtual void set_user_initializations();
  virtual void set_user_primarygen_action();
  virtual void set_user_std_actions();
  virtual void apply_commands();

private:
  G4RunManager* m_G4RunManager;
  CLHEP::HepRandomEngine* m_RandomEnginePtr;

  std::string m_RandomEngine;
  int m_RandomSeed1;
  
  int m_NumberOfTrial;
  int m_VerboseLevel;
};

}

#endif /* ANLGEANT4_Geant4Simple_H */
