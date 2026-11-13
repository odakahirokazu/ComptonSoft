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

#include "NanoGRAMSSelectEvents.hh"

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "CSHitCollection.hh"
#include "DetectorHit.hh"
#include "FlagDefinition.hh"
#include "NanoGRAMSConfig.hh"
#include "NanoGRAMSLoadConfig.hh"
#include "RealDetectorUnitNanoGRAMS.hh"

using namespace anlnext;

namespace comptonsoft {

namespace {

constexpr uint64_t RejectedClusterFlags = flag::NanoGRAMSTimeUp | flag::NanoGRAMSPixelCountOutOfRange |
                                          flag::NanoGRAMSCollinear | flag::NanoGRAMSMultipleClustersInFEC;

} // namespace

NanoGRAMSSelectEvents::NanoGRAMSSelectEvents() = default;

NanoGRAMSSelectEvents::~NanoGRAMSSelectEvents() = default;

ANLStatus NanoGRAMSSelectEvents::mod_define()
{
  define_parameter("reject_charge_clusters", &mod_class::rejectChargeClusters_);
  set_parameter_description("remove the charge clusters with the rejection flags (false: event-level cut only)");
  return AS_OK;
}

ANLStatus NanoGRAMSSelectEvents::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  const NanoGRAMSLoadConfig* configloader = nullptr;
  get_module("NanoGRAMSLoadConfig", &configloader);
  config_ = &configloader->config();
  get_module_NC("CSHitCollection", &hitCollection_);

  numEvents_ = 0;
  numSelectedEvents_ = 0;
  numRejectedDetectorEvents_ = 0;
  numRejectedHits_ = 0;
  return AS_OK;
}

ANLStatus NanoGRAMSSelectEvents::mod_analyze()
{
  ++numEvents_;

  // event-level judgement of each NanoGRAMS detector (detector ID -> rejected)
  std::map<int, bool> detectorRejected;
  for (auto& detector : getDetectorManager()->getDetectors()) {
    if (!detector->checkType(DetectorType::NanoGRAMS)) {
      continue;
    }
    // always RealDetectorUnitNanoGRAMS
    const auto* nanograms = static_cast<const RealDetectorUnitNanoGRAMS*>(detector.get());
    const bool rejected = isDetectorRejected(*nanograms, config_->light_event_selection_mode);
    detectorRejected[nanograms->getID()] = rejected;
    if (rejected) {
      ++numRejectedDetectorEvents_;
    }
  }

  auto isRejected = [&detectorRejected, this](const DetectorHit_sptr& hit) {
    const auto it = detectorRejected.find(hit->DetectorID());
    if (it == detectorRejected.end()) {
      return false; // not a NanoGRAMS detector
    }
    return it->second || (rejectChargeClusters_ && isClusterRejected(*hit));
  };

  std::size_t numRemainingHits = 0;
  for (int timeGroup = 0; timeGroup < hitCollection_->NumberOfTimeGroups(); ++timeGroup) {
    std::vector<DetectorHit_sptr>& hits = hitCollection_->getHits(timeGroup);
    const auto newEnd = std::remove_if(hits.begin(), hits.end(), isRejected);
    numRejectedHits_ += std::distance(newEnd, hits.end());
    hits.erase(newEnd, hits.end());
    numRemainingHits += hits.size();
  }

  if (numRemainingHits == 0) {
    return AS_SKIP;
  }

  ++numSelectedEvents_;
  return AS_OK;
}

ANLStatus NanoGRAMSSelectEvents::mod_end_run()
{
  std::cout << "[NanoGRAMSSelectEvents] reject_charge_clusters: " << (rejectChargeClusters_ ? "true" : "false") << "\n"
            << "[NanoGRAMSSelectEvents] events: " << numEvents_ << "\n"
            << "[NanoGRAMSSelectEvents] selected events: " << numSelectedEvents_ << "\n"
            << "[NanoGRAMSSelectEvents] rejected by event-level judgement (detector x event): "
            << numRejectedDetectorEvents_ << "\n"
            << "[NanoGRAMSSelectEvents] rejected hits: " << numRejectedHits_ << std::endl;
  return AS_OK;
}

bool NanoGRAMSSelectEvents::isClusterRejected(const DetectorHit& hit)
{
  return (hit.Flags() & RejectedClusterFlags) != 0;
}

bool NanoGRAMSSelectEvents::isDetectorRejected(const RealDetectorUnitNanoGRAMS& detector,
                                               grams::LightEventSelectionMode mode)
{
  if (detector.isEventFlags(nanograms_event_flag::ExcludedCore)) {
    return true;
  }

  if (mode == grams::LightEventSelectionMode::Disabled) {
    return false;
  }
  if (detector.isEventFlags(nanograms_event_flag::LightCosmic) ||
      detector.isEventFlags(nanograms_event_flag::LightPileup)) {
    return true;
  }
  if (mode == grams::LightEventSelectionMode::GammaRequired &&
      !detector.isEventFlags(nanograms_event_flag::LightGamma)) {
    return true;
  }
  return false;
}

} /* namespace comptonsoft */
