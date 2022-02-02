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

#include "HistogramDecodedImage.hh"

#include "XrayEventCollection.hh"
#include "AstroUnits.hh"
#include "TH2.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

HistogramDecodedImage::HistogramDecodedImage()
  : outputName_("decoded_image_sum")
{
}

ANLStatus HistogramDecodedImage::mod_define()
{
  define_parameter("decoding_module_names", &mod_class::decodingModuleNames_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}

ANLStatus HistogramDecodedImage::mod_initialize()
{
  modules_.resize(decodingModuleNames_.size());
  for (size_t i=0; i<decodingModuleNames_.size(); i++){
    get_module_NC(decodingModuleNames_[i], &modules_[i]);  
  }

  ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  mkdir();
  histogram_ = (TH2D*)modules_[0]->getDecodedImageHistogram()->Clone();
  histogram_->Reset();

  return AS_OK;
}

ANLStatus HistogramDecodedImage::mod_end_run()
{
  sumImages();
  return AS_OK;
}

void HistogramDecodedImage::sumImages()
{
  histogram_->Reset();
  for (size_t i=0; i<modules_.size(); i++){
    histogram_->Add(modules_[i]->getDecodedImageHistogram());
  }
}

void HistogramDecodedImage::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
{
  sumImages();

  const std::string outputFile = outputName_+".png";
  canvas->cd();
  gStyle->SetOptStat(0);
  gStyle->SetPalette(56);
  const double zmax = histogram_->GetBinContent(histogram_->GetMaximumBin());
  const double zmin = 0;
  histogram_->GetZaxis()->SetRangeUser(zmin, zmax);
  histogram_->Draw("colz");
  canvas->SaveAs(outputFile.c_str());
  filenames->push_back(outputFile);
}

} /* namespace comptonsoft */
