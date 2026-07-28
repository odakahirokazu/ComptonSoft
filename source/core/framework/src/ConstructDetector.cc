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
  : detector_manager_(new DetectorSystem),
    configuration_file_("detector_config.xml"),
    parameters_file_(""),
    verbose_level_(0)
{
  add_alias("ConstructDetector");
}

ConstructDetector::~ConstructDetector() = default;

ANLStatus ConstructDetector::mod_define()
{
  detector_manager_->setMCSimulation(false);

  define_parameter("detector_configuration", &mod_class::configuration_file_);
  set_parameter_description("XML data file describing a detector configuration.");

  define_parameter("detector_parameters", &mod_class::parameters_file_);
  set_parameter_description("XML data file of information on detectors.");

  define_parameter("verbose_level", &mod_class::verbose_level_);

  return AS_OK;
}

ANLStatus ConstructDetector::mod_initialize()
{
  try {
    if (!detector_manager_->isConstructed()) {
      detector_manager_->readDetectorConfiguration(configuration_file_);
    }

    if (parameters_file_ != "") {
      detector_manager_->readDetectorParameters(parameters_file_);
    }
    else {
      if (detector_manager_->isMCSimulation()) {
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
    for (const auto& detector: detector_manager_->getDetectors()) {
      std::cout << "****************************************\n\n";
      std::cout << "Detector ID: " << detector->getID() << '\n';
      std::cout << "Name: " << detector->getName() << "\n\n";
      detector->printDetectorParameters(std::cout);
      std::cout << '\n';
      std::cout << "****************************************\n\n"
                << std::endl;
    }
    detector_manager_->printDetectorGroups();
    std::cout << "\n\n";
  }

  return AS_OK;
}

ANLStatus ConstructDetector::mod_analyze()
{
  detector_manager_->initializeEvent();
  return AS_OK;
}

} /* namespace comptonsoft */
