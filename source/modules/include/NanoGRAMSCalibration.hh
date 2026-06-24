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

#ifndef COMPTONSOFT_NanoGRAMSCalibration_H
#define COMPTONSOFT_NanoGRAMSCalibration_H 1

#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <string>

#include "NanoGRAMSCalibrationData.hh"
#include "VCSModule.hh"
#include "NanoGRAMSTPCProperty.hh"

class TFile;
class TTree;

namespace comptonsoft
{

class HitTreeIOWithInitialInfo;
class NanoGRAMSHitExtraction;

class NanoGRAMSCalibration : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSCalibration, 1.0);

public:
  NanoGRAMSCalibration();
  ~NanoGRAMSCalibration() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  void updateGainCorrectionForCurrentEvent();

  std::string hittree_file_;
  std::string gain_tp_file_;
  std::map<std::string, double> gain_tp_dict_;
  double gain_tp_value_ = 0.0;
  double gain_cache_seconds_ = 60.0;

  const NanoGRAMSHitExtraction* data_reduction_ = nullptr;
  std::unique_ptr<TFile> output_file_;
  TTree* hit_tree_ = nullptr;
  std::unique_ptr<HitTreeIOWithInitialInfo> tree_io_;
  CalibrationConfig calibration_config_;
  TestPulseGainTable gain_tp_table_;
  TPCProperty tpc_property_;
  int64_t written_events_ = 0;
  bool use_event_time_gain_ = false;
  int64_t cached_gain_time_bin_ = std::numeric_limits<int64_t>::min();
  //std::vector<double> gain_tp_array_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSCalibration_H */
