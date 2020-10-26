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

#include "HistogramXrayEventWeight.hh"

#include "XrayEventCollection.hh"
#include "AstroUnits.hh"
#include "TH1.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

HistogramXrayEventWeight::HistogramXrayEventWeight()
  : numBins_(31), weightMin_(-0.5), weightMax_(30.5),
    collectionModule_("XrayEventCollection"), outputName_("weight")
{
}

ANLStatus HistogramXrayEventWeight::mod_define()
{
  define_parameter("num_bins", &mod_class::numBins_);
  define_parameter("weight_min", &mod_class::weightMin_);
  define_parameter("weight_max", &mod_class::weightMax_);
  define_parameter("collection_module", &mod_class::collectionModule_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}

ANLStatus HistogramXrayEventWeight::mod_initialize()
{
  get_module_NC(collectionModule_, &collection_);

  ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  mkdir();
  const std::string name = "weight";
  const std::string title = "Weight";
  histogram_ = new TH1D(name.c_str(), title.c_str(),
                        numBins_, weightMin_, weightMax_);

  return AS_OK;
}

ANLStatus HistogramXrayEventWeight::mod_analyze()
{
  for (const auto& event: collection_->getEvents()) {
    const double weight = event->Weight();
    histogram_->Fill(weight);
  }

  return AS_OK;
}

void HistogramXrayEventWeight::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
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
