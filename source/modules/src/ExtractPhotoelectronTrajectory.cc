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

#include "ExtractPhotoelectronTrajectory.hh"
#include <boost/format.hpp>
#include "TTree.h"
#include "TH2.h"
#include "AstroUnits.hh"
#include "InitialInformation.hh"
#include "DetectorHit.hh"
#include "HitTreeIOWithInitialInfo.hh"
#include "CSHitCollection.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

ExtractPhotoelectronTrajectory::ExtractPhotoelectronTrajectory()
  : pixelNumber_(64), pixelSize_(0.25*unit::um),
    weightMin_(2), pixelThreshold_(1.0*unit::eV)
{
}

ANLStatus ExtractPhotoelectronTrajectory::mod_define()
{
  define_parameter("pixel_number", &mod_class::pixelNumber_);
  define_parameter("pixel_size", &mod_class::pixelSize_, unit::cm, "cm");
  define_parameter("weight_min", &mod_class::weightMin_);
  define_parameter("pixel_threshold", &mod_class::pixelThreshold_, unit::keV, "keV");

  return AS_OK;
}

ANLStatus ExtractPhotoelectronTrajectory::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module("CSHitCollection", &hitCollection_);
  define_evs("ExtractPhotoelectronTrajectory:Fill");

  if (exist_module("InitialInformation")) {
    get_module_IF("InitialInformation", &initialInfo_);
  }
  else {
    return AS_QUIT_ERROR;
  }

  image_.reset(new Array2F(boost::extents[pixelNumber_][pixelNumber_]));
  resetImage();

  tree_ = new TTree("petree", "Photoelectron tree");
  tree_->Branch("image", image_->data(), (boost::format("image[%d][%d]/F")%pixelNumber_%pixelNumber_).str().c_str());
  tree_->Branch("angle", &angle_, "angle/F");
  tree_->Branch("weight", &weight_, "weight/I");
  
  return AS_OK;
}

ANLStatus ExtractPhotoelectronTrajectory::mod_analyze()
{
  if (hitCollection_->NumberOfTimeGroups()!=1) {
    return AS_SKIP;
  }

  const vector3_t center = initialInfo_->InitialPosition();
  const std::vector<DetectorHit_sptr>& hits = hitCollection_->getHits(0);
  if (hits.size() >= 2) {
    const vector3_t hit0Position = hits[0]->RealPosition();
    const vector3_t hit1Position = hits[1]->RealPosition();
    const vector3_t electronDirection = (hit1Position-hit0Position).unit();
    const double electronAngle = std::atan2(electronDirection.y(), electronDirection.x());
    angle_ = electronAngle;

    const int NPixels = pixelNumber_;
    const double imageSize = pixelSize_*pixelNumber_;
    TH2D image("image", "image",
               NPixels, -0.5*imageSize/unit::cm, +0.5*imageSize/unit::cm,
               NPixels, -0.5*imageSize/unit::cm, +0.5*imageSize/unit::cm);
    for (const auto& hit: hits) {
      const vector3_t position = hit->RealPosition()-center;
      const double energy = hit->EnergyDeposit();
      image.Fill(position.x()/unit::cm, position.y()/unit::cm, energy/unit::keV);
    }

    copyImage(&image);
    const int weight = calculateImageWeight();
    weight_ = weight;

    if (weight >= weightMin_) {
      tree_->Fill();
      set_evs("ExtractPhotoelectronTrajectory:Fill");
    }
  }

  return AS_OK;
}

void ExtractPhotoelectronTrajectory::resetImage()
{
  const int NPixels = pixelNumber_;
  for (int ix=0; ix<NPixels; ix++) {
    for (int iy=0; iy<NPixels; iy++) {
      (*image_)[ix][iy] = 0.0;
    }
  }
}

void ExtractPhotoelectronTrajectory::copyImage(const TH2* image)
{
  const int NPixels = pixelNumber_;
  for (int ix=0; ix<NPixels; ix++) {
    for (int iy=0; iy<NPixels; iy++) {
      const double v = image->GetBinContent(ix+1, iy+1);
      (*image_)[ix][iy] = v;
    }
  }
}

int ExtractPhotoelectronTrajectory::calculateImageWeight() const
{
  int weight = 0;
  const int NPixels = pixelNumber_;
  const double thresholdValue = pixelThreshold_/unit::keV;
  for (int ix=0; ix<NPixels; ix++) {
    for (int iy=0; iy<NPixels; iy++) {
      const double v = (*image_)[ix][iy];
      if (v >= thresholdValue) {
        ++weight;
      }
    }
  }
  return weight;
}

} /* namespace comptonsoft */
