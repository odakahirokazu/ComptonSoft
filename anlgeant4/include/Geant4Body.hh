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

#ifndef ANLGEANT4_Geant4Body_H
#define ANLGEANT4_Geant4Body_H 1

#include <string>
#include "BasicModule.hh"
#include "globals.hh"

namespace CLHEP
{
class HepRandomEngine;
}

namespace anlgeant4
{

class ANLG4RunManager;


class Geant4Body : public anl::BasicModule
{
  DEFINE_ANL_MODULE(Geant4Body, 2.1);
public: 
  Geant4Body();
  ~Geant4Body();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_com();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_bgnrun(); 
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_endrun(); 
  anl::ANLStatus mod_exit();

  void set_verbose_level(G4int v) { m_VerboseLevel = v; }
  G4int get_verbose_level() { return m_VerboseLevel; }
  
//---------------------------------------

protected:
  virtual void set_user_initializations();
  virtual void set_user_primarygen_action();
  virtual void set_user_std_actions();
  virtual void apply_commands();

private:
  ANLG4RunManager* m_G4RunManager;
  CLHEP::HepRandomEngine* m_RandomEnginePtr;
  
  std::string m_RandomEngine;
  int m_RandomInitMode;
  int m_RandomSeed1;
  std::string m_RandomInitialStatusFileName;
  std::string m_RandomFinalStatusFileName;

  int m_VerboseLevel;
};

}

#endif /* ANLGEANT4_Geant4Body_H */
