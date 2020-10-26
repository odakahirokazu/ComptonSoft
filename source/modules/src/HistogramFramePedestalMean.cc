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

#include "HistogramFramePedestalMean.hh"

#include "AstroUnits.hh"
#include "TH1.h"
#include "TStyle.h"
#include "FrameData.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

HistogramFramePedestalMean::HistogramFramePedestalMean()
  : numBins_(64), min_(-64.0), max_(64.0),
    outputName_("pedestal_mean")
{
}

ANLStatus HistogramFramePedestalMean::mod_define()
{
  define_parameter("detector_id", &mod_class::detectorID_);
  define_parameter("num_bins", &mod_class::numBins_);
  define_parameter("min", &mod_class::min_);
  define_parameter("max", &mod_class::max_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}

ANLStatus HistogramFramePedestalMean::mod_initialize()
{
  ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  mkdir();
  const std::string name = "pedestal_mean";
  const std::string title = "pedestal mean";
  histogram_ = new TH1D(name.c_str(), title.c_str(),
                        numBins_, min_, max_);
  return AS_OK;
}

ANLStatus HistogramFramePedestalMean::mod_analyze()
{
  return AS_OK;
}

ANLStatus HistogramFramePedestalMean::mod_end_run()
{
  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID_);
  if (detector == nullptr) {
    std::cout << "Detector " << detectorID_ << " does not exist." << std::endl;
    return AS_OK;
  }
  fillInHistogram();
  return AS_OK;
}

void HistogramFramePedestalMean::fillInHistogram()
{
  histogram_->Reset();
  
  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID_);
  frame_ = detector->getFrameData();
  frame_->calculateStatistics();
  const image_t& pedestals = frame_->getPedestals();

  const int nx = pedestals.shape()[0];
  const int ny = pedestals.shape()[1];
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      const double mean = pedestals[ix][iy];
      histogram_->Fill(mean);
    }
  }
  std::cout << std::endl;
}


void HistogramFramePedestalMean::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
{
  fillInHistogram();
  const std::string outputFile = outputName_+".png";
  canvas->cd();
  gStyle->SetOptStat("e");
  gStyle->SetStatH(0.15);
  histogram_->Draw();
  canvas->SaveAs(outputFile.c_str());
  filenames->push_back(outputFile);
}

} /* namespace comptonsoft */
