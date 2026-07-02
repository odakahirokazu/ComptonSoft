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

#include "HistogramXrayEventSpectrum.hh"

#include "XrayEventCollection.hh"
#include "AstroUnits.hh"
#include "TH1.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

HistogramXrayEventSpectrum::HistogramXrayEventSpectrum()
  : numBins_(512), energyMin_(-0.5), energyMax_(4095.5),
    collectionModule_("XrayEventCollection"), outputName_("spectrum")
{
}

ANLStatus HistogramXrayEventSpectrum::mod_define()
{
  define_parameter("num_bins", &mod_class::numBins_);
  define_parameter("energy_min", &mod_class::energyMin_);
  define_parameter("energy_max", &mod_class::energyMax_);
  define_parameter("collection_module", &mod_class::collectionModule_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}

ANLStatus HistogramXrayEventSpectrum::mod_initialize()
{
  get_module_NC(collectionModule_, &collection_);

  ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  mkdir();
  const std::string name = "spectrum";
  const std::string title = "Spectrum";
  histogram_ = new TH1D(name.c_str(), title.c_str(),
                        numBins_, energyMin_, energyMax_);

  return AS_OK;
}

ANLStatus HistogramXrayEventSpectrum::mod_analyze()
{
  for (const auto& event: collection_->getEvents()) {
    const double energy = event->SumPH();
    histogram_->Fill(energy);
  }

  return AS_OK;
}

void HistogramXrayEventSpectrum::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
{
  const std::string outputFile = outputName_+".png";
  canvas->cd();
  canvas->SetLogy();
  gStyle->SetOptStat("e");
  gStyle->SetStatH(0.15);
  histogram_->Draw();
  canvas->SaveAs(outputFile.c_str());
  filenames->push_back(outputFile);
}

} /* namespace comptonsoft */
