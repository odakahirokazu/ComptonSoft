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

#include "VCSSensitiveDetector.hh"

#include "G4VProcess.hh"

#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include "FlagDefinition.hh"
#include "DetectorSystem.hh"
#include "MultiChannelData.hh"
#include "VRealDetectorUnit.hh"
#include "DeviceSimulation.hh"

namespace comptonsoft {

VCSSensitiveDetector::VCSSensitiveDetector(G4String name)
  : G4VSensitiveDetector(name),
    positionCalculation_(false), SDCheck_(false), layerOffset_(0),
    detectorSystem_(nullptr)
{ 
}

VCSSensitiveDetector::~VCSSensitiveDetector() = default;

G4bool
VCSSensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory* )
{
  const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
  const G4int DetectorID = GetDetectorID(touchable);
  if (DetectorID == -1) {
    if (SDCheck_) {
      std::cout << "Detector unregistered: " << HierarchyString(touchable) << std::endl;
    }
    return true;
  }

  G4Track* aTrack = aStep->GetTrack();
  G4double edep = aStep->GetTotalEnergyDeposit();

  uint32_t processFlag = 0;
  const G4VProcess* process = aStep->GetPostStepPoint()->GetProcessDefinedStep();
  if (process) {
    G4String processName = process->GetProcessName();
    if(processName.find("phot") != std::string::npos) {
      processFlag = process::PhotoelectricAbsorption;
    }
    else if(processName.find("compt") != std::string::npos) {
      processFlag = process::ComptonScattering;
    }
    else if(processName.find("Rayl") != std::string::npos) {
      processFlag = process::RayleighScattering;
    }
    else if(processName.find("conv") != std::string::npos) {
      processFlag = process::GammaConversion;
    }
  }

  const G4ParticleDefinition* particleDefinition = aTrack->GetDefinition();
  if (particleDefinition->GetParticleType() == "nucleus") {
    processFlag |= process::NucleusHit;
  }
    
  if (edep==0.0 && processFlag==0) { return true; }
  
  comptonsoft::DetectorHit_sptr hit(new comptonsoft::DetectorHit);
  hit->setTrackID(aTrack->GetTrackID());
  hit->setDetectorID(DetectorID);
  hit->setEnergyDeposit(edep);
  hit->setProcess(processFlag);
  hit->setParticle(particleDefinition->GetPDGEncoding());

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
  hit->setRealPosition(position);
  hit->setRealTime(aTrack->GetGlobalTime());

  for (G4int i = touchable->GetHistoryDepth()-1; i >= 0; i--) {
   G4VPhysicalVolume* physicalVolume = touchable->GetVolume(i);
    position += physicalVolume->GetFrameTranslation();
    if (physicalVolume->GetFrameRotation() != 0) {
      position = (*physicalVolume->GetFrameRotation()) * position;
    }
  }
  
  hit->setLocalPosition(position);

  detectorSystem_->getDeviceSimulationByID(DetectorID)->insertRawHit(hit);

  if (positionCalculation_) {
    std::set<int>::iterator it = positionCalculationSet_.find(DetectorID);
    
    if (it!=positionCalculationSet_.end()) {
      G4ThreeVector center(0, 0, 0);
      G4ThreeVector xdir(1, 0, 0);
      G4ThreeVector ydir(0, 1, 0);
      G4ThreeVector zdir(0, 0, 1);
      
      for (G4int i = 0; i < touchable->GetHistoryDepth(); i++) {
        G4VPhysicalVolume* physicalVolume = touchable->GetVolume(i);
        if (physicalVolume->GetFrameRotation() != 0) {
          center = (*physicalVolume->GetObjectRotation()) * center;
          xdir   = (*physicalVolume->GetObjectRotation()) * xdir;
          ydir   = (*physicalVolume->GetObjectRotation()) * ydir;
          zdir   = (*physicalVolume->GetObjectRotation()) * zdir;
        }
        center += physicalVolume->GetObjectTranslation();
      }
      
      VRealDetectorUnit* detector = detectorSystem_->getDetectorByID(DetectorID);
      detector->setCenterPosition(center);
      detector->setXAxisDirection(xdir);
      detector->setYAxisDirection(ydir);
      detector->setZAxisDirection(zdir);

      positionCalculationSet_.erase(it);
    }

    if (positionCalculationSet_.empty()) {
      positionCalculation_ = false;
    }
  }

  return true;
}

} /* namespace comptonsoft */
