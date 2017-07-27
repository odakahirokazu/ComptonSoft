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

#include "ConstructDetectorForSimulation.hh"

#include <iostream>

#include "VRealDetectorUnit.hh"
#include "DeviceSimulation.hh"
#include "DetectorSystem.hh"

using namespace anlnext;

namespace comptonsoft
{

ConstructDetectorForSimulation::ConstructDetectorForSimulation()
{
}

ConstructDetectorForSimulation::~ConstructDetectorForSimulation() = default;

ANLStatus ConstructDetectorForSimulation::mod_define()
{
  ConstructDetector::mod_define();

  DetectorSystem* detectorManager = getDetectorManager();
  detectorManager->setMCSimulation(true);

  return AS_OK;
}

ANLStatus ConstructDetectorForSimulation::mod_initialize()
{
  ANLStatus status = ConstructDetector::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  DetectorSystem* detectorManager = getDetectorManager();
  
  if (VerboseLevel() > 0) {
    std::cout << "\n\n";
    std::cout << "######  Simulation parameters  ######\n\n";
    for (const auto& ds: detectorManager->getDeviceSimulationVector()) {
      std::cout << "****************************************\n\n";
      std::cout << "Detector ID: " << ds->getID() << '\n';
      std::cout << "Name: " << ds->getName() << "\n\n";
      ds->printSimulationParameters(std::cout);
      std::cout << '\n';
      std::cout << "****************************************\n\n"
                << std::endl;
    }
  }

  return AS_OK;
}

ANLStatus ConstructDetectorForSimulation::mod_begin_run()
{
  if (!exist_module("Geant4Body")) {
    return AS_OK;
  }

  DetectorSystem* detectorManager = getDetectorManager();
  detectorManager->registerGeant4SensitiveDetectors();
  return AS_OK;
}

} /* namespace comptonsoft */
