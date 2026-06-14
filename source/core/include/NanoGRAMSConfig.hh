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

#ifndef COMPTONSOFT_NanoGRAMSConfig_H
#define COMPTONSOFT_NanoGRAMSConfig_H 1

#include <map>
#include <string>
#include <vector>

#include "AstroUnits.hh"

namespace comptonsoft
{
namespace grams
{

enum class LightEventSelectionMode
{
  Disabled,
  GammaRequired,
  VetoOnly,
};

struct Config
{
  int daq_time      = 0;
  int delay_counts  = 0;
  int pix_min       = 0;
  int pix_max       = 0;
  int circ_min_hits = 0;

  double adc2mv           = (1.0 / 8192.0) * 1000.0;
  double adu_min          = 0.0;
  double adu_max          = 0.0;
  double light_gamma_thr  = 0.0;
  double light_cosmic_thr = 0.0;
  double circ_thr         = 0.0;
  double spread_thr       = 0.0;
  double drift_time_max   = 0.0 * anlgeant4::unit::us;
  //double late_window      = 0.0;
  //double late_peak_thr    = 0.0;
  double pre_roi_window      = 0.0;
  double post_roi_window     = 0.0;
  double out_roi_peak_thr    = 0.0;
  double noise_th            = 0.0;
  //double circ_min_ratio      = 0.0;
  double timebin_ns_override = 0.0;
  double cross_fec_merge_drift_time_tolerance = -1.0 * anlgeant4::unit::us;

  std::vector<int> light_channels = {4, 6, 5, 7};
  std::string light_waveform_analysis = "average";
  LightEventSelectionMode light_event_selection_mode = LightEventSelectionMode::GammaRequired;
  bool use_light_for_event_selection = true;
  std::map<int, std::vector<int>> core_exclude_pix;
};

void readConfig(Config& cfg, const std::string& config_path);

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSConfig_H */
