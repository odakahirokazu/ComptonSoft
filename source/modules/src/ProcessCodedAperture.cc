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

#include "ProcessCodedAperture.hh"

#include "XrayEvent.hh"
#include "AstroUnits.hh"
#include <fstream>
#include <sstream>
#include "TDirectory.h"
#include "TH2.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

ProcessCodedAperture::ProcessCodedAperture()
  : outputFile_("decoded.png")
{
}

ANLStatus ProcessCodedAperture::mod_define()
{
  define_parameter("num_encoded_image_x", &mod_class::numEncodedImageX_);
  define_parameter("num_encoded_image_y", &mod_class::numEncodedImageY_);
  define_parameter("num_decoded_image_x", &mod_class::numDecodedImageX_);
  define_parameter("num_decoded_image_y", &mod_class::numDecodedImageY_);
  define_parameter("num_mask_x", &mod_class::numMaskX_);
  define_parameter("num_mask_y", &mod_class::numMaskY_);
  define_parameter("detector_element_size", &mod_class::detectorElementSize_, unit::um, "um");
  define_parameter("mask_element_size", &mod_class::maskElementSize_, unit::um, "um");
  define_parameter("mask_to_detector_distance", &mod_class::maskToDetectorDistance_, unit::mm, "mm");
  define_parameter("source_to_mask_distance", &mod_class::sourceToMaskDistance_, unit::mm, "mm"); 
  define_parameter("pattern_file", &mod_class::patternFile_);
  define_parameter("analyze_every_time", &mod_class::analyzeEveryTime_);
  define_parameter("image_owner_module", &mod_class::imageOwnerModule_);
  
  return AS_OK;
}    

ANLStatus ProcessCodedAperture::mod_initialize()
{
  get_module_NC(imageOwnerModule_, &image_owner_);
  coded_aperture_.reset(createCodedAperture());

  coded_aperture_->setNumEncodedImageX(1);
  coded_aperture_->setNumEncodedImageX(NumEncodedImageX());
  coded_aperture_->setNumEncodedImageY(NumEncodedImageY());
  coded_aperture_->setNumDecodedImageX(NumDecodedImageX());
  coded_aperture_->setNumDecodedImageY(NumDecodedImageY());
  coded_aperture_->setNumMaskX(NumMaskX());
  coded_aperture_->setNumMaskY(NumMaskY());
  coded_aperture_->setDetectorElementSize(DetectorElementSize());
  coded_aperture_->setMaskElementSize(MaskElementSize());
  coded_aperture_->setMaskToDetectorDistance(MaskToDetectorDistance());
  coded_aperture_->setSourceToMaskDistance(SourceToMaskDistance());

  const int nx = NumMaskX();
  const int ny = NumMaskY();
  pattern_.resize(boost::extents[nx][ny]);
  std::ifstream fin(patternFile_);
  if(!fin) {
    std::cout << "cannnot open file" << std::endl;
    return AS_ERROR;
  }
  std::string reading_line_buffer;
  int ix = 0;
  int iy = 0;
  while(std::getline(fin, reading_line_buffer)) {
    if(reading_line_buffer.size() == 0) { break; }
    std::istringstream is(reading_line_buffer);
    int num = 0;
    while(is >> num) {
      if (ix>=NumMaskX() || iy>=NumMaskY()) {
        std::cout << "Input pattern does not match pixel number of masks." << std::endl;
        return AS_ERROR;
      }
      pattern_[ix][iy] = num;
      iy += 1;
    }
    ix += 1;
    iy = 0;
  }
  fin.close();

  coded_aperture_->setup();
  coded_aperture_->setPattern(pattern_);

  ANLStatus status = VCSModule::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }
  mkdir();
  const std::string name = "DecodedImage";
  const std::string title = "Decoded Image";
  totalHistogram_ = new TH2D(name.c_str(), title.c_str(), NumDecodedImageX(), 0.0, static_cast<double>(NumDecodedImageX()), 
                             NumDecodedImageY(), 0.0, static_cast<double>(NumDecodedImageY()));
 
  return AS_OK;
}

ANLStatus ProcessCodedAperture::mod_begin_run()
{
  const int nx = NumDecodedImageX();
  const int ny = NumDecodedImageY();
  accumulatedDecodedImage_.resize(boost::extents[nx][ny]);
  totalDecodedImage_.resize(boost::extents[nx][ny]);
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      accumulatedDecodedImage_[ix][iy] = 0.0;
      totalDecodedImage_[ix][iy] = 0.0;
    }
  }
  coded_aperture_->calculateApertureRatio();
  coded_aperture_->makeDecoderArray();
  coded_aperture_->calculateFieldOfView();

  return AS_OK;
}

ANLStatus ProcessCodedAperture::mod_analyze()
{
  if (analyzeEveryTime_) {
    image_t image = image_owner_->Image();
    coded_aperture_->setEncodedImage(image);
    coded_aperture_->decode();
    coded_aperture_->mirrorDecodedImage();
    image_t decodedImageThisTime = coded_aperture_->DecodedImage();
    const int nx = NumDecodedImageX();
    const int ny = NumDecodedImageY();
    for (int ix=0; ix<nx; ix++) {
      for (int iy=0; iy<ny; iy++) {
        accumulatedDecodedImage_[ix][iy] += decodedImageThisTime[ix][iy];
      }
    }
  }

  return AS_OK;
}

ANLStatus ProcessCodedAperture::mod_end_run()
{
  image_t image = image_owner_->TotalImage();
  coded_aperture_->setEncodedImage(image);
  coded_aperture_->decode();
  coded_aperture_->mirrorDecodedImage();
  totalDecodedImage_ = coded_aperture_->DecodedImage();

  fillHistogram();

  return AS_OK;
}

CodedAperture* ProcessCodedAperture::createCodedAperture()
{
  return new comptonsoft::CodedAperture;
}

void ProcessCodedAperture::fillHistogram()
{
  const int nx = NumDecodedImageX();
  const int ny = NumDecodedImageY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      const double v = totalDecodedImage_[ix][iy];
      totalHistogram_->SetBinContent(ix+1, iy+1, v);
    }
  }
}

void ProcessCodedAperture::drawOutputFiles(TCanvas* c1, std::vector<std::string>* filenames)
{
  image_t image = image_owner_->TotalImage();
  coded_aperture_->setEncodedImage(image);
  coded_aperture_->decode();
  coded_aperture_->mirrorDecodedImage();
  totalDecodedImage_ = coded_aperture_->DecodedImage();
  fillHistogram();
  
  c1->cd();
  gStyle->SetOptStat(0);
  gStyle->SetPalette(56);
  const double zmax = totalHistogram_->GetMaximum();
  const double zmin = zmax * (-1);
  totalHistogram_->GetZaxis()->SetRangeUser(zmin, zmax);
  totalHistogram_->Draw("colz");
  c1->SaveAs(outputFile_.c_str());
  filenames->push_back(outputFile_);
}

} /* namespace comptonsoft */
