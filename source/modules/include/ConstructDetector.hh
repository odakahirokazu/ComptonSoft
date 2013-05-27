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

// ConstructDetector.hh
// 2008-08-27 Hirokazu Odaka

#ifndef COMPTONSOFT_ConstructDetector_H
#define COMPTONSOFT_ConstructDetector_H 1

#include "BasicModule.hh"
#include "DetectorManager.hh"

namespace comptonsoft {

class ConstructDetector : public anl::BasicModule
{
  DEFINE_ANL_MODULE(ConstructDetector, 2.0);
public:
  ConstructDetector();
  ~ConstructDetector() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_prepare();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_exit();
  
  comptonsoft::DetectorManager* GetDetectorManager() { return detector_manager; }

private:
  comptonsoft::DetectorManager* detector_manager;
  std::string configuration_file_name;
};

}

#endif /* COMPTONSOFT_ConstructDetector_H */
