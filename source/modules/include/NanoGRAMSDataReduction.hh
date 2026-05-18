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

#ifndef COMPTONSOFT_NanoGRAMSDataReduction_H
#define COMPTONSOFT_NanoGRAMSDataReduction_H 1

#include <cstdint>
#include <memory>
#include <string>

#include "NanoGRAMSTPCTreeUtil.hh"
#include "VCSModule.hh"

class TFile;

namespace comptonsoft
{

class NanoGRAMSDataReduction : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSDataReduction, 1.0);

public:
  NanoGRAMSDataReduction();
  ~NanoGRAMSDataReduction() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  bool hasCurrentEvent() const { return current_has_event_; }
  std::int64_t currentEventId() const { return current_event_id_; }
  std::int64_t currentRawEventId() const { return current_raw_event_id_; }
  const std::vector<ngUtil::StreamingFecHit>& currentEventHits() const
  {
    return current_event_hits_;
  }
  const std::string& rawHitFilePath() const { return cfg_.output_file_path; }

private:
  std::string config_file_;
  std::string tpctree_file_;
  std::string rawhitdata_file_;

  ngUtil::Config cfg_;
  std::unique_ptr<TFile> input_file_;
  std::unique_ptr<ngUtil::StreamingProcessor> processor_;
  std::unique_ptr<ngUtil::RawHitTreeOutputWriter> writer_;
  std::int64_t gamma_events_ = 0;
  bool current_has_event_ = false;
  std::int64_t current_event_id_ = -1;
  std::int64_t current_raw_event_id_ = -1;
  std::vector<ngUtil::StreamingFecHit> current_event_hits_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSDataReduction_H */
