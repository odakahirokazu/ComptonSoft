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
  : m_EventReconstruction(nullptr), m_InitialInfo(nullptr),
    m_NumBinEnergy(720), m_RangeEnergy1(0.0), m_RangeEnergy2(720.0)
{
}

ResponseMatrix::~ResponseMatrix() = default;

ANLStatus ResponseMatrix::mod_define()
{
  register_parameter(&m_NumBinEnergy, "number_of_bins");
  register_parameter(&m_RangeEnergy1, "energy_min", 1.0, "keV");
  register_parameter(&m_RangeEnergy2, "energy_max", 1.0, "keV");
  register_parameter(&m_Selections, "event_selections");
  
  return AS_OK;
}

ANLStatus ResponseMatrix::mod_initialize()
{
  get_module("EventReconstruction", &m_EventReconstruction);
  get_module_IFNC("InitialInformation", &m_InitialInfo);

  VCSModule::mod_initialize();
  mkdir();

  const size_t n = m_Selections.size();
  for (size_t i=0; i<n; i++) {
    const std::string name = (boost::format("response_%03d") % i).str();
    const std::string selection = m_Selections[i];
    const std::string title = (boost::format("input energy : output energy (%s)") % selection).str();
    TH2F* hist = new TH2F(name.c_str(), title.c_str(),
                          m_NumBinEnergy, m_RangeEnergy1, m_RangeEnergy2,
                          m_NumBinEnergy, m_RangeEnergy1, m_RangeEnergy2);
    hist->Sumw2();
    m_Responses[m_Selections[i]] = hist;
  }

  return AS_OK;
}

ANLStatus ResponseMatrix::mod_analyze()
{
  const double weight = m_InitialInfo->Weight();
  const double initialEnergy = m_InitialInfo->InitialEnergy();

  const std::vector<BasicComptonEvent_sptr> events = m_EventReconstruction->getReconstructedEvents();
  for (const auto& event: events) {
    const double energy = event->IncidentEnergy();
    const double eventWeight = event->ReconstructionFraction() * weight;
  
    for (auto& pair: m_Responses) {
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
