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

#include "MakeXrayEventImage.hh"

#include "XrayEvent.hh"
#include "XrayEventCollection.hh"
#include "XrayEventTreeIO.hh"
#include "AstroUnits.hh"
#include "TH2.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

MakeXrayEventImage::MakeXrayEventImage()
  : collectionModule_("XrayEventCollection"),
    outputFile_("image.png")
{
}

ANLStatus MakeXrayEventImage::mod_define()
{
  define_parameter("collection_module", &mod_class::collectionModule_);
  define_parameter("num_pixel_x", &mod_class::numPixelX_);
  define_parameter("num_pixel_y", &mod_class::numPixelY_);
  define_parameter("offset_x", &mod_class::offsetX_);
  define_parameter("offset_y", &mod_class::offsetY_);
  define_parameter("rotation_angle", &mod_class::rotationAngle_, unit::degree, "degree");
  define_parameter("rotation_center_x", &mod_class::rotationCenterX_);
  define_parameter("rotation_center_y", &mod_class::rotationCenterY_);
  define_parameter("output", &mod_class::outputFile_);
  
  return AS_OK;
}

ANLStatus MakeXrayEventImage::mod_initialize()
{
  get_module_NC(collectionModule_, &collection_);
  const int nx = NumPixelX();
  const int ny = NumPixelY();
  image_.resize(boost::extents[nx][ny]);
  totalImage_.resize(boost::extents[nx][ny]);

  ANLStatus status = VCSModule::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }
  mkdir();
  const std::string name = "EncodedImage";
  const std::string title = "Encoded Image";
  totalHistogram_ = new TH2D(name.c_str(), title.c_str(),
                             nx, 0.0, static_cast<double>(nx),
                             ny, 0.0, static_cast<double>(ny));

  return AS_OK;
}

ANLStatus MakeXrayEventImage::mod_begin_run()
{
  resetImage(image_);
  resetImage(totalImage_);
  return AS_OK;
}

ANLStatus MakeXrayEventImage::mod_analyze()
{
  resetImage(image_);
  for (const auto& event: collection_->getEvents()) {
    const int ix = event->X() - OffsetX();
    const int iy = event->Y() - OffsetY();
    const int nx = image_.shape()[0];
    const int ny = image_.shape()[1];
    if (0<=ix && ix<nx && 0<=iy && iy<ny) {
      image_[ix][iy] += 1.0;
    }
  }
  rotateImage(image_);

  const int nx = image_.shape()[0];
  const int ny = image_.shape()[1];
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      totalImage_[ix][iy] += image_[ix][iy];
    }
  }

  return AS_OK;
}

ANLStatus MakeXrayEventImage::mod_end_run()
{
  rotateImage(totalImage_);
  fillHistogram();
  return AS_OK;
}

void MakeXrayEventImage::resetImage(image_t& image) const
{
  const int nx = image.shape()[0];
  const int ny = image.shape()[1];
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      image[ix][iy] = 0.0;
    }
  }
}

void MakeXrayEventImage::rotateImage(image_t& image) const
{
  if (rotationAngle_ == 0.0) { return; }

  const int nx = image.shape()[0];
  const int ny = image.shape()[1];
  const double cx = RotationCenterX();
  const double cy = RotationCenterY();
  const double theta = RotationAngle();
  image_t new_image(boost::extents[nx][ny]);
  resetImage(new_image);
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      const double v = image[ix][iy];
      const double x = ((ix - cx ) * std::cos(theta) - (iy - cy) * std::sin(theta));
      const double y = ((ix - cx ) * std::sin(theta) + (iy - cy) * std::sin(theta));
      const int ixx = static_cast<int>(x+0.5+cx);
      const int iyy = static_cast<int>(y+0.5+cx);
      if (ixx>=0 && ixx<nx && iyy>=0 && iyy<ny){
        new_image[ixx][iyy] += v;
      }
    }
  }
  image = std::move(new_image);
}

void MakeXrayEventImage::fillHistogram()
{
  const int nx = NumPixelX();
  const int ny = NumPixelY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      const double v = totalImage_[ix][iy];
      totalHistogram_->SetBinContent(ix+1, iy+1, v);
    }
  }
}

void MakeXrayEventImage::drawOutputFiles(TCanvas* c1, std::vector<std::string>* filenames)
{
  fillHistogram();
  c1->cd();
  gStyle->SetOptStat(0);
  totalHistogram_->Draw("colz");
  c1->SaveAs(outputFile_.c_str());
  filenames->push_back(outputFile_);
}

} /* namespace comptonsoft */
