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

#include "VCSModule.hh"

#include "ConstructDetector.hh"
#include "ConstructDetector_Sim.hh"

#if USE_ROOT
#include "TDirectory.h"
#include "SaveData.hh"
#endif

using namespace anl;
using namespace comptonsoft;

VCSModule::VCSModule()
  : detector_manager(0)
#if USE_ROOT
  , save_dir(0)
#endif
{
}


VCSModule::~VCSModule()
{
}


ANLStatus VCSModule::mod_init()
{
  if (ModuleExist("ConstructDetector_Sim")) {
    ConstructDetector_Sim* detectorModule;
    GetANLModuleNC("ConstructDetector_Sim", &detectorModule);
    detector_manager = detectorModule->GetDetectorManager();
  }
  else if (ModuleExist("ConstructDetector")) {
    ConstructDetector* detectorModule;
    GetANLModuleNC("ConstructDetector", &detectorModule);
    detector_manager = detectorModule->GetDetectorManager();
  }
  
  return AS_OK;
}


#if USE_ROOT
ANLStatus VCSModule::mod_his()
{
  if (ModuleExist("SaveData")) {
    SaveData* saveModule;
    GetANLModuleNC("SaveData", &saveModule);
    save_dir = saveModule->GetDirectory();
    save_dir->cd();
  }
  
  return AS_OK;
}


void VCSModule::mkdir_module(const std::string& name)
{
  if (name=="") {
    save_dir->mkdir(module_name().c_str());
    save_dir->cd(module_name().c_str());
  }
  else {
    save_dir->mkdir(name.c_str());
    save_dir->cd(name.c_str());
  }
}
#endif
