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

#include "MakeXrayEventAzimuthAngle.hh"

#include "XrayEvent.hh"
#include "XrayEventCollection.hh"
#include "XrayEventTreeIO.hh"
#include "AstroUnits.hh"
#include "TH1.h"
#include "TStyle.h"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

MakeXrayEventAzimuthAngle::MakeXrayEventAzimuthAngle()
  : collectionModule_("XrayEventCollection"),
    outputFile_("image.png")
{
}

ANLStatus MakeXrayEventAzimuthAngle::mod_define()
{
  define_parameter("collection_module", &mod_class::collectionModule_);
  define_parameter("num_bin", &mod_class::numBin_);
  define_parameter("output", &mod_class::outputFile_);
  
  return AS_OK;
}

ANLStatus MakeXrayEventAzimuthAngle::mod_initialize()
{
  get_module_NC(collectionModule_, &collection_);
  const int numbin = NumBin();
  const double amin = AngleMin();
  const double amax = AngleMax();
  distribution_.resize(numbin);
  totalDistribution_.resize(numbin);

  ANLStatus status = VCSModule::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }
  mkdir();
  const std::string name = "azimuth_angle";
  const std::string title = "Azimuth Angle";
  totalHistogram_ = new TH1D(name.c_str(), title.c_str(), numbin, amin, amax);

  return AS_OK;
}

ANLStatus MakeXrayEventAzimuthAngle::mod_begin_run()
{
  resetDistribution(distribution_);
  resetDistribution(totalDistribution_);
  return AS_OK;
}

ANLStatus MakeXrayEventAzimuthAngle::mod_analyze()
{
  for (const auto& event: collection_->getEvents()) {
    const double angle = event->Angle()/unit::degree;
    totalHistogram_->Fill(angle);
  }

  return AS_OK;
}

ANLStatus MakeXrayEventAzimuthAngle::mod_end_run()
{
  fillHistogram();
  return AS_OK;
}

void MakeXrayEventAzimuthAngle::resetDistribution(std::vector<double>& distribution) const
{
  const int numbin = distribution.size();
  for (int i=0; i<numbin; i++) {
    distribution[i] = 0.0;
  }
}


void MakeXrayEventAzimuthAngle::fillHistogram()
{
}

void MakeXrayEventAzimuthAngle::drawOutputFiles(TCanvas* c1, std::vector<std::string>* filenames)
{
  c1->cd();
  gStyle->SetOptStat("e");
  gStyle->SetStatH(0.15);
  totalHistogram_->Draw();
  c1->SaveAs(outputFile_.c_str());
  filenames->push_back(outputFile_);
}

} /* namespace comptonsoft */
