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

#ifndef COMPTONSOFT_VCSModule_H
#define COMPTONSOFT_VCSModule_H 1

#include "BasicModule.hh"

#include "DetectorManager.hh"
#include "RealDetectorUnit.hh"

#if USE_ROOT
class TDirectory;
#endif

namespace comptonsoft {

/**
 * class VCSModule
 * @author Hirokazu Odaka
 * @date 2008-08-30
 */
class VCSModule : public anl::BasicModule
{
  DEFINE_ANL_MODULE(VCSModule, 1.0);
public:
  VCSModule();
  ~VCSModule();
  
  virtual anl::ANLStatus mod_init();

#if USE_ROOT
  virtual anl::ANLStatus mod_his();

protected:
  void mkdir_module(const std::string& name="");
#endif
  
protected:
  comptonsoft::DetectorManager* GetDetectorManager() { return detector_manager; } 
  
  std::vector<comptonsoft::RealDetectorUnit*>& GetDetectorVector() 
  { return detector_manager->getDetectorVector(); }

private:
  comptonsoft::DetectorManager* detector_manager;
#if USE_ROOT
  TDirectory* save_dir;
#endif
};

}

#endif /* COMPTONSOFT_VCSModule_H */
