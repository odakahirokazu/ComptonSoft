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

namespace comptonsoft {

ProcessCodedAperture::ProcessCodedAperture()
  : patternFile_("pattern.dat"),
    imageOwnerModule_("ExtractXrayEventImage"),
    outputName_("decoded")
{
}

ANLStatus ProcessCodedAperture::mod_define()
{
  define_parameter("num_encoded_image_x", &mod_class::numEncodedImageX_);
  define_parameter("num_encoded_image_y", &mod_class::numEncodedImageY_);
  define_parameter("num_aperture_x", &mod_class::numApertureX_);
  define_parameter("num_aperture_y", &mod_class::numApertureY_);
  define_parameter("num_sky_x", &mod_class::numSkyX_);
  define_parameter("num_sky_y", &mod_class::numSkyY_);
  define_parameter("detector_element_size", &mod_class::detectorElementSize_, unit::um, "um");
  define_parameter("aperture_element_size", &mod_class::apertureElementSize_, unit::um, "um");
  define_parameter("sky_image_angle_x", &mod_class::skyImageAngleX_, unit::arcsecond, "arcsecond");
  define_parameter("sky_image_angle_y", &mod_class::skyImageAngleY_, unit::arcsecond, "arcsecond");
  define_parameter("detector_to_aperture_distance", &mod_class::detectorToApertureDistance_, unit::cm, "cm");
  define_parameter("detector_roll_angle", &mod_class::detectorRollAngle_, unit::degree, "degree");
  define_parameter("aperture_roll_angle", &mod_class::apertureRollAngle_, unit::degree, "degree");
  define_parameter("aperture_offset_x", &mod_class::apertureOffsetX_, unit::um, "um");
  define_parameter("aperture_offset_y", &mod_class::apertureOffsetY_, unit::um, "um");
  define_parameter("sky_offset_angle_x", &mod_class::skyOffsetAngleX_, unit::degree, "degree");
  define_parameter("sky_offset_angle_y", &mod_class::skyOffsetAngleY_, unit::degree, "degree");
  define_parameter("num_decoding_iteration", &mod_class::numDecodingIteration_);
  define_parameter("decoding_mode", &mod_class::decodingMode_);
  define_parameter("pattern_file", &mod_class::patternFile_);
  define_parameter("image_owner_module", &mod_class::imageOwnerModule_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}    

ANLStatus ProcessCodedAperture::mod_initialize()
{
  get_module_NC(imageOwnerModule_, &imageOwner_);

  codedAperture_ = createCodedAperture();
  const int nx = NumApertureX();
  const int ny = NumApertureY();
  std::shared_ptr<image_t> pattern(new image_t(boost::extents[nx][ny]));
  std::ifstream fin(patternFile_);
  if(!fin) {
    std::cout << "cannnot open file" << std::endl;
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
      if (ix>=NumApertureX() || iy>=NumApertureY()) {
        std::cerr << "Input pattern does not match pixel number of apertures." << std::endl;
        return AS_ERROR;
      }
      (*pattern)[ix][iy] = num;
      iy += 1;
    }
    ix += 1;
    iy = 0;
  }
  fin.close();
  codedAperture_->setAperturePattern(pattern);
  std::shared_ptr<image_t> encodedImage = imageOwner_->Image();
  codedAperture_->setEncodedImage(encodedImage);
  bool sky_image_status = codedAperture_->setSkyImage();
  if (!sky_image_status) {
    std::cerr << "Sky image was not set appropriately." << std::endl;
    return AS_QUIT_ERROR;
  }
  const std::shared_ptr<image_t> decodedImageDummy = codedAperture_->DecodedImage();
  const int Nsx = decodedImageDummy->shape()[0];
  const int Nsy = decodedImageDummy->shape()[1];

  ANLStatus status = VCSModule::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }
  mkdir();
  const std::string name = "image";
  const std::string title = "Decoded image";
  const double dsx = codedAperture_->SkyElementAngleX();
  const double dsy = codedAperture_->SkyElementAngleY();
  const double xmin = -dsx*Nsx*0.5/unit::arcsecond;
  const double xmax = dsx*Nsx*0.5/unit::arcsecond;
  const double ymin = -dsy*Nsy*0.5/unit::arcsecond;
  const double ymax = dsy*Nsy*0.5/unit::arcsecond;
  histogram_ = new TH2D(name.c_str(), title.c_str(),
                        Nsx, xmin, xmax,
                        Nsy, ymin, ymax);
  
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
  ca->setNumSky(NumSkyX(), NumSkyY());
  ca->setSkySizeAngle(SkyImageAngleX(), SkyImageAngleY());
  ca->setDetectorToApertureDistance(detectorToApertureDistance_);
  ca->setDetectorRollAngle(DetectorRollAngle());
  ca->setApertureRollAngle(ApertureRollAngle());
  ca->setApertureOffset(ApertureOffsetX(), ApertureOffsetY());
  ca->setSkyOffsetAngle(SkyOffsetAngleX(), SkyOffsetAngleY());
  ca->setNumDecodingIteration(NumDecodingIteration());
  ca->setDecodingMode(DecodingMode());
  return ca;
}

void ProcessCodedAperture::decode()
{
  std::shared_ptr<image_t> image = imageOwner_->Image();
  codedAperture_->setEncodedImage(image);
  std::cout << "ProcessCodedAperture: decoding..." << std::endl;
  codedAperture_->decode();
  // coded_aperture_->mirrorDecodedImage();
  decodedImage_ = codedAperture_->DecodedImage();
}

void ProcessCodedAperture::fillHistogram()
{
  const image_t& image = *decodedImage_;
  TH2& h = *histogram_;

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
  const double zmax = histogram_->GetMaximum();
  const double zmin = zmax * (-1);
  histogram_->GetZaxis()->SetRangeUser(zmin, zmax);
  histogram_->Draw("colz");
  canvas->SaveAs(outputFile.c_str());
  filenames->push_back(outputFile);
}

} /* namespace comptonsoft */
