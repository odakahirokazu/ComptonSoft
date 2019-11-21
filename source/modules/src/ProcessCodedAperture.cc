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
  define_parameter("num_mask_x", &mod_class::numMaskX_);
  define_parameter("num_mask_y", &mod_class::numMaskY_);
  define_parameter("detector_element_size", &mod_class::detectorElementSize_, unit::um, "um");
  define_parameter("mask_element_size", &mod_class::maskElementSize_, unit::um, "um");
  define_parameter("pattern_file", &mod_class::patternFile_);
  define_parameter("image_owner_module", &mod_class::imageOwnerModule_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}    

ANLStatus ProcessCodedAperture::mod_initialize()
{
  get_module_NC(imageOwnerModule_, &imageOwner_);

  codedAperture_ = createCodedAperture();
  
  const int nx = NumMaskX();
  const int ny = NumMaskY();
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
      if (ix>=NumMaskX() || iy>=NumMaskY()) {
        std::cout << "Input pattern does not match pixel number of masks." << std::endl;
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
  histogram_ = new TH2D(name.c_str(), title.c_str(),
                        Nsx, 0.0, static_cast<double>(Nsx), 
                        Nsy, 0.0, static_cast<double>(Nsy));
  
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
                      MaskElementSize(),
                      MaskElementSize());
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
