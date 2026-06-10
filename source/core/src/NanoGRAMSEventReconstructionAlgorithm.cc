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

#include "NanoGRAMSEventReconstructionAlgorithm.hh"
#include <algorithm>
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "ComptonConstraints.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

using namespace compton_constraints;

NanoGRAMSEventReconstructionAlgorithm::NanoGRAMSEventReconstructionAlgorithm()
    : is_escape_event_(true)
{
}

NanoGRAMSEventReconstructionAlgorithm::~NanoGRAMSEventReconstructionAlgorithm() = default;

bool NanoGRAMSEventReconstructionAlgorithm::loadParameters(boost::property_tree::ptree& pt)
{
    incident_energy_candidates_.clear();

    for (const auto& item : pt.get_child("incident_energy_candidates")) {
        incident_energy_candidates_.push_back(item.second.get_value<double>() * unit::keV);
    }

    std::cout << "incident_energy_candidates:" << std::endl;;
    for (double e : incident_energy_candidates_) {
        std::cout << e / unit::keV << " keV" << std::endl;
    }
    std::cout << std::endl;;

    return true;
}

#if CS_USE_YAMLCPP
bool NanoGRAMSEventReconstructionAlgorithm::loadParametersYAML(YAML::Node& node)
{
    incident_energy_candidates_.clear();
    std::vector<double> energy_values = node["incident_energy_candidates"].as<std::vector<double>>();

    for (const auto& energy_keV : energy_values) {
        incident_energy_candidates_.push_back(energy_keV * unit::keV);
    }

    std::cout << "incident_energy_candidates:" << std::endl;;
    for (double e : incident_energy_candidates_) {
        std::cout << e / unit::keV << " keV" << std::endl;
    }
    std::cout << std::endl;;

    return true;
}
#endif /* CS_USE_YAMLCPP */

void NanoGRAMSEventReconstructionAlgorithm::initializeEvent()
{
}

bool NanoGRAMSEventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  setTotalEnergyDepositsAndNumHits(hits);

  bool result = false;
  if (num_hits_ != 2) {
      return false;
  } else{
      result = reconstruct2HitEvent(hits, baseEvent, eventsReconstructed);
  }

  if (result == true) {
    if(selecting_most_likely_order_){
        selectLeastARMOrder(eventsReconstructed);
    }
    return true;
  } else{
    return false;
  }
}


void NanoGRAMSEventReconstructionAlgorithm::selectLeastARMOrder(std::vector<BasicComptonEvent_sptr>& eventsReconstructed){
  using event_t = BasicComptonEvent_sptr;
  const event_t event_yielding_least_arm =
    *std::min_element(eventsReconstructed.begin(),
                      eventsReconstructed.end(),
                      [](const event_t& a, const event_t& b) {
                        return std::abs(a->DeltaTheta()) < std::abs(b->DeltaTheta());
                      });
  eventsReconstructed.clear();
  eventsReconstructed.push_back(event_yielding_least_arm);
}

bool NanoGRAMSEventReconstructionAlgorithm::
reconstruct2HitEvent(const std::vector<DetectorHit_sptr>& hits,
                     const BasicComptonEvent& baseEvent,
                     std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  bool result = false;

  std::vector<int> scattering_order(num_hits_);
  for (int i_hit = 0; i_hit < num_hits_; ++i_hit){
    scattering_order[i_hit] = i_hit;
  }

  int reconstructedOrder = 0;
  do {
    auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);

    std::vector<DetectorHit_sptr> ordered_hits(num_hits_);
    for(int i_hit=0; i_hit<num_hits_; ++i_hit){
      ordered_hits[i_hit] = hits[ scattering_order[i_hit] ];
    }

    for(const auto& e0:incident_energy_candidates_){
        bool this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, e0);
        if (this_result) {
          eventReconstructed->setReconstructedOrder(reconstructedOrder);
          eventsReconstructed.push_back(eventReconstructed);
          result = this_result;
        }
    }
        
    ++reconstructedOrder;
  } while (std::next_permutation(scattering_order.begin(), scattering_order.end()));

  return result;
}



bool NanoGRAMSEventReconstructionAlgorithm::
reconstructOrderedHits(const std::vector<DetectorHit_sptr>& ordered_hits,
                       BasicComptonEvent& eventReconstructed,
                       double incident_energy)
{
  bool result_kinematics = isSatisfyKinematics(ordered_hits, incident_energy);
  if(!result_kinematics){
      return false;
  }

  eventReconstructed.setHit1(0, ordered_hits[0]);
  eventReconstructed.setHit2(1, ordered_hits[1]);
  eventReconstructed.setHit2Energy(incident_energy - ordered_hits[0]->Energy());
  eventReconstructed.setNumberOfHits(num_hits_);
  eventReconstructed.setEscapeFlag(is_escape_event_);
  eventReconstructed.setTotalEnergyDeposit(total_energy_deposits_);

  return true;
}

void NanoGRAMSEventReconstructionAlgorithm::setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits)
{
  total_energy_deposits_ = total_energy_deposits(hits);
  num_hits_ = hits.size();
}


bool NanoGRAMSEventReconstructionAlgorithm::isSatisfyKinematics(const std::vector<DetectorHit_sptr>& ordered_hits, double incident_energy)
{
    double cosThetaK = 1.0 - CLHEP::electron_mass_c2 * (1.0/(incident_energy-ordered_hits[0]->Energy()) - 1.0/incident_energy);
    return (-1.0 < cosThetaK) && (cosThetaK < 1.0);
}


} /* namespace comptonsoft */
