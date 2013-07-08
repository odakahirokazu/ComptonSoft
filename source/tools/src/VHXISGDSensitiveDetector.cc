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

// 2011-04-04 Hiro Odaka

#include "VHXISGDSensitiveDetector.hh"

#include "G4VProcess.hh"

#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include "HXISGDFlagDef.hh"
#include "DetectorManager.hh"
#include "BasicDeviceSimulation.hh"


VHXISGDSensitiveDetector::VHXISGDSensitiveDetector(G4String name)
  : G4VSensitiveDetector(name),
    detector_manager(0), position_calculation(false), sd_check(false)
{ 
}


G4bool
VHXISGDSensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory* )
{
  const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
  const G4int detid = GetDetectorID(touchable);
  if (detid == -1) {
    if (sd_check) {
      std::cout << "Detector unregistered: " << MakeKey(touchable) << std::endl;
    }
    return true;
  }

  G4Track* aTrack = aStep->GetTrack();
  G4double edep = aStep->GetTotalEnergyDeposit();

  G4int process = 0;
  const G4VProcess* proc = aStep->GetPostStepPoint()->GetProcessDefinedStep();
  if (proc) {
    G4String processName = proc->GetProcessName();
    if(processName.find("phot") != std::string::npos) {
      process = comptonsoft::PROC_PHOTOABS;
    }
    else if(processName.find("compt") != std::string::npos) {
      process = comptonsoft::PROC_COMPTON;
    }
    else if(processName.find("Rayl") != std::string::npos) {
      process = comptonsoft::PROC_RAYLEIGH;
    }
    else if(processName.find("conv") != std::string::npos) {
      process = comptonsoft::PROC_GAMMACONVERSION;
    }
    else if(processName.find("eBrem") != std::string::npos) {
      process = comptonsoft::PROC_BREMS;
    }

    if (processName.find("Coupled") == std::string::npos
        && processName.find("Rayl") == std::string::npos
        && edep == 0.0) {
      process |= comptonsoft::PROC_0ENE;
    }
  }

  if (aTrack->GetDefinition()->GetParticleType() == "nucleus") {
    process |= comptonsoft::QUENCHING;
  }
    
  // e- coupled transportation (ignoring e.g. edep~e-26)
  if (edep < 1.0e-15 && process == 0) return true;
  //  if (edep == 0.0 && process == 0) return true;
  
  comptonsoft::DetectorHit_sptr hit(new comptonsoft::DetectorHit);
  hit->setDetectorID(detid);
  hit->setEdep(edep);
  hit->setProcess(process);

  G4ThreeVector position;
  switch (aStep->GetPostStepPoint()->GetStepStatus()) 
    {
    case fPostStepDoItProc:
      position = aStep->GetPostStepPoint()->GetPosition();
      break;
    case fAlongStepDoItProc:
      position = 0.5*(aStep->GetPreStepPoint()->GetPosition() + 
                      aStep->GetPostStepPoint()->GetPosition());
      break;
    default:
      position = aStep->GetPreStepPoint()->GetPosition();
      break;
    }

  hit->setRealPosX(position.x());
  hit->setRealPosY(position.y());
  hit->setRealPosZ(position.z());
  
  //  hit->setTime(aTrack->GetGlobalTime() - starttime);
  hit->setTime(aTrack->GetGlobalTime());
  hit->setEnergy(aTrack->GetKineticEnergy());

  G4VPhysicalVolume* physivol = 0;
     
  for (G4int i = touchable->GetHistoryDepth()-1; i >= 0; i--) {
    physivol = touchable->GetVolume(i);
    position += physivol->GetFrameTranslation();
    if (physivol->GetFrameRotation() != 0) {
      position = (*physivol->GetFrameRotation()) * position;
    }
  }
  
  hit->setLocalPosX(position.x());
  hit->setLocalPosY(position.y());
  hit->setLocalPosZ(position.z());

  detector_manager->getDeviceSimulationByID(detid)->insertRawHit(hit);

  if (position_calculation) {
    std::set<int>::iterator it = position_calculation_set.find(detid);
    
    if (it!=position_calculation_set.end()) {
      G4ThreeVector center(0, 0, 0);
      G4ThreeVector dirx(1, 0, 0);
      G4ThreeVector diry(0, 1, 0);
      
      for (G4int i = 0; i < touchable->GetHistoryDepth(); i++) {
        physivol = touchable->GetVolume(i);
        if (physivol->GetFrameRotation() != 0) {
          center = (*physivol->GetObjectRotation()) * center;
          dirx   = (*physivol->GetObjectRotation()) * dirx;
          diry   = (*physivol->GetObjectRotation()) * diry;
        }
        center += physivol->GetObjectTranslation();
      }
      
      comptonsoft::RealDetectorUnit* det = detector_manager->getDetectorByID(detid);
      det->setCenterPosition(center.x(), center.y(), center.z());
      det->setDirectionX(dirx.x(), dirx.y(), dirx.z());
      det->setDirectionY(diry.x(), diry.y(), diry.z());

      position_calculation_set.erase(it);
    }

    if (position_calculation_set.empty()) {
      position_calculation = false;
    }
  }

  return true;
}


G4String VHXISGDSensitiveDetector::MakeKey(const G4VTouchable* touchable)
{
 G4String key;
 for(G4int i=touchable->GetHistoryDepth()-1; i>=0; --i) {
   G4String volumeName = touchable->GetVolume(i)->GetName();
   key += volumeName + (i==0 ? "" : "/");
 }
 return key;
}
