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

#include <array>
#include <map>
#include <string>
#include <vector>

#include "AstroUnits.hh"
#include "NanoGRAMSEvent.hh"

namespace comptonsoft
{
namespace grams
{

namespace unit = anlgeant4::unit;

enum class LightEventSelectionMode
{
  Disabled,
  GammaRequired,
  VetoOnly,
};

struct Config
{
  int daq_time      = 0;
  int pix_min       = 0;
  int pix_max       = 0;

  double adc2mv           = (1.0 / 8192.0) * 1000.0;
  double core_noise_energy_th = 0.0 * anlgeant4::unit::keV;
  double light_gamma_thr  = 0.0;
  double light_cosmic_thr = 0.0;
  double spread_thr_energy = 0.0 * anlgeant4::unit::keV;
  double drift_time_max   = 0.0 * anlgeant4::unit::us;
  //double late_window      = 0.0;
  //double late_peak_thr    = 0.0;
  double pre_roi_window      = 0.0;
  double post_roi_window     = 0.0;
  double out_roi_peak_thr    = 0.0;
  double light_transimpedance_feedback_resistance_ohm = 3200.0 * unit::ohm;
  double light_output_impedance_ohm = 50.0 * unit::ohm;
  double timebin_ns_override = 0.0;
  double cross_fec_merge_drift_time_tolerance = -1.0 * anlgeant4::unit::us;

  std::vector<int> general_analysis_channels = {4, 6, 5, 7};
  std::vector<int> pileup_analysis_channels  = {4};
  std::array<int, NUM_CH_DPP_MAX> light_delay_counts{};
  std::string light_waveform_analysis = "average";
  LightEventSelectionMode light_event_selection_mode = LightEventSelectionMode::GammaRequired;
  bool use_light_for_event_selection = true;
  std::map<int, std::vector<int>> core_exclude_pix;
};

void readConfig(Config& cfg, const std::string& config_path);
void readDPPConfig(Config& cfg, const std::string& tpctree_file);

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSConfig_H */
