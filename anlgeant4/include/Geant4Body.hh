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
#include <memory>
#include <anl/BasicModule.hh>
#include "globals.hh"

namespace CLHEP
{
class HepRandomEngine;
}

namespace anlgeant4
{

class ANLG4RunManager;

/**
 * @author Hirokazu Odaka
 * @date 2017-07-28 | 3.0, re-designed.
 */
class Geant4Body : public anl::BasicModule
{
  DEFINE_ANL_MODULE(Geant4Body, 3.0);
public: 
  Geant4Body();
  ~Geant4Body();

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_begin_run() override;
  anl::ANLStatus mod_analyze() override;
  anl::ANLStatus mod_end_run() override;
  anl::ANLStatus mod_finalize() override;

  void set_verbose_level(G4int v) { m_VerboseLevel = v; }
  G4int get_verbose_level() { return m_VerboseLevel; }
  
protected:
  virtual void initialize_random_generator();
  virtual void set_user_initializations();
  virtual void set_user_primary_generator_action();
  virtual void set_user_defined_actions();
  virtual void apply_commands();

private:
  std::unique_ptr<ANLG4RunManager> m_G4RunManager;
  std::unique_ptr<CLHEP::HepRandomEngine> m_RandomEnginePtr;
  int m_EventIndex = 0;
  
  std::string m_RandomEngine;
  int m_RandomInitMode;
  int m_RandomSeed1;
  bool m_OutputRandomStatus;
  std::string m_RandomInitialStatusFileName;
  std::string m_RandomFinalStatusFileName;

  int m_VerboseLevel;
  std::vector<std::string> m_UserCommands;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_Geant4Body_H */
