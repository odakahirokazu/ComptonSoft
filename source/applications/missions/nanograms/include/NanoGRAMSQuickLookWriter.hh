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

#ifndef COMPTONSOFT_NanoGRAMSQuickLookWriter_H
#define COMPTONSOFT_NanoGRAMSQuickLookWriter_H 1

#include <memory>
#include <string>
#include <vector>

#include "NanoGRAMSQuickLookTreeIO.hh"
#include "VCSModule.hh"

namespace comptonsoft {

class NanoGRAMSReadTPCEvents;
class NanoGRAMSReadDppEvents;
class RealDetectorUnitNanoGRAMS;
class NanoGRAMSLoadConfig;

/**
 * Writes the quicklook tree of the NanoGRAMS detector unit.
 * The event type is classified with the flags of the detector unit and the criteria of NanoGRAMSSelectEvents:
 *   Other (excluded core) > Gamma (a cluster survives the selection) > Cosmic > PileUp > TimeUp > Other.
 * Error events are not written since they are skipped by NanoGRAMSReadTPCEvents.
 * The event ID is taken from NanoGRAMSReadTPCEvents, or from NanoGRAMSReadDppEvents for DPP-only data
 * (then only the light branches are meaningful).
 *
 * This module should be placed after SelectHits and before NanoGRAMSSelectEvents
 * to write the events that are rejected by the selection.
 * @date 2026-09-24 | rewritten to take the data from RealDetectorUnitNanoGRAMS
 */
class NanoGRAMSQuickLookWriter : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSQuickLookWriter, 2.0);

public:
  NanoGRAMSQuickLookWriter();
  ~NanoGRAMSQuickLookWriter() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  std::vector<int> selectedClusters() const;
  // clusters rejected by their own flags (independent of the detector-level rejection)
  std::vector<int> rejectedClusters() const;
  grams::TPCEventType classifyEvent(const std::vector<int>& selected_clusters) const;
  bool shouldWrite(grams::TPCEventType event_type, const std::vector<int>& selected_clusters) const;

  std::string quicklook_file_ = "quicklook.root";
  std::vector<std::string> event_types_;
  int num_hits_ = -1;
  bool save_waveforms_ = true;
  int detector_id_ = 0;

  const NanoGRAMSReadTPCEvents* tpc_events_ = nullptr;
  const NanoGRAMSReadDppEvents* dpp_events_ = nullptr;
  const RealDetectorUnitNanoGRAMS* detector_ = nullptr;
  const NanoGRAMSLoadConfig* configLoader_ = nullptr;
  std::unique_ptr<grams::QuickLookTreeIO> writer_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSQuickLookWriter_H */
