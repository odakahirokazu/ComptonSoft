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

#include "HistogramFramePedestal.hh"

#include "AstroUnits.hh"
#include "TH2.h"
#include "TStyle.h"
#include "FrameData.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

HistogramFramePedestal::HistogramFramePedestal()
  : meanNumBins_(64), meanMin_(-64.0), meanMax_(64.0),
    sigmaNumBins_(64), sigmaMin_(0.0), sigmaMax_(64.0),
    outputName_("pedestal")
{
}

ANLStatus HistogramFramePedestal::mod_define()
{
  define_parameter("detector_id", &mod_class::detectorID_);
  define_parameter("mean_num_bins", &mod_class::meanNumBins_);
  define_parameter("mean_min", &mod_class::meanMin_);
  define_parameter("mean_max", &mod_class::meanMax_);
  define_parameter("sigma_num_bins", &mod_class::sigmaNumBins_);
  define_parameter("sigma_min", &mod_class::sigmaMin_);
  define_parameter("sigma_max", &mod_class::sigmaMax_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}

ANLStatus HistogramFramePedestal::mod_initialize()
{
  ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }
  mkdir();
  const std::string name = "pedestal";
  const std::string title = "pedestal";
  histogram_ = new TH2D(name.c_str(), title.c_str(),
                        sigmaNumBins_, sigmaMin_, sigmaMax_,
                        meanNumBins_, meanMin_, meanMax_);
  return AS_OK;
}

ANLStatus HistogramFramePedestal::mod_analyze()
{
  return AS_OK;
}

ANLStatus HistogramFramePedestal::mod_end_run()
{
  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID_);
  if (detector == nullptr) {
    std::cout << "Detector " << detectorID_ << " does not exist." << std::endl;
    return AS_OK;
  }
  fillInHistogram();
  return AS_OK;
}

void HistogramFramePedestal::fillInHistogram()
{
  histogram_->Reset();
  
  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID_);
  frame_ = detector->getFrameData();
  frame_->calculateStatistics();
  const image_t& pedestals = frame_->getPedestals();
  const image_t& frameDeviation = frame_->getDeviationFrame();

  const int nx = pedestals.shape()[0];
  const int ny = pedestals.shape()[1];
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      const double mean = pedestals[ix][iy];
      const double sigma = frameDeviation[ix][iy];
      histogram_->Fill(sigma, mean);
    }
  }
  std::cout << std::endl;
}


void HistogramFramePedestal::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
{
  fillInHistogram();
  const std::string outputFile = outputName_+".png";
  canvas->cd();
  canvas->SetLogz(1);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(56);
  histogram_->Draw("colz");
  canvas->SaveAs(outputFile.c_str());
  filenames->push_back(outputFile);
}

} /* namespace comptonsoft */
