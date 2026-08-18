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

namespace comptonsoft
{

class NanoGRAMSReadTPCEvents;

class NanoGRAMSQuickLookWriter : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSQuickLookWriter, 1.0);

public:
  NanoGRAMSQuickLookWriter();
  ~NanoGRAMSQuickLookWriter() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  bool shouldWrite() const;

  std::string quicklook_file_ = "quicklook.root";
  std::vector<std::string> event_types_;
  int num_hits_ = -1;
  bool save_waveforms_ = true;
  int output_flush_entries_ = 1000;

  const NanoGRAMSReadTPCEvents* tpc_events_ = nullptr;
  std::unique_ptr<grams::QuickLookTreeOutputWriter> writer_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSQuickLookWriter_H */
