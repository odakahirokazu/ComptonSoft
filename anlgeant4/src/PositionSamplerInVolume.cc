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
#include "Randomize.hh"

#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"
#include "G4Ellipsoid.hh"

namespace anlgeant4
{

PositionSamplerInVolume::PositionSamplerInVolume()
  : volume_type_(VolumeType_t::Box), theVolume_(nullptr),
    box_half_size_x_(0.0), box_half_size_y_(0.0), box_half_size_z_(0.0),
    inner_radius_(0.0), outer_radius_(0.0),
    start_phi_(0.0), delta_phi_(0.0),
    start_theta_(0.0), delta_theta_(0.0)
{
}

void PositionSamplerInVolume::define_volume_size()
{
  G4PhysicalVolumeStore* PVStore = G4PhysicalVolumeStore::GetInstance();
  theVolume_ = PVStore->GetVolume(volume_ID_[volume_ID_.size()-1]);
  G4VSolid* solid = theVolume_->GetLogicalVolume()->GetSolid();

  if (solid->GetEntityType() == "G4Box") {
    volume_type_ = VolumeType_t::Box;
    G4Box* box = static_cast<G4Box*>(solid);
    box_half_size_x_ = box->GetXHalfLength();
    box_half_size_y_ = box->GetYHalfLength();
    box_half_size_z_ = box->GetZHalfLength();
  }
  else if (solid->GetEntityType() == "G4Sphere") {
    volume_type_ = VolumeType_t::Sphere;
    G4Sphere* sphere = static_cast<G4Sphere*>(solid);
    inner_radius_ = sphere->GetInnerRadius();
    outer_radius_ = sphere->GetOuterRadius();
    start_phi_ = sphere->GetStartPhiAngle();
    delta_phi_ = sphere->GetDeltaPhiAngle();
    start_theta_ = sphere->GetStartThetaAngle();
    delta_theta_ = sphere->GetDeltaThetaAngle();

    box_half_size_x_ = outer_radius_;
    box_half_size_y_ = outer_radius_;
    box_half_size_z_ = outer_radius_;
  }
  else if (solid->GetEntityType() == "G4Tubs") {
    volume_type_ = VolumeType_t::Tube;
    G4Tubs* tube = static_cast<G4Tubs*>(solid);
    inner_radius_ = tube->GetInnerRadius();
    outer_radius_ = tube->GetOuterRadius();
    start_phi_ = tube->GetStartPhiAngle();
    delta_phi_ = tube->GetDeltaPhiAngle();
    box_half_size_z_ = tube->GetZHalfLength();

    box_half_size_x_ = outer_radius_;
    box_half_size_y_ = outer_radius_;
  }
  else if (solid->GetEntityType() == "G4EllipticalTube") {
    volume_type_ = VolumeType_t::EllipticalTube;
    G4EllipticalTube* tube = static_cast<G4EllipticalTube*>(solid);
    box_half_size_x_ = tube->GetDx();
    box_half_size_y_ = tube->GetDy();
    box_half_size_z_ = tube->GetDz();
  }
  else if (solid->GetEntityType() == "G4Ellipsoid") {
    volume_type_ = VolumeType_t::Ellipsoid;
    G4Ellipsoid* ellipsoid = static_cast<G4Ellipsoid*>(solid);
    box_half_size_x_ = ellipsoid->GetSemiAxisMax(0);
    box_half_size_y_ = ellipsoid->GetSemiAxisMax(1);
    box_half_size_z_ = ellipsoid->GetSemiAxisMax(2);
    const double zTop = ellipsoid->GetZTopCut();
    const double zBottom = ellipsoid->GetZBottomCut();
    if (std::abs(zBottom) > std::abs(zTop)) {
      box_half_size_z_ = std::abs(zBottom);
    }
    else {
      box_half_size_z_ = std::abs(zTop);
    }
  }
  else {
    volume_type_ = VolumeType_t::Any;
    double radius = 0.;
    const int num_samples = 10000;
    for (int i=0; i<num_samples; i++) {
      const double radius_trial = solid->GetPointOnSurface().mag();
      if (radius_trial > radius) {
        radius = radius_trial;
      }
    }
    box_half_size_x_ = radius;
    box_half_size_y_ = radius;
    box_half_size_z_ = radius;
  }
}

G4ThreeVector PositionSamplerInVolume::sample_position() const
{
  double posx(0.0), posy(0.0), posz(0.0);

  const G4LogicalVolume* logvol = theVolume_->GetLogicalVolume();
  const G4VSolid* solid = logvol->GetSolid();

 position_sampling_start:
  if (false && volume_type_==VolumeType_t::Tube) {
    const double r2min = inner_radius_*inner_radius_;
    const double r2max = outer_radius_*outer_radius_;
    const double r = std::sqrt(r2min+G4UniformRand()*(r2max-r2min));
    const double phi = start_phi_ + G4UniformRand()*delta_phi_;
    posx = r * std::cos(phi);
    posy = r * std::sin(phi);
    posz = -box_half_size_z_ + 2.0 * box_half_size_z_ * G4UniformRand();
  }
  else if (false && volume_type_==VolumeType_t::Sphere) {
    const double r3min = inner_radius_*inner_radius_*inner_radius_;
    const double r3max = outer_radius_*outer_radius_*outer_radius_;
    const double r = std::cbrt(r3min+G4UniformRand()*(r3max-r3min));
    const double phi = start_phi_ + G4UniformRand()*delta_phi_;
    const double cosTheta0 = std::cos(start_theta_);
    const double cosTheta1 = std::cos(start_theta_+delta_theta_);
    const double cosTheta = cosTheta0 + G4UniformRand()*(cosTheta1-cosTheta0);
    const double sinTheta = std::sqrt(1.0-cosTheta*cosTheta);
    posx = r * sinTheta * std::cos(phi);
    posy = r * sinTheta * std::sin(phi);
    posz = r * cosTheta;
  }
  else { // Box and others
    posx = -box_half_size_x_ + 2.0 * box_half_size_x_ * G4UniformRand();
    posy = -box_half_size_y_ + 2.0 * box_half_size_y_ * G4UniformRand();
    posz = -box_half_size_z_ + 2.0 * box_half_size_z_ * G4UniformRand();
  }
  G4ThreeVector position(posx, posy, posz);

  if (solid->Inside(position) != kInside) {
    goto position_sampling_start;
  }

  for (std::size_t i=0; i<logvol->GetNoDaughters(); i++) {
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

  for (int i=volume_ID_.size()-1; i>=0; i--) {
    G4VPhysicalVolume* physivol = PVStore->GetVolume(volume_ID_[i]);
    if (physivol->GetFrameRotation() != 0) {
      position = (*physivol->GetObjectRotation()) * position;
    }
    position += physivol->GetObjectTranslation();
  }

  return position;
}

} /* namespace anlgeant4 */
