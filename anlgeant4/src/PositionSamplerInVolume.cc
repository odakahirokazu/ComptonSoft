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

#include "PositionSamplerInVolume.hh"
#include <cmath>
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"

namespace anlgeant4
{

PositionSamplerInVolume::PositionSamplerInVolume()
  : volumeType_(VolumeType_t::Box), theVolume_(nullptr),
    boxHSizeX_(0.0), boxHSizeY_(0.0), boxHSizeZ_(0.0),
    innerRadius_(0.0), outerRadius_(0.0),
    startPhi_(0.0), deltaPhi_(0.0),
    startTheta_(0.0), deltaTheta_(0.0)
{
}

void PositionSamplerInVolume::defineVolumeSize()
{
  G4PhysicalVolumeStore* PVStore = G4PhysicalVolumeStore::GetInstance();
  theVolume_ = PVStore->GetVolume(volumeID_[volumeID_.size()-1]);
  G4VSolid* solid = theVolume_->GetLogicalVolume()->GetSolid();

  if (solid->GetEntityType() == "G4Box") {
    volumeType_ = VolumeType_t::Box;
    G4Box* box = static_cast<G4Box*>(solid);
    boxHSizeX_ = box->GetXHalfLength();
    boxHSizeY_ = box->GetYHalfLength();
    boxHSizeZ_ = box->GetZHalfLength();
  }
  else if (solid->GetEntityType() == "G4Sphere") {
    volumeType_ = VolumeType_t::Sphere;
    G4Sphere* sphere = static_cast<G4Sphere*>(solid);
    innerRadius_ = sphere->GetInnerRadius();
    outerRadius_ = sphere->GetOuterRadius();
    startPhi_ = sphere->GetStartPhiAngle();
    deltaPhi_ = sphere->GetDeltaPhiAngle();
    startTheta_ = sphere->GetStartThetaAngle();
    deltaTheta_ = sphere->GetDeltaThetaAngle();

    boxHSizeX_ = outerRadius_;
    boxHSizeY_ = outerRadius_;
    boxHSizeZ_ = outerRadius_;
  }
  else if (solid->GetEntityType() == "G4Tubs") {
    volumeType_ = VolumeType_t::Tube;
    G4Tubs* tube = static_cast<G4Tubs*>(solid);
    innerRadius_ = tube->GetInnerRadius();
    outerRadius_ = tube->GetOuterRadius();
    startPhi_ = tube->GetStartPhiAngle();
    deltaPhi_ = tube->GetDeltaPhiAngle();
    boxHSizeZ_ = tube->GetZHalfLength();

    boxHSizeX_ = outerRadius_;
    boxHSizeY_ = outerRadius_;
  }
  else if (solid->GetEntityType() == "G4EllipticalTube") {
    volumeType_ = VolumeType_t::EllipticalTube;
    G4EllipticalTube* tube = static_cast<G4EllipticalTube*>(solid);
    boxHSizeX_ = tube->GetDx();
    boxHSizeY_ = tube->GetDy();
    boxHSizeZ_ = tube->GetDz();
  }
  else {
    volumeType_ = VolumeType_t::Any;
    double radius = 0.;
    const G4int numSampleSurface = 10000;
    for (G4int i=0; i<numSampleSurface; i++) {
      G4double tmpRadius = solid->GetPointOnSurface().mag();
      if (tmpRadius > radius) {
        radius = tmpRadius;
      }
    }
    boxHSizeX_ = radius;
    boxHSizeY_ = radius;
    boxHSizeZ_ = radius;
  }
}

G4ThreeVector PositionSamplerInVolume::samplePosition()
{
  double posx(0.0), posy(0.0), posz(0.0);
  
  const G4LogicalVolume* logvol = theVolume_->GetLogicalVolume();
  const G4VSolid* solid = logvol->GetSolid();

 position_sampling_start:
  if (false && volumeType_==VolumeType_t::Tube) {
    const double r2min = innerRadius_*innerRadius_;
    const double r2max = outerRadius_*outerRadius_;
    const double r = std::sqrt(r2min+G4UniformRand()*(r2max-r2min));
    const double phi = startPhi_ + G4UniformRand()*deltaPhi_;
    posx = r * std::cos(phi);
    posy = r * std::sin(phi);
    posz = -boxHSizeZ_ + 2.0 * boxHSizeZ_ * G4UniformRand();
  }
  else if (false && volumeType_==VolumeType_t::Sphere) {
    const double r3min = innerRadius_*innerRadius_*innerRadius_;
    const double r3max = outerRadius_*outerRadius_*outerRadius_;
    const double r = std::cbrt(r3min+G4UniformRand()*(r3max-r3min));
    const double phi = startPhi_ + G4UniformRand()*deltaPhi_;
    const double cosTheta0 = std::cos(startTheta_);
    const double cosTheta1 = std::cos(startTheta_+deltaTheta_);
    const double cosTheta = cosTheta0 + G4UniformRand()*(cosTheta1-cosTheta0);
    const double sinTheta = std::sqrt(1.0-cosTheta*cosTheta);
    posx = r * sinTheta * std::cos(phi);
    posy = r * sinTheta * std::sin(phi);
    posz = r * cosTheta;
  }
  else { // Box and others
    posx = -boxHSizeX_ + 2.0 * boxHSizeX_ * G4UniformRand();
    posy = -boxHSizeY_ + 2.0 * boxHSizeY_ * G4UniformRand();
    posz = -boxHSizeZ_ + 2.0 * boxHSizeZ_ * G4UniformRand();
  }
  G4ThreeVector position(posx, posy, posz);

  if (solid->Inside(position) != kInside) {
    goto position_sampling_start;
  }
  
  for (G4int i=0; i<logvol->GetNoDaughters(); i++) {
    G4VPhysicalVolume* daughter = logvol->GetDaughter(i);
    G4ThreeVector posInDaughter = position;
    posInDaughter += daughter->GetFrameTranslation();
    if (daughter->GetFrameRotation() != 0) {
      posInDaughter = (*daughter->GetFrameRotation()) * posInDaughter;
    }

    G4VSolid* daughterSolid = daughter->GetLogicalVolume()->GetSolid();

    if (daughterSolid->Inside(posInDaughter) == kInside) {
      goto position_sampling_start;
    }
  }

  G4PhysicalVolumeStore* PVStore = G4PhysicalVolumeStore::GetInstance();

  for (int i=volumeID_.size()-1; i>=0; i--) {
    G4VPhysicalVolume* physivol = PVStore->GetVolume(volumeID_[i]);
    if (physivol->GetFrameRotation() != 0) {
      position = (*physivol->GetObjectRotation()) * position;
    }
    position += physivol->GetObjectTranslation();
  }
  
  return position;
}

} /* namespace anlgeant4 */
