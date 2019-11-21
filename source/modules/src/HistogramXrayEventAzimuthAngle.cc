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

#include "HistogramXrayEventAzimuthAngle.hh"

#include "XrayEventCollection.hh"
#include "AstroUnits.hh"
#include "TH1.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

HistogramXrayEventAzimuthAngle::HistogramXrayEventAzimuthAngle()
  : numBins_(33), angleMin_(-180.0-0.5*11.25), angleMax_(180.0+0.5*11.25),
    collectionModule_("XrayEventCollection"), outputName_("angle")
{
}

ANLStatus HistogramXrayEventAzimuthAngle::mod_define()
{
  define_parameter("num_bins", &mod_class::numBins_);
  define_parameter("angle_min", &mod_class::angleMin_, 1.0, "degree");
  define_parameter("angle_max", &mod_class::angleMax_, 1.0, "degree");
  define_parameter("collection_module", &mod_class::collectionModule_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}

ANLStatus HistogramXrayEventAzimuthAngle::mod_initialize()
{
  get_module_NC(collectionModule_, &collection_);

  ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  mkdir();
  const std::string name = "azimuth_angle";
  const std::string title = "Azimuth angle";
  histogram_ = new TH1D(name.c_str(), title.c_str(),
                        numBins_, angleMin_, angleMax_);

  return AS_OK;
}

ANLStatus HistogramXrayEventAzimuthAngle::mod_analyze()
{
  for (const auto& event: collection_->getEvents()) {
    const double angle = event->Angle();
    histogram_->Fill(angle/unit::degree);
  }

  return AS_OK;
}

void HistogramXrayEventAzimuthAngle::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
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
