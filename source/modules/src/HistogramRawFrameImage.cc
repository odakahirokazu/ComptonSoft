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

#include "HistogramRawFrameImage.hh"

#include "FrameData.hh"
#include "TH2.h"
#include "TStyle.h"

using namespace anlnext;


namespace comptonsoft {

HistogramRawFrameImage::HistogramRawFrameImage()
  : outputName_("rawimage")
{
}

ANLStatus HistogramRawFrameImage::mod_define()
{
  define_parameter("detector_id", &mod_class::detectorID_);
  define_parameter("output_name", &mod_class::outputName_);
  define_parameter("rebin_x", &mod_class::rebinX_);
  define_parameter("rebin_y", &mod_class::rebinY_);
  
  return AS_OK;
}

ANLStatus HistogramRawFrameImage::mod_initialize()
{
  ANLStatus status = VCSModule::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }

  if (rebinX_<=0) {
    rebinX_ = 1;
  }
  if (rebinY_<=0) {
    rebinY_ = 1;
  }
  
  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID_);
  frame_ = detector->getFrameData();
  const image_t image = frame_->getRawFrame();

  mkdir();
  const std::string name = "rawimage";
  const std::string title = "RawImage";
  const int nx = image.shape()[0];
  const int ny = image.shape()[1];
  const int numx = nx/rebinX_;
  const int numy = ny/rebinY_;
  histogram_ = new TH2D(name.c_str(), title.c_str(),
                        numx, 0.0, static_cast<double>(nx),
                        numy, 0.0, static_cast<double>(ny));

  return AS_OK;
}


ANLStatus HistogramRawFrameImage::mod_end_run()
{
  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID_);
  if (detector == nullptr) {
    std::cout << "Detector " << detectorID_ << " does not exist." << std::endl;
    return AS_OK;
  }
  fillHistogram();

  return AS_OK;
}


void HistogramRawFrameImage::fillHistogram()
{
  histogram_->Reset();
  
  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID_);
  frame_ = detector->getFrameData();
  const image_t image = frame_->getRawFrame();
  
  const int Nx = image.shape()[0];
  const int Ny = image.shape()[1];
  for (int ix=0; ix<Nx; ix++) {
    for (int iy=0; iy<Ny; iy++) {
      const double v = image[ix][iy];
      histogram_->Fill(ix, iy, v/(rebinX_*rebinY_));
    }
  }
}

void HistogramRawFrameImage::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
{
  fillHistogram();

  const std::string outputFile = outputName_+".png";
  canvas->cd();
  gStyle->SetOptStat(0);
  gStyle->SetPalette(56);
  histogram_->Draw("colz");
  canvas->SaveAs(outputFile.c_str());
  filenames->push_back(outputFile);
}

} /* namespace comptonsoft */
