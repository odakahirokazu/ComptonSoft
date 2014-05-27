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

#include "VolumeInsideSample.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"

#include "TMath.h"

using namespace comptonsoft;


VolumeInsideSample::VolumeInsideSample()
  : VolumeBox(true),
    BoxHSizeX(0.0), BoxHSizeY(0.0), BoxHSizeZ(0.0),
    TargetVolume(0)
{
}


void VolumeInsideSample::defineVolumeSize()
{
  G4PhysicalVolumeStore* PVStore = G4PhysicalVolumeStore::GetInstance();
  TargetVolume = PVStore->GetVolume(VolumeId[VolumeId.size()-1]);
  G4VSolid* solid = TargetVolume->GetLogicalVolume()->GetSolid();

  if (solid->GetEntityType() == "G4Box") {
    VolumeBox = true;
    G4Box* box = static_cast<G4Box*>(solid);
    BoxHSizeX = box->GetXHalfLength();
    BoxHSizeY = box->GetYHalfLength();
    BoxHSizeZ = box->GetZHalfLength();
  }
  else if (solid->GetEntityType() == "G4Sphere") {
    VolumeBox = false;
    G4Sphere* sphere = static_cast<G4Sphere*>(solid);
    double radius = sphere->GetOuterRadius();
    BoxHSizeX = radius;
    BoxHSizeY = radius;
    BoxHSizeZ = radius;
  }
  else if (solid->GetEntityType() == "G4Tubs") {
    VolumeBox = false;
    G4Tubs* tube = static_cast<G4Tubs*>(solid);
    double radius = tube->GetOuterRadius();
    double halfHeight = tube->GetZHalfLength();
    BoxHSizeX = radius;
    BoxHSizeY = radius;
    BoxHSizeZ = halfHeight;
  }
  else if (solid->GetEntityType() == "G4EllipticalTube") {
    VolumeBox = false;
    G4EllipticalTube* tube = static_cast<G4EllipticalTube*>(solid);
    BoxHSizeX = tube->GetDx();
    BoxHSizeY = tube->GetDy();
    BoxHSizeZ = tube->GetDz();
  }
  else {
    VolumeBox = false;
    double radius = 0.;
    const G4int numSampleSurface = 10000;
    for (G4int i=0; i<numSampleSurface; i++) {
      G4double tmpRadius = solid->GetPointOnSurface().mag();
      if (tmpRadius > radius) {
	radius = tmpRadius;
      }
    }
    BoxHSizeX = radius;
    BoxHSizeY = radius;
    BoxHSizeZ = radius;
  }
}


G4ThreeVector VolumeInsideSample::samplePosition()
{
  G4double posx = 0.0*cm, posy = 0.0*cm, posz = 0.0*cm;
  
  const G4LogicalVolume* logvol = TargetVolume->GetLogicalVolume();
  const G4VSolid* solid = logvol->GetSolid();

 position_sampling_start:
  posx = -BoxHSizeX + 2.0 * BoxHSizeX * G4UniformRand();
  posy = -BoxHSizeY + 2.0 * BoxHSizeY * G4UniformRand();
  posz = -BoxHSizeZ + 2.0 * BoxHSizeZ * G4UniformRand();
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

  for (int i=VolumeId.size()-1; i>=0; i--) {
    G4VPhysicalVolume* physivol = PVStore->GetVolume(VolumeId[i]);
    if (physivol->GetFrameRotation() != 0) {
      position = (*physivol->GetObjectRotation()) * position;
    }
    position += physivol->GetObjectTranslation();
  }
  
  return position;
}
