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

// ConstructDetector.cc
// 2008-08-27 Hirokazu Odaka

#include "ConstructDetector.hh"
#include <iostream>
#include "RealDetectorUnit.hh"

using namespace comptonsoft;
using namespace anl;


ConstructDetector::ConstructDetector()
  : detector_manager(0),
    configuration_file_name("detector_config.xml")
{
}


ANLStatus ConstructDetector::mod_startup()
{
  detector_manager = new DetectorManager;
  detector_manager->setMCSimulation(false);

  register_parameter(&configuration_file_name, "Detector configuration file");
  set_parameter_description("XML data file describing a detector configuration.");

  return AS_OK;
}


ANLStatus ConstructDetector::mod_prepare()
{
  detector_manager->loadDetectorConfiguration(configuration_file_name.c_str());

  return AS_OK;
}


ANLStatus ConstructDetector::mod_init()
{
  if (detector_manager->IsConstructed()) {
    return AS_OK;
  }
  else {
    return AS_QUIT;
  }
}


ANLStatus ConstructDetector::mod_ana()
{
  for (DetectorIter it=detector_manager->DetectorBegin();
       it!=detector_manager->DetectorEnd();
       ++it) {
    (*it)->clearDetectorHits();
    (*it)->clearAnalyzedHits();
  }
  return AS_OK;
}


ANLStatus ConstructDetector::mod_exit()
{
  std::cout << "deleting Detector Manager" << std::endl;
  std::cout << "detector number " << detector_manager->NumDetector() 
	    << std::endl;
  std::cout << "read module number " << detector_manager->NumReadModule()
	    << std::endl;

  delete detector_manager;

  std::cout << "delete Detector Manager done" << std::endl;

  return AS_OK;
}
