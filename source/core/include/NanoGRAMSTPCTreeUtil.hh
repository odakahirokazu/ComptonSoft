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

/**
 * @file NanoGRAMSTPCTreeUtil.hh
 * @brief Public utility API for NanoGRAMS data reduction.
 * @author Satoshi Takashima
 * @date 2026-05-17
 */

#ifndef COMPTONSOFT_NanoGRAMSTPCTreeUtil_H
#define COMPTONSOFT_NanoGRAMSTPCTreeUtil_H 1

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

class TTree;

namespace comptonsoft
{
namespace ngUtil
{

constexpr int kDefaultFecNum = 4;
constexpr int kNPixel = 64;

struct Config
{
  std::string file_path;
  std::string output_file_path;
  std::string tree_name = "tpctree";
  std::string output_tree_name = "rawvata";
  std::string error_flag_branch = "error_flag";

  int fec_num = kDefaultFecNum;
  int daq_time = 60;
  int delay_counts = 60;
  int downsample_every = 1;
  int pix_min = 1;
  int pix_max = 3;
  int circ_min_hits = 4;

  double adc2mv = (1.0 / 8192.0) * 1000.0;
  double adc_min = 18.0;
  double adc_max = 1000.0;
  double light_peak_thr_mV = 250.0;
  double circ_thr = 10.0;
  double spread_thr = 7.0;
  double drift_time_max_us = 150.0;
  double late_window_us = 5.0;
  double late_peak_thr_mV = 15.0;
  double noise_th = 20.0;
  double circ_min_ratio = 0.2;
  double timebin_ns_override = 0.0;

  std::string delay_units = "samples_x8";
  std::vector<int> light_channels = {4, 6, 5, 7};
  std::map<int, std::vector<int>> exclude_pix;
};

struct StreamingFecHit
{
  int fec = 0;
  std::uint64_t ti = 0;
  double drift_us = 0.0;
  std::vector<std::int16_t> channels;
  std::vector<float> adcs;
};

void readConfig(Config& cfg, const std::string& config_path);

class StreamingProcessor
{
public:
  StreamingProcessor(TTree* tree, const Config& cfg);
  ~StreamingProcessor();

  bool hasErrorBranch() const;
  const std::string& errorBranchName() const;
  bool processNext(std::int64_t& raw_event_id, std::vector<StreamingFecHit>& event_hits);

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

class RawHitTreeOutputWriter
{
public:
  explicit RawHitTreeOutputWriter(const Config& cfg);
  ~RawHitTreeOutputWriter();

  void fillEvent(std::int64_t event_id,
                 std::int64_t raw_event_id,
                 const std::vector<StreamingFecHit>& hits);
  std::string close();

private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} /* namespace ngUtil */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSTPCTreeUtil_H */
