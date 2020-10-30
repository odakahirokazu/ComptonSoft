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

#include "HistogramXrayEventPerFrame.hh"

#include "XrayEventCollection.hh"
#include "AstroUnits.hh"
#include "TH1.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

HistogramXrayEventPerFrame::HistogramXrayEventPerFrame()
  : numBins_(512), frameMin_(-0.5), frameMax_(4095.5),
    collectionModule_("XrayEventCollection"), outputName_("eventPerFrame")
{
}

ANLStatus HistogramXrayEventPerFrame::mod_define()
{
  define_parameter("num_bins", &mod_class::numBins_);
  define_parameter("frame_min", &mod_class::frameMin_);
  define_parameter("frame_max", &mod_class::frameMax_);
  define_parameter("collection_module", &mod_class::collectionModule_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}

ANLStatus HistogramXrayEventPerFrame::mod_initialize()
{
  get_module_NC(collectionModule_, &collection_);

  ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  mkdir();
  const std::string name = "eventPerFrame";
  const std::string title = "event per frame";
  histogram_ = new TH1D(name.c_str(), title.c_str(),
                        numBins_, frameMin_, frameMax_);

  return AS_OK;
}

ANLStatus HistogramXrayEventPerFrame::mod_analyze()
{
  const double framePerBin = (frameMax_ - frameMin_) / numBins_;
  for (const auto& event: collection_->getEvents()) {
    const double frame = event->FrameID();
    histogram_->Fill(frame, 1.0/framePerBin);
  }

  return AS_OK;
}

void HistogramXrayEventPerFrame::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
{
  const std::string outputFile = outputName_+".png";
  canvas->cd();
  gStyle->SetOptStat("e");
  gStyle->SetStatH(0.15);
  histogram_->Draw();
  canvas->SaveAs(outputFile.c_str());
  filenames->push_back(outputFile);
}

} /* namespace comptonsoft */
