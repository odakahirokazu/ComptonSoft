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

#include "MakeXrayEventSpectrum.hh"

#include "XrayEvent.hh"
#include "XrayEventCollection.hh"
#include "XrayEventTreeIO.hh"
#include "AstroUnits.hh"
#include "TH1.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

MakeXrayEventSpectrum::MakeXrayEventSpectrum()
  : collectionModule_("XrayEventCollection"),
    outputFile_("image.png")
{
}

ANLStatus MakeXrayEventSpectrum::mod_define()
{
  define_parameter("collection_module", &mod_class::collectionModule_);
  define_parameter("energy_min", &mod_class::energyMin_);
  define_parameter("energy_max", &mod_class::energyMax_);
  define_parameter("num_bin", &mod_class::numBin_);
  define_parameter("output", &mod_class::outputFile_);
  
  return AS_OK;
}

ANLStatus MakeXrayEventSpectrum::mod_initialize()
{
  get_module_NC(collectionModule_, &collection_);
  const int numbin = NumBin();
  const double emin = EnergyMin();
  const double emax = EnergyMax();
  spectrum_.resize(numbin);
  totalSpectrum_.resize(numbin);

  ANLStatus status = VCSModule::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }
  mkdir();
  const std::string name = "Spectrum";
  const std::string title = "Spectrum";
  totalHistogram_ = new TH1D(name.c_str(), title.c_str(), numbin, emin, emax);

  return AS_OK;
}

ANLStatus MakeXrayEventSpectrum::mod_begin_run()
{
  resetSpectrum(spectrum_);
  resetSpectrum(totalSpectrum_);
  return AS_OK;
}

ANLStatus MakeXrayEventSpectrum::mod_analyze()
{
  for (const auto& event: collection_->getEvents()) {
    const double energy = event->SumPH();
    totalHistogram_->Fill(energy);
  }

  return AS_OK;
}

ANLStatus MakeXrayEventSpectrum::mod_end_run()
{
  return AS_OK;
}

void MakeXrayEventSpectrum::resetSpectrum(std::vector<double>& spectrum) const
{
  const int numbin = spectrum.size();
  for (int i=0; i<numbin; i++) {
    spectrum[i] = 0.0;
  }
}


void MakeXrayEventSpectrum::fillHistogram()
{
}

void MakeXrayEventSpectrum::drawOutputFiles(TCanvas* c1, std::vector<std::string>* filenames)
{
  c1->cd();
  gStyle->SetOptStat("e");
  gStyle->SetStatH(0.15);
  totalHistogram_->Draw();
  c1->SaveAs(outputFile_.c_str());
  filenames->push_back(outputFile_);
}

} /* namespace comptonsoft */
