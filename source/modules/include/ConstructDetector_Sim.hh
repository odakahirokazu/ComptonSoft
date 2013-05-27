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

// ConstructDetector_Sim.hh
// 2008-08-27 Hirokazu Odaka

#ifndef COMPTONSOFT_ConstructDetector_Sim_H
#define COMPTONSOFT_ConstructDetector_Sim_H 1

#include "BasicModule.hh"
#include "DetectorManager.hh"

namespace comptonsoft {

class ConstructDetector_Sim : public anl::BasicModule
{
  DEFINE_ANL_MODULE(ConstructDetector_Sim, 2.0);
public:
  ConstructDetector_Sim();
  ~ConstructDetector_Sim() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_prepare();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_bgnrun();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_exit();

  comptonsoft::DetectorManager* GetDetectorManager() { return detector_manager; }

protected:
  bool sim_radioactive_decay;

private:
  comptonsoft::DetectorManager* detector_manager;

  std::string configuration_file_name;
  std::string simparam_file_name;
  
  bool print_parameters;
};

}

#endif /* COMPTONSOFT_ConstructDetector_Sim_H */
