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

#include <fstream>
#include <sstream>
#include "TDirectory.h"
#include "TH2.h"
#include "TStyle.h"
#include "AstroUnits.hh"
#include "XrayEvent.hh"
#include "CodedAperture.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace {

void reset_image(std::shared_ptr<comptonsoft::image_t> image)
{
  const int nx = (*image).shape()[0];
  const int ny = (*image).shape()[1];
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      (*image)[ix][iy] = 0.0;
    }
  }
}

void add_image(std::shared_ptr<comptonsoft::image_t> image, std::shared_ptr<const comptonsoft::image_t> image1)
{
  const int nx = (*image).shape()[0];
  const int ny = (*image).shape()[1];
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      (*image)[ix][iy] += (*image1)[ix][iy];
    }
  }
}

std::shared_ptr<comptonsoft::image_t> create_image(int nx, int ny)
{
  std::shared_ptr<comptonsoft::image_t> image(new comptonsoft::image_t(boost::extents[nx][ny]));
  reset_image(image);
  return image;
}

} /* anonymous namespace */

namespace comptonsoft {

ProcessCodedAperture::ProcessCodedAperture()
  : patternFile_("pattern.dat"),
    imageOwnerModule_("ExtractXrayEventImage"),
    outputName_("decoded"),
    outputAngleUnit_("degree")
{
}

ANLStatus ProcessCodedAperture::mod_define()
{
  define_parameter("aperture_num_x", &mod_class::apertureNumX_);
  define_parameter("aperture_num_y", &mod_class::apertureNumY_);
  define_parameter("sky_num_x", &mod_class::skyNumX_);
  define_parameter("sky_num_y", &mod_class::skyNumY_);
  define_parameter("detector_element_size", &mod_class::detectorElementSize_, unit::cm, "cm");
  define_parameter("aperture_element_size", &mod_class::apertureElementSize_, unit::cm, "cm");
  define_parameter("sky_fov_x", &mod_class::skyFovX_, unit::degree, "degree");
  define_parameter("sky_fov_y", &mod_class::skyFovY_, unit::degree, "degree");
  define_parameter("detector_to_aperture_distance", &mod_class::detectorToApertureDistance_, unit::cm, "cm");
  define_parameter("detector_roll_angle", &mod_class::detectorRollAngle_, unit::degree, "degree");
  set_parameter_description("Roll angle of the detector with respect to the sky coordinate.");
  define_parameter("aperture_roll_angle", &mod_class::apertureRollAngle_, unit::degree, "degree");
  set_parameter_description("Roll angle of the aperture with respect to the detector coordinate.");
  define_parameter("aperture_offset_x", &mod_class::apertureOffsetX_, unit::cm, "cm");
  define_parameter("aperture_offset_y", &mod_class::apertureOffsetY_, unit::cm, "cm");
  define_parameter("sky_offset_x", &mod_class::skyOffsetX_, unit::degree, "degree");
  define_parameter("sky_offset_y", &mod_class::skyOffsetY_, unit::degree, "degree");
  define_parameter("num_decoding_iterations", &mod_class::numDecodingIterations_);
  define_parameter("decoding_mode", &mod_class::decodingMode_);
  define_parameter("pattern_file", &mod_class::patternFile_);
  define_parameter("image_owner_module", &mod_class::imageOwnerModule_);
  define_parameter("output_name", &mod_class::outputName_);
  define_parameter("output_angle_unit", &mod_class::outputAngleUnit_);
  
  return AS_OK;
}    

ANLStatus ProcessCodedAperture::mod_initialize()
{
  get_module_NC(imageOwnerModule_, &imageOwner_);

  codedAperture_ = createCodedAperture();
  const int nx = ApertureNumX();
  const int ny = ApertureNumY();
  std::shared_ptr<image_t> pattern = create_image(nx, ny);
  std::ifstream fin(patternFile_);
  if(!fin) {
    std::cerr << "cannnot open pattern file: " << patternFile_ << std::endl;
    return AS_ERROR;
  }
  std::string reading_line_buffer;
  int ix = 0;
  int iy = 0;
  while (std::getline(fin, reading_line_buffer)) {
    if (reading_line_buffer.size() == 0) { break; }
    std::istringstream is(reading_line_buffer);
    int num = 0;
    while(is >> num) {
      if (ix>=ApertureNumX() || iy>=ApertureNumY()) {
        std::cerr << "Input pattern does not match pixel number of apertures." << std::endl;
        return AS_QUIT_ERROR;
      }
      (*pattern)[ix][iy] = num;
      iy += 1;
    }
    ix += 1;
    iy = 0;
  }
  fin.close();
  codedAperture_->setAperturePattern(pattern);

  const int encoded_image_nx = imageOwner_->RecentImage()->shape()[0];
  const int encoded_image_ny = imageOwner_->RecentImage()->shape()[1];
  encodedImage_ = create_image(encoded_image_nx, encoded_image_ny);
  codedAperture_->setEncodedImage(encodedImage_);
  bool sky_image_status = codedAperture_->buildSkyImage();
  if (!sky_image_status) {
    std::cerr << "Sky image was not set appropriately." << std::endl;
    return AS_QUIT_ERROR;
  }
  const int Nsx = codedAperture_->DecodedImage()->shape()[0];
  const int Nsy = codedAperture_->DecodedImage()->shape()[1];
  decodedImage_ = create_image(Nsx, Nsy);
  decodedImageSum_ = create_image(Nsx, Nsy);

  double output_angle_unit_value = 0.0;
  if (outputAngleUnit_=="degree") {
    output_angle_unit_value = unit::degree;
  }
  else if (outputAngleUnit_=="radian") {
    output_angle_unit_value = unit::radian;
  }
  else if (outputAngleUnit_=="arcminute") {
    output_angle_unit_value = unit::arcminute;
  }
  else if (outputAngleUnit_=="arcsecond") {
    output_angle_unit_value = unit::arcsecond;
  }
  else {
    std::cerr << "Invalid output_angle_unit: " << outputAngleUnit_ << std::endl;
    return AS_QUIT_ERROR;
  }

  ANLStatus status = VCSModule::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }
  mkdir();
  const std::string name = "image";
  const std::string title = "Decoded image";
  const double dsx = codedAperture_->SkyElementAngleX();
  const double dsy = codedAperture_->SkyElementAngleY();
  const double xmin = -dsx*Nsx*0.5/output_angle_unit_value;
  const double xmax = dsx*Nsx*0.5/output_angle_unit_value;
  const double ymin = -dsy*Nsy*0.5/output_angle_unit_value;
  const double ymax = dsy*Nsy*0.5/output_angle_unit_value;
  histogram_ = new TH2D(name.c_str(), title.c_str(),
                        Nsx, xmin, xmax,
                        Nsy, ymin, ymax);
  histogram_->GetXaxis()->SetTitle(outputAngleUnit_.c_str());
  histogram_->GetYaxis()->SetTitle(outputAngleUnit_.c_str());

  return AS_OK;
}

ANLStatus ProcessCodedAperture::mod_analyze()
{
  std::shared_ptr<image_t> image = imageOwner_->RecentImage();
  add_image(encodedImage_, image);
  
  return AS_OK;
}

ANLStatus ProcessCodedAperture::mod_end_run()
{
  decode();
  fillHistogram();
  finalHistogramReady_ = true;

  return AS_OK;
}

std::unique_ptr<VCodedAperture> ProcessCodedAperture::createCodedAperture()
{
  std::unique_ptr<CodedAperture> ca(new CodedAperture);
  ca->setElementSizes(DetectorElementSize(),
                      DetectorElementSize(),
                      ApertureElementSize(),
                      ApertureElementSize());
  ca->setSkyNum(SkyNumX(), SkyNumY());
  ca->setSkyFov(SkyFovX(), SkyFovY());
  ca->setDetectorToApertureDistance(detectorToApertureDistance_);
  ca->setDetectorRollAngle(DetectorRollAngle());
  ca->setApertureRollAngle(ApertureRollAngle());
  ca->setApertureOffset(ApertureOffsetX(), ApertureOffsetY());
  ca->setSkyOffset(SkyOffsetX(), SkyOffsetY());
  ca->setNumDecodingIterations(NumDecodingIterations());
  ca->setDecodingMode(DecodingMode());
  return ca;
}

void ProcessCodedAperture::decode()
{
  codedAperture_->setEncodedImage(encodedImage_);
  std::cout << "ProcessCodedAperture: decoding..." << std::endl;
  codedAperture_->decode();
  decodedImage_ = codedAperture_->DecodedImage();
  add_image(decodedImageSum_, decodedImage_);
  reset_image(encodedImage_);
}

void ProcessCodedAperture::fillHistogram()
{
  const image_t& image = *decodedImageSum_;
  TH2& h = *histogram_;
  h.Reset();

  const int Nx = image.shape()[0];
  const int Ny = image.shape()[1];
  for (int ix=0; ix<Nx; ix++) {
    for (int iy=0; iy<Ny; iy++) {
      const double v = image[ix][iy];
      const int binX = ix + 1;
      const int binY = iy + 1;
      h.SetBinContent(binX, binY, v);
    }
  }
}

void ProcessCodedAperture::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
{
  if (!finalHistogramReady_) {
    decode();
    fillHistogram();
  }

  const std::string outputFile = outputName_+".png";
  canvas->cd();
  gStyle->SetOptStat(0);
  gStyle->SetPalette(56);
  const double zmax = histogram_->GetBinContent(histogram_->GetMaximumBin());
  const double zmin = 0;
  histogram_->GetZaxis()->SetRangeUser(zmin, zmax);
  histogram_->Draw("colz");
  canvas->SaveAs(outputFile.c_str());
  filenames->push_back(outputFile);
}

} /* namespace comptonsoft */
