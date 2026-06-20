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
#include <anlnext/BasicModule.hh>
#include "globals.hh"

namespace CLHEP
{
class HepRandomEngine;
}

class G4RunManager;


namespace anlgeant4
{

class ActionInitialization;
class VANLPrimaryGen;
class VUserActionAssembly;


/**
 * @author Hirokazu Odaka
 * @date 2017-07-28 | 3.0, re-designed.
 * @date 2026-04-15 | 4.0
 */
class Geant4Body : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(Geant4Body, 4.0);
public:
  Geant4Body();
  ~Geant4Body();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

  void register_user_action(VANLPrimaryGen* primary_gen);
  void register_user_action(VUserActionAssembly* uaa);

protected:
  virtual void initialize_random_generator();
  virtual void set_user_initializations();
  virtual void apply_commands();

private:
  std::unique_ptr<G4RunManager> run_manager_;
  ActionInitialization* action_initialization_;
  std::unique_ptr<CLHEP::HepRandomEngine> random_engine_ptr_;

  int num_events_ = 1;
  int num_threads_ = 0;
  bool print_beamon_time_ = false;
  std::string random_engine_;
  int random_seed_;
  int verbose_level_;
  std::vector<std::string> user_commands_;

  int random_seed_initial_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_Geant4Body_H */
