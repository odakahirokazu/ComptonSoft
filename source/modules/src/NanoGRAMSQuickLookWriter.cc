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

#include "NanoGRAMSReadTPCEvents.hh"

using namespace anlnext;

namespace comptonsoft
{

namespace
{

bool matchesEventType(const std::string& name, grams::TPCEventType event_type)
{
  return (name == "error" && event_type == grams::TPCEventType::Error)
      || (name == "other" && event_type == grams::TPCEventType::Other)
      || (name == "gamma" && event_type == grams::TPCEventType::Gamma)
      || (name == "cosmic" && event_type == grams::TPCEventType::Cosmic)
      || (name == "pileup" && event_type == grams::TPCEventType::PileUp)
      || (name == "timeup" && event_type == grams::TPCEventType::TimeUp);
}

} // namespace

NanoGRAMSQuickLookWriter::NanoGRAMSQuickLookWriter() = default;

NanoGRAMSQuickLookWriter::~NanoGRAMSQuickLookWriter() = default;

ANLStatus NanoGRAMSQuickLookWriter::mod_define()
{
  define_parameter("quicklook_file", &mod_class::quicklook_file_);
  define_parameter("event_types", &mod_class::event_types_);
  define_parameter("num_hits", &mod_class::num_hits_);
  define_parameter("save_waveforms", &mod_class::save_waveforms_);
  define_parameter("output_flush_entries", &mod_class::output_flush_entries_);
  return AS_OK;
}

ANLStatus NanoGRAMSQuickLookWriter::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }
  if (!exist_module("NanoGRAMSReadTPCEvents")) {
    return AS_QUIT_ERROR;
  }
  get_module("NanoGRAMSReadTPCEvents", &tpc_events_);
  return AS_OK;
}

ANLStatus NanoGRAMSQuickLookWriter::mod_analyze()
{
  if (!shouldWrite()) {
    return AS_OK;
  }

  if (!writer_) {
    writer_ = std::make_unique<grams::QuickLookTreeOutputWriter>(
        quicklook_file_,
        tpc_events_->currentTPCBuffer(),
        tpc_events_->tpcProperty(),
        save_waveforms_,
        output_flush_entries_);
  }
  writer_->fillEvent(tpc_events_->currentRawEventId(),
                     tpc_events_->currentEventType(),
                     tpc_events_->currentTPCBuffer(),
                     tpc_events_->currentEventHits());
  return AS_OK;
}

ANLStatus NanoGRAMSQuickLookWriter::mod_end_run()
{
  if (writer_) {
    writer_->close();
  }
  return AS_OK;
}

bool NanoGRAMSQuickLookWriter::shouldWrite() const
{
  const auto& hits = tpc_events_->currentEventHits();
  if (num_hits_ >= 0 && static_cast<int>(hits.size()) != num_hits_) {
    return false;
  }
  if (event_types_.empty()) {
    return true;
  }
  for (const std::string& name : event_types_) {
    if (matchesEventType(name, tpc_events_->currentEventType())) {
      return true;
    }
  }
  return false;
}

} /* namespace comptonsoft */
