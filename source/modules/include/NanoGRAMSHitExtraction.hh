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

#ifndef COMPTONSOFT_NanoGRAMSHitExtraction_H
#define COMPTONSOFT_NanoGRAMSHitExtraction_H 1

#include <cstdint>
#include <memory>
#include <string>

#include "NanoGRAMSTPCDataProcessor.hh"
#include "VCSModule.hh"

class TFile;

namespace comptonsoft
{

class NanoGRAMSHitExtraction : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSHitExtraction, 1.0);

public:
  NanoGRAMSHitExtraction();
  ~NanoGRAMSHitExtraction() override;

  anlnext::ANLStatus mod_define()     override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze()    override;
  anlnext::ANLStatus mod_end_run()    override;

  bool hasCurrentEvent() const { return !current_event_hits_.empty(); }
  int64_t currentEventId() const { return hasCurrentEvent() ? gamma_events_ - 1 : -1; }
  int64_t currentRawEventId() const { return current_raw_event_id_; }
  const std::vector<grams::RawFECHit>& currentEventHits() const
  {
    return current_event_hits_;
  }
  const std::string& configFilePath() const { return config_file_; }

private:
  std::string config_file_;
  std::string tpctree_file_;
  std::string rawhitdata_file_;
  std::string quicklook_file_;

  grams::Config cfg_;
  std::unique_ptr<TFile> input_file_;
  std::unique_ptr<grams::TPCTreeReader> tpc_tree_reader_;
  std::unique_ptr<grams::RawHitTreeOutputWriter> rawhit_tree_writer_;
  std::unique_ptr<grams::QuickLookTreeOutputWriter> quicklook_tree_writer_;
  int64_t gamma_events_         = 0;
  int64_t current_raw_event_id_ = -1;
  std::vector<grams::RawFECHit> current_event_hits_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSHitExtraction_H */
