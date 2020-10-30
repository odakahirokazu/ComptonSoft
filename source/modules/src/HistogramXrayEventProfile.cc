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

#include "HistogramXrayEventProfile.hh"

#include "XrayEventCollection.hh"
#include "AstroUnits.hh"
#include "TH1.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

HistogramXrayEventProfile::HistogramXrayEventProfile()
  : numBins_(100), min_(0.0), max_(5120.0), axis_(0),
    collectionModule_("XrayEventCollection"), outputName_("profile")
{
}

ANLStatus HistogramXrayEventProfile::mod_define()
{
  define_parameter("num_bins", &mod_class::numBins_);
  define_parameter("min", &mod_class::min_);
  define_parameter("max", &mod_class::max_);
  define_parameter("axis", &mod_class::axis_);
  define_parameter("collection_module", &mod_class::collectionModule_);
  define_parameter("output_name", &mod_class::outputName_);

  return AS_OK;
}

ANLStatus HistogramXrayEventProfile::mod_initialize()
{
  get_module_NC(collectionModule_, &collection_);

  ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  mkdir();
  std::string name = "profile";
  std::string title = "profile";
  if (axis_ == 0) {
    name = "ix";
    title = "ix";
  } else if (axis_ == 1) {
    name = "iy";
    title = "iy";
  }

  histogram_ = new TH1D(name.c_str(), title.c_str(),
                        numBins_, min_, max_);

  return AS_OK;
}

ANLStatus HistogramXrayEventProfile::mod_analyze()
{
  for (const auto& event: collection_->getEvents()) {
    if (axis_ == 0) {
      const int ix = event->PixelX();
      histogram_->Fill(ix);
    } else if (axis_ == 1) {
      const int ix = event->PixelY();
      histogram_->Fill(ix);
    }
  }

  return AS_OK;
}

void HistogramXrayEventProfile::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
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
