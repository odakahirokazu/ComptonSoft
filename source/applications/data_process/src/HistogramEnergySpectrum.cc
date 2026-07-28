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

#include "HistogramEnergySpectrum.hh"

#include <boost/format.hpp>
#include "TDirectory.h"
#include "TH1.h"
#include "AstroUnits.hh"
#include "CSHitCollection.hh"
#include "InitialInformation.hh"
#include "DetectorHit.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

HistogramEnergySpectrum::HistogramEnergySpectrum()
  : hit_collection_(0), initial_info_(0), energy_bin_type_("lin"),
    num_bins_(720), energy_min_(0.0), energy_max_(720.0)
{
}

ANLStatus HistogramEnergySpectrum::mod_define()
{
  define_parameter("bin_type", &mod_class::energy_bin_type_);
  define_parameter("number_of_bins", &mod_class::num_bins_);
  define_parameter("energy_min", &mod_class::energy_min_, 1, "keV");
  define_parameter("energy_max", &mod_class::energy_max_, 1, "keV");
  define_parameter("event_selections", &mod_class::event_selections_);

  return AS_OK;
}

ANLStatus HistogramEnergySpectrum::mod_initialize()
{
  get_module_NC("CSHitCollection", &hit_collection_);
  get_module_IFNC("InitialInformation", &initial_info_);

  VCSModule::mod_initialize();
  mkdir();

  std::vector<double> xs(num_bins_+1);
  if (energy_bin_type_=="log") {
    const double width = std::log(energy_max_-energy_min_)/num_bins_;
    for (size_t k=0; k<xs.size(); k++) {
      xs[k] = energy_min_ * std::exp( k*width );
    }
  }

  const size_t n = event_selections_.size();
  for (size_t i=0; i<n; i++) {
    const std::string name = (boost::format("spectrum_%03d") % i).str();
    const std::string selection = event_selections_[i];
    const std::string title = (boost::format("Spectrum [%s]") % selection).str();
    TH1F* hist = nullptr;
    if (energy_bin_type_=="log") {
      hist = new TH1F(name.c_str(), title.c_str(),
                      num_bins_, &xs[0]);
    }
    else {
      hist = new TH1F(name.c_str(), title.c_str(),
                      num_bins_, energy_min_, energy_max_);
    }
    hist->Sumw2();
    histograms_[event_selections_[i]] = hist;
  }

  return AS_OK;
}

ANLStatus HistogramEnergySpectrum::mod_analyze()
{
  typedef std::vector<DetectorHit_sptr> HitVector;

  const double weight = initial_info_->weight();

  HitVector& hitVec = hit_collection_->getHits();

  double energy = 0.0;
  for (HitVector::iterator it=hitVec.begin(); it!=hitVec.end(); ++it) {
    energy += (*it)->Energy();
  }

  for (std::map<std::string, TH1*>::iterator it=histograms_.begin(); it!=histograms_.end(); ++it) {
    const std::string& evsName = (*it).first;
    TH1* hist = (*it).second;
    if (evs(evsName)) {
      hist->Fill(energy/unit::keV, weight);
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
