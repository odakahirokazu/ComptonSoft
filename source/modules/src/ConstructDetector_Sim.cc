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

// ConstructDetector_Sim.cc
// 2008-08-27 Hirokazu Odaka

#include "ConstructDetector_Sim.hh"

#include <iostream>

#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"

#include "RealDetectorUnit.hh"
#include "VDeviceSimulation.hh"
#include "VHXISGDSensitiveDetector.hh"

using namespace comptonsoft;
using namespace anl;

ConstructDetector_Sim::ConstructDetector_Sim()
  : configuration_file_name("detector_config.xml"),
    simparam_file_name("simparam.xml"),
    print_parameters(false)
{
}


ANLStatus ConstructDetector_Sim::mod_startup()
{
  detector_manager = new DetectorManager;
  detector_manager->setMCSimulation(true);

  register_parameter(&configuration_file_name, "detector_configuration");
  register_parameter(&simparam_file_name, "simulation_parameters");

  return AS_OK;
}


ANLStatus ConstructDetector_Sim::mod_prepare()
{
  detector_manager->loadDetectorConfiguration(configuration_file_name.c_str());
  detector_manager->loadSimulationParameter(simparam_file_name.c_str());

  return AS_OK;
}


ANLStatus ConstructDetector_Sim::mod_init()
{
  if (detector_manager->IsConstructed()) {
    std::vector<VDeviceSimulation*>& dsvec
      = detector_manager->getDeviceSimulationVector();
    
    std::vector<VDeviceSimulation*>::iterator itDS;
    std::vector<VDeviceSimulation*>::iterator itDSEnd = dsvec.end();
   
    if (print_parameters) {
      std::cout << "\nSimulation parameters\n";
      for (itDS = dsvec.begin(); itDS != itDSEnd; itDS++) {
        std::cout << "ID " << (*itDS)->DetectorID() << '\n';
        (*itDS)->printSimParam();
        std::cout << std::endl;      
      }
    }
    return AS_OK;
  }
  else {
    return AS_QUIT;
  }
}


ANLStatus ConstructDetector_Sim::mod_bgnrun()
{
  if (!ModuleExist("Geant4Body")) return AS_OK;
    
  int nSD = detector_manager->NumSensitiveDetector();
  for (int i=0; i<nSD; ++i) {
    VHXISGDSensitiveDetector* sd = detector_manager->getSensitiveDetector(i);
    G4String name = sd->GetName();
    G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    G4LogicalVolume* lvol =
      G4LogicalVolumeStore::GetInstance()->GetVolume(name);
    
    if (lvol) {
      lvol->SetSensitiveDetector(sd);
    }
    else {
      std::cout << "Error: not found: Logical volume " << name << std::endl;
    }
  }

  return AS_OK;
}


ANLStatus ConstructDetector_Sim::mod_ana()
{
  for (DetectorIter it=detector_manager->DetectorBegin();
       it!=detector_manager->DetectorEnd();
       ++it) {
    (*it)->clearDetectorHits();
    (*it)->clearAnalyzedHits();
  }

  for (VDeviceSimulationIter it=detector_manager->DeviceSimulationBegin();
       it!=detector_manager->DeviceSimulationEnd();
       ++it) {
    (*it)->clearRawHits();
  }

  return AS_OK;
}


ANLStatus ConstructDetector_Sim::mod_exit()
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
