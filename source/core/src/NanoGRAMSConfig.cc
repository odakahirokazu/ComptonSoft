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

#include "NanoGRAMSConfig.hh"

#include "NanoGRAMSFECGeometry.hh"
#include "NanoGRAMSLightAnalysis.hh"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

namespace comptonsoft
{
namespace grams
{
namespace
{

namespace unit = anlgeant4::unit;

bool isPeripheralToken(const YAML::Node& node)
{
  if (!node.IsScalar()) {
    return false;
  }

  const std::string token = node.as<std::string>();
  return token == "peripheral" || token == "periphery";
}

void appendExcludePixel(std::vector<int>& pixels, const YAML::Node& node)
{
  const int pix = node.as<int>();
  if (pix < 0 || pix >= NUM_CH_EACH_VATA) {
    throw std::runtime_error("exclude_pix contains a pixel outside 0-63.");
  }
  pixels.push_back(pix);
}

std::vector<int> readExcludePixelList(const YAML::Node& node,
                                      int fec,
                                      const AnodeChannelTopology& topology)
{
  std::vector<int> pixels;

  if (isPeripheralToken(node)) {
    pixels = topology.periphery[fec];
  } else if (node.IsSequence()) {
    for (const auto& item : node) {
      if (isPeripheralToken(item)) {
        pixels.insert(pixels.end(),
                      topology.periphery[fec].begin(),
                      topology.periphery[fec].end());
      } else {
        appendExcludePixel(pixels, item);
      }
    }
  } else {
    appendExcludePixel(pixels, node);
  }

  std::sort(pixels.begin(), pixels.end());
  pixels.erase(std::unique(pixels.begin(), pixels.end()), pixels.end());
  return pixels;
}

void readLightConfig(Config& cfg, const YAML::Node& node)
{
  const auto nodeLight = node["light"];
  cfg.delay_counts     = nodeLight["delay_counts"].as<int>();
  cfg.light_gamma_thr  = nodeLight["light_gamma_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.light_cosmic_thr = nodeLight["light_cosmic_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.pre_roi_window   = nodeLight["pre_roi_window_us"].as<double>() * unit::us;
  cfg.post_roi_window  = nodeLight["post_roi_window_us"].as<double>() * unit::us;
  cfg.pre_roi_peak_thr = nodeLight["pre_roi_peak_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.post_roi_peak_thr = nodeLight["post_roi_peak_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.light_channels   = nodeLight["light_channels"].as<std::vector<int>>();
  cfg.light_waveform_analysis = normalizeLightWaveformAnalysis(
      nodeLight["waveform_analysis"] ?
      nodeLight["waveform_analysis"].as<std::string>() :
      cfg.light_waveform_analysis);

  std::cout << "readLightConfig()" << std::endl;
  std::cout << "delay_counts:       " << cfg.delay_counts << std::endl;
  std::cout << "light_gamma_thr_mV:  "  << cfg.light_gamma_thr / (unit::volt/1000.0) << std::endl;
  std::cout << "light_cosmic_thr_mV:  " << cfg.light_cosmic_thr / (unit::volt/1000.0) << std::endl;
  std::cout << "pre_roi_window_us:  "   << cfg.pre_roi_window / unit::us << std::endl;
  std::cout << "post_roi_window_us: "   << cfg.post_roi_window / unit::us << std::endl;
  std::cout << "pre_roi_peak_thr_mV:  " << cfg.pre_roi_peak_thr  / (unit::volt/1000.0) << std::endl;
  std::cout << "post_roi_peak_thr_mV: " << cfg.post_roi_peak_thr  / (unit::volt/1000.0) << std::endl;
  std::cout << "waveform_analysis:   " << cfg.light_waveform_analysis << std::endl;

  std::cout << "light_channels: [ ";
  bool first_light_channel = true;
  for (const int light_ch : cfg.light_channels) {
    if (first_light_channel) {
      std::cout << light_ch;
      first_light_channel = false;
    } else {
      std::cout << ", " << light_ch;
    }
  }
  std::cout << " ]" << std::endl;
}

void readChargeConfig(Config& cfg, const YAML::Node& node)
{
  const auto nodeCharge = node["charge"];
  cfg.pix_min           = nodeCharge["pix_min"].as<int>();
  cfg.pix_max           = nodeCharge["pix_max"].as<int>();
  cfg.circ_min_hits     = nodeCharge["circ_min_hits"].as<int>();
  cfg.adu_min           = nodeCharge["adu_min"].as<double>();
  cfg.adu_max           = nodeCharge["adu_max"].as<double>();
  cfg.circ_thr          = nodeCharge["circ_thr"].as<double>();
  cfg.spread_thr        = nodeCharge["spread_thr"].as<double>();
  cfg.drift_time_max    = nodeCharge["drift_time_max_us"].as<double>() * unit::us;
  cfg.noise_th          = nodeCharge["noise_th"].as<double>();
  cfg.circ_min_ratio    = nodeCharge["circ_min_ratio"].as<double>();
  if (nodeCharge["cross_fec_merge_drift_time_tolerance_us"]) {
    cfg.cross_fec_merge_drift_time_tolerance =
        nodeCharge["cross_fec_merge_drift_time_tolerance_us"].as<double>() * unit::us;
  }

  const AnodeChannelTopology topology = buildAnodeChannelTopology();
  for (const auto& item : nodeCharge["exclude_pix"]) {
    const int fec = item.first.as<int>();
    if (fec < 0 || fec >= NUM_VATA) {
      throw std::runtime_error("exclude_pix contains an FEC outside 0-3.");
    }
    cfg.exclude_pix[fec] = readExcludePixelList(item.second, fec, topology);
  }

  std::cout << "pix_min: "           << cfg.pix_min           << std::endl;
  std::cout << "pix_max: "           << cfg.pix_max           << std::endl;
  std::cout << "circ_min_hits: "     << cfg.circ_min_hits     << std::endl;
  std::cout << "adu_min: "           << cfg.adu_min           << std::endl;
  std::cout << "adu_max: "           << cfg.adu_max           << std::endl;
  std::cout << "circ_thr: "          << cfg.circ_thr          << std::endl;
  std::cout << "spread_thr: "        << cfg.spread_thr        << std::endl;
  std::cout << "drift_time_max_us: " << cfg.drift_time_max / unit::us << std::endl;
  std::cout << "noise_th: "          << cfg.noise_th          << std::endl;
  std::cout << "circ_min_ratio: "    << cfg.circ_min_ratio    << std::endl;
  if (cfg.cross_fec_merge_drift_time_tolerance >= 0.0) {
    std::cout << "cross_fec_merge_drift_time_tolerance_us: "
              << cfg.cross_fec_merge_drift_time_tolerance / unit::us << std::endl;
  } else {
    std::cout << "cross_fec_merge_drift_time_tolerance_us: disabled" << std::endl;
  }

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const std::vector<int> pix_vec = cfg.exclude_pix[fec];
    std::cout << fec << ": [ ";
    int index = 0;
    for (const auto& pix : pix_vec) {
      if (index == 0) {
        std::cout << pix;
      } else {
        std::cout << ", " << pix;
      }
      ++index;
    }
    std::cout << " ]" << std::endl;
  }
}

} // namespace

void readConfig(Config& cfg, const std::string& config_path)
{
  const auto configNode = YAML::LoadFile(config_path);

  readLightConfig(cfg, configNode);
  readChargeConfig(cfg, configNode);
}

} /* namespace grams */
} /* namespace comptonsoft */
