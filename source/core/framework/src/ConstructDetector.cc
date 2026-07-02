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

#include "ConstructDetector.hh"
#include <iostream>
#include "VRealDetectorUnit.hh"
#include "DetectorSystem.hh"

using namespace anlnext;

namespace comptonsoft {

ConstructDetector::ConstructDetector()
  : detectorManager_(new DetectorSystem),
    configurationFile_("detector_config.xml"),
    parametersFile_(""),
    verboseLevel_(0)
{
  add_alias("ConstructDetector");
}

ConstructDetector::~ConstructDetector() = default;

ANLStatus ConstructDetector::mod_define()
{
  detectorManager_->setMCSimulation(false);

  register_parameter(&configurationFile_, "detector_configuration");
  set_parameter_description("XML data file describing a detector configuration.");

  register_parameter(&parametersFile_, "detector_parameters");
  set_parameter_description("XML data file of information on detectors.");

  register_parameter(&verboseLevel_, "verbose_level");

  return AS_OK;
}

ANLStatus ConstructDetector::mod_initialize()
{
  try {
    if (!detectorManager_->isConstructed()) {
      detectorManager_->readDetectorConfiguration(configurationFile_);
    }

    if (parametersFile_ != "") {
      detectorManager_->readDetectorParameters(parametersFile_);
    }
    else {
      if (detectorManager_->isMCSimulation()) {
        std::cout << "Error: detector parameters file should be given." << std::endl;
        return AS_QUIT;
      }
    }
  }
  catch (CSException& e) {
    std::cout << "\n\n";
    std::cout << "Error in ConstructDetector::mod_initialize()\n"
              << "Loading the XML data files failed." << std::endl;
    std::cout << "CSException ===> \n"
              << e.toString()
              << std::endl;
    return AS_QUIT;
  }
  
  if (VerboseLevel() > 0) {
    std::cout << "\n\n";
    std::cout << "######  Detector parameters  ######\n\n";
    for (const auto& detector: detectorManager_->getDetectors()) {
      std::cout << "****************************************\n\n";
      std::cout << "Detector ID: " << detector->getID() << '\n';
      std::cout << "Name: " << detector->getName() << "\n\n";
      detector->printDetectorParameters(std::cout);
      std::cout << '\n';
      std::cout << "****************************************\n\n"
                << std::endl;
    }
    detectorManager_->printDetectorGroups();
    std::cout << "\n\n";
  }
  
  return AS_OK;
}

ANLStatus ConstructDetector::mod_analyze()
{
  detectorManager_->initializeEvent();
  return AS_OK;
}

} /* namespace comptonsoft */
