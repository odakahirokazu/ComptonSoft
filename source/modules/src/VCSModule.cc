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
#include "TDirectory.h"
#include "SaveData.hh"

using namespace anlnext;

namespace comptonsoft
{

VCSModule::VCSModule()
  : detectorSystem_(nullptr), saveDir_(nullptr)
{
}

VCSModule::~VCSModule() = default;

ANLStatus VCSModule::mod_initialize()
{
  if (exist_module("ConstructDetector")) {
    ConstructDetector* detectorModule;
    get_module_NC("ConstructDetector", &detectorModule);
    detectorSystem_ = detectorModule->getDetectorManager();
  }
  
  if (exist_module("SaveData")) {
    SaveData* saveModule;
    get_module_NC("SaveData", &saveModule);
    saveDir_ = saveModule->GetDirectory();
    saveDir_->cd();
  }
  
  return AS_OK;
}

void VCSModule::mkdir(const std::string& name)
{
  if (name=="") {
    saveDir_->mkdir(module_id().c_str());
    saveDir_->cd(module_id().c_str());
  }
  else {
    saveDir_->mkdir(name.c_str());
    saveDir_->cd(name.c_str());
  }
}

} /* namespace comptonsoft */
