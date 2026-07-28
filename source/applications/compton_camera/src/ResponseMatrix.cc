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

#include "ResponseMatrix.hh"

#include <boost/format.hpp>
#include "TDirectory.h"
#include "TH2.h"
#include "AstroUnits.hh"
#include "BasicComptonEvent.hh"
#include "InitialInformation.hh"
#include "EventReconstruction.hh"
#include "DetectorHit.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

ResponseMatrix::ResponseMatrix()
  : event_reconstruction_(nullptr), initial_info_(nullptr),
    num_bins_(720), energy_min_(0.0), energy_max_(720.0)
{
}

ResponseMatrix::~ResponseMatrix() = default;

ANLStatus ResponseMatrix::mod_define()
{
  define_parameter("number_of_bins", &mod_class::num_bins_);
  define_parameter("energy_min", &mod_class::energy_min_, 1.0, "keV");
  define_parameter("energy_max", &mod_class::energy_max_, 1.0, "keV");
  define_parameter("event_selections", &mod_class::event_selections_);

  return AS_OK;
}

ANLStatus ResponseMatrix::mod_initialize()
{
  get_module("EventReconstruction", &event_reconstruction_);
  get_module_IFNC("InitialInformation", &initial_info_);

  VCSModule::mod_initialize();
  mkdir();

  const size_t n = event_selections_.size();
  for (size_t i=0; i<n; i++) {
    const std::string name = (boost::format("response_%03d") % i).str();
    const std::string selection = event_selections_[i];
    const std::string title = (boost::format("input energy : output energy (%s)") % selection).str();
    TH2F* hist = new TH2F(name.c_str(), title.c_str(),
                          num_bins_, energy_min_, energy_max_,
                          num_bins_, energy_min_, energy_max_);
    hist->Sumw2();
    responses_[event_selections_[i]] = hist;
  }

  return AS_OK;
}

ANLStatus ResponseMatrix::mod_analyze()
{
  const double weight = initial_info_->weight();
  const double initialEnergy = initial_info_->initial_energy();

  const std::vector<BasicComptonEvent_sptr> events = event_reconstruction_->getReconstructedEvents();
  for (const auto& event: events) {
    const double energy = event->IncidentEnergy();
    const double eventWeight = event->ReconstructionFraction() * weight;

    for (auto& pair: responses_) {
      const std::string& evsName = pair.first;
      TH2* hist = pair.second;
      if (evs(evsName)) {
        hist->Fill(initialEnergy/unit::keV, energy/unit::keV, eventWeight);
      }
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
