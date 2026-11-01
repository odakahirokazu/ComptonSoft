/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 *************************************************************************/

#include "NanoGRAMSQuickLookWriter.hh"

#include <cmath>
#include <iostream>
#include <string>

#include "CSException.hh"
#include "DetectorHit.hh"
#include "FlagDefinition.hh"
#include "NanoGRAMSConfig.hh"
#include "NanoGRAMSQuickLookTreeIO.hh"
#include "NanoGRAMSReadDppEvents.hh"
#include "NanoGRAMSReadTPCEvents.hh"
#include "NanoGRAMSSelectEvents.hh"
#include "RealDetectorUnitNanoGRAMS.hh"

using namespace anlnext;

namespace comptonsoft {

namespace {

bool matchesEventType(const std::string& name, grams::TPCEventType event_type)
{
  return (name == "error" && event_type == grams::TPCEventType::Error) ||
         (name == "other" && event_type == grams::TPCEventType::Other) ||
         (name == "gamma" && event_type == grams::TPCEventType::Gamma) ||
         (name == "cosmic" && event_type == grams::TPCEventType::Cosmic) ||
         (name == "pileup" && event_type == grams::TPCEventType::PileUp) ||
         (name == "timeup" && event_type == grams::TPCEventType::TimeUp) ||
         (name == "excluded" && event_type == grams::TPCEventType::ExcludedCore) ||
         (name == "nocluster" && event_type == grams::TPCEventType::NoCluster) ||
         (name == "lightnotgamma" && event_type == grams::TPCEventType::LightNotGamma) ||
         (name == "rej_pixelcount" && event_type == grams::TPCEventType::RejPixelCount) ||
         (name == "rej_collinear" && event_type == grams::TPCEventType::RejCollinear) ||
         (name == "rej_multicluster" && event_type == grams::TPCEventType::RejMultiCluster) ||
         (name == "rej_timeup" && event_type == grams::TPCEventType::RejTimeUp);
}

} // namespace

NanoGRAMSQuickLookWriter::NanoGRAMSQuickLookWriter() = default;

NanoGRAMSQuickLookWriter::~NanoGRAMSQuickLookWriter() = default;

ANLStatus NanoGRAMSQuickLookWriter::mod_define()
{
  define_parameter("quicklook_file", &mod_class::quicklook_file_);
  define_parameter("detector_id", &mod_class::detector_id_);
  define_parameter("event_types", &mod_class::event_types_);
  define_parameter("num_hits", &mod_class::num_hits_);
  define_parameter("save_waveforms", &mod_class::save_waveforms_);
  return AS_OK;
}

ANLStatus NanoGRAMSQuickLookWriter::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }
  if (exist_module("NanoGRAMSReadTPCEvents")) {
    get_module("NanoGRAMSReadTPCEvents", &tpc_events_);
  }
  else if (exist_module("NanoGRAMSReadDppEvents")) {
    get_module("NanoGRAMSReadDppEvents", &dpp_events_);
  }
  else {
    std::cerr << "NanoGRAMSReadTPCEvents or NanoGRAMSReadDppEvents not found" << std::endl;
    return AS_QUIT_ERROR;
  }

  if (!exist_module("NanoGRAMSLoadConfig")) {
    std::cerr << "NanoGRAMSLoadConfig not found" << std::endl;
    return AS_QUIT_ERROR;
  }

  get_module("NanoGRAMSLoadConfig", &configLoader_);

  auto detector = getDetectorManager()->getDetectorByID(detector_id_);

  if (detector->checkType(DetectorType::NanoGRAMS)) {
    // always RealDetectorUnitNanoGRAMS
    detector_ = static_cast<RealDetectorUnitNanoGRAMS*>(detector);
  }
  if (detector_ == nullptr) {
    throw CSException("NanoGRAMSQuickLookWriter: no NanoGRAMS detector is found.");
  }
  return AS_OK;
}

ANLStatus NanoGRAMSQuickLookWriter::mod_analyze()
{
  const std::vector<int> selected_clusters = selectedClusters();
  const grams::TPCEventType event_type = classifyEvent(selected_clusters);
  if (!shouldWrite(event_type, selected_clusters)) {
    return AS_OK;
  }

  if (!writer_) {
    writer_ = std::make_unique<grams::QuickLookTreeIO>(quicklook_file_, *detector_, save_waveforms_);
  }
  const int64_t raw_event_id = tpc_events_ ? tpc_events_->currentRawEventId() : dpp_events_->currentRawEventId();
  writer_->fillEvent(raw_event_id, event_type, *detector_, selected_clusters, rejectedClusters());
  return AS_OK;
}

ANLStatus NanoGRAMSQuickLookWriter::mod_end_run()
{
  if (writer_) {
    writer_->close();
  }
  return AS_OK;
}

std::vector<int> NanoGRAMSQuickLookWriter::selectedClusters() const
{
  std::vector<int> selected;
  if (NanoGRAMSSelectEvents::isDetectorRejected(*detector_, configLoader_->config().light_event_selection_mode)) {
    return selected;
  }
  for (int i = 0; i < detector_->NumberOfReconstructedHits(); ++i) {
    if (!NanoGRAMSSelectEvents::isClusterRejected(*detector_->getReconstructedHit(i))) {
      selected.push_back(i);
    }
  }
  return selected;
}

std::vector<int> NanoGRAMSQuickLookWriter::rejectedClusters() const
{
  std::vector<int> rejected;
  for (int i = 0; i < detector_->NumberOfReconstructedHits(); ++i) {
    if (NanoGRAMSSelectEvents::isClusterRejected(*detector_->getReconstructedHit(i))) {
      rejected.push_back(i);
    }
  }
  return rejected;
}

grams::TPCEventType NanoGRAMSQuickLookWriter::classifyEvent(const std::vector<int>& selected_clusters) const
{
  if (detector_->isEventFlags(nanograms_event_flag::ExcludedCore)) {
    return grams::TPCEventType::ExcludedCore;
  }
  if (!selected_clusters.empty()) {
    return grams::TPCEventType::Gamma;
  }

  const bool usesLight = (configLoader_->config().light_event_selection_mode != grams::LightEventSelectionMode::Disabled);
  if (usesLight && detector_->isEventFlags(nanograms_event_flag::LightCosmic)) {
    return grams::TPCEventType::Cosmic;
  }
  if (usesLight && detector_->isEventFlags(nanograms_event_flag::LightPileup)) {
    return grams::TPCEventType::PileUp;
  }

  // time up: the drift times of all the FECs are over the limit (or not available)
  bool timeUp = true;
  for (int fec = 0; fec < detector_->NumberOfMultiChannelData(); ++fec) {
    const double driftTime = detector_->DriftTime(fec);
    if (std::isfinite(driftTime) && driftTime < detector_->DriftTimeLimit()) {
      timeUp = false;
      break;
    }
  }
  if (timeUp) {
    return grams::TPCEventType::TimeUp;
  }

  if (configLoader_->config().light_event_selection_mode == grams::LightEventSelectionMode::GammaRequired &&
      !detector_->isEventFlags(nanograms_event_flag::LightGamma)) {
    return grams::TPCEventType::LightNotGamma;
  }

  const int numHits = detector_->NumberOfReconstructedHits();
  if (numHits == 0) {
    return grams::TPCEventType::NoCluster;
  }

  // all the clusters are rejected: classify by the reason (first match in the priority order)
  uint64_t clusterFlags = 0;
  for (int i = 0; i < numHits; ++i) {
    clusterFlags |= detector_->getReconstructedHit(i)->Flags();
  }
  if (clusterFlags & flag::NanoGRAMSPixelCountOutOfRange) {
    return grams::TPCEventType::RejPixelCount;
  }
  if (clusterFlags & flag::NanoGRAMSCollinear) {
    return grams::TPCEventType::RejCollinear;
  }
  if (clusterFlags & flag::NanoGRAMSMultipleClustersInFEC) {
    return grams::TPCEventType::RejMultiCluster;
  }
  if (clusterFlags & flag::NanoGRAMSTimeUp) {
    return grams::TPCEventType::RejTimeUp;
  }
  return grams::TPCEventType::Other;
}

// selection of the events to be written, by the module parameters event_types and num_hits
bool NanoGRAMSQuickLookWriter::shouldWrite(grams::TPCEventType event_type,
                                           const std::vector<int>& selected_clusters) const
{
  if (num_hits_ >= 0 && static_cast<int>(selected_clusters.size()) != num_hits_) {
    return false;
  }
  if (event_types_.empty()) {
    return true;
  }
  for (const std::string& name : event_types_) {
    if (matchesEventType(name, event_type)) {
      return true;
    }
  }
  return false;
}

} /* namespace comptonsoft */
