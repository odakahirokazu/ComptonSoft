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
#include <filesystem>
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
namespace fs = std::filesystem;

bool isPeripheralToken(const YAML::Node& node)
{
  if (!node.IsScalar()) {
    return false;
  }

  const std::string token = node.as<std::string>();
  return token == "peripheral";
}

LightEventSelectionMode parseLightEventSelectionMode(const std::string& mode)
{
  if (mode == "gamma_required") {
    return LightEventSelectionMode::GammaRequired;
  }
  if (mode == "veto_only") {
    return LightEventSelectionMode::VetoOnly;
  }
  if (mode == "disabled") {
    return LightEventSelectionMode::Disabled;
  }

  throw std::runtime_error(
      "light.event_selection_mode must be gamma_required, veto_only, or disabled.");
}

std::string lightEventSelectionModeName(LightEventSelectionMode mode)
{
  if (mode == LightEventSelectionMode::GammaRequired) {
    return "gamma_required";
  }
  if (mode == LightEventSelectionMode::VetoOnly) {
    return "veto_only";
  }
  if (mode == LightEventSelectionMode::Disabled) {
    return "disabled";
  }

  return "unknown";
}

std::vector<int> readDPPChannelList(const YAML::Node& node,
                                    const std::string& key)
{
  const std::vector<int> channels = node.as<std::vector<int>>();
  for (const int ch : channels) {
    if (ch < 0 || ch >= NUM_CH_DPP_MAX) {
      throw std::runtime_error(key + " contains a DPP channel outside 0-7.");
    }
  }
  return channels;
}

void printDPPChannelList(const std::string& label,
                         const std::vector<int>& channels)
{
  std::cout << label << ": [ ";
  bool first_channel = true;
  for (const int ch : channels) {
    if (first_channel) {
      std::cout << ch;
      first_channel = false;
    } else {
      std::cout << ", " << ch;
    }
  }
  std::cout << " ]" << std::endl;
}

void appendCoreExcludePixel(std::vector<int>& pixels, const YAML::Node& node)
{
  const int pix = node.as<int>();
  if (pix < 0 || pix >= NUM_CH_EACH_VATA) {
    throw std::runtime_error("core_exclude_pix contains a pixel outside 0-63.");
  }
  pixels.push_back(pix);
}

std::vector<int> readCoreExcludePixelList(const YAML::Node& node,
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
        appendCoreExcludePixel(pixels, item);
      }
    }
  } else {
    appendCoreExcludePixel(pixels, node);
  }

  std::sort(pixels.begin(), pixels.end());
  pixels.erase(std::unique(pixels.begin(), pixels.end()), pixels.end());
  return pixels;
}

void readClusteringPixelRange(Config& cfg, const YAML::Node& nodeCharge)
{
  const auto range = nodeCharge["clustering_pix_range"].as<std::vector<int>>();
  if (range.size() != 2) {
    throw std::runtime_error("charge.clustering_pix_range must contain [min, max].");
  }
  cfg.pix_min = range[0];
  cfg.pix_max = range[1];
}

double readChargeThresholdKeV(const YAML::Node& nodeCharge,
                              const std::string& key)
{
  if (nodeCharge[key]) {
    return nodeCharge[key].as<double>() * unit::keV;
  }
  throw std::runtime_error("charge." + key + " is missing.");
}

YAML::Node coreExcludePixelNode(const YAML::Node& nodeCharge)
{
  if (nodeCharge["core_exclude_pix"]) {
    return nodeCharge["core_exclude_pix"];
  }

  throw std::runtime_error("charge.core_exclude_pix is missing.");
}

void readGeneralConfig(Config& cfg, const YAML::Node& node)
{
  const auto nodeGeneral = node["general"];
  cfg.drift_time_max = nodeGeneral["drift_time_max_us"].as<double>() * unit::us;

  std::cout << "readGeneralConfig()" << std::endl;
  std::cout << "drift_time_max_us: " << cfg.drift_time_max / unit::us << std::endl;
}

void readLightConfig(Config& cfg, const YAML::Node& node)
{
  const auto nodeLight = node["light"];
  cfg.light_gamma_thr  = nodeLight["light_gamma_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.light_cosmic_thr = nodeLight["light_cosmic_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.pre_roi_window   = nodeLight["pre_roi_window_us"].as<double>() * unit::us;
  cfg.post_roi_window  = nodeLight["post_roi_window_us"].as<double>() * unit::us;
  cfg.out_roi_peak_thr = nodeLight["out_roi_peak_thr_mV"].as<double>() * (unit::volt/1000.0);
  cfg.light_transimpedance_feedback_resistance_ohm =
      nodeLight["transimpedance_feedback_resistance_ohm"].as<double>() * unit::ohm;
  cfg.light_output_impedance_ohm =
      nodeLight["output_impedance_ohm"].as<double>() * unit::ohm;
  std::cout << "konnichiwa: " << nodeLight["output_impedance_ohm"].as<double>() << std::endl;
  if (cfg.light_transimpedance_feedback_resistance_ohm <= 0.0*unit::ohm) {
    throw std::runtime_error(
        "light.transimpedance_feedback_resistance_ohm must be positive.");
  }
  if (cfg.light_output_impedance_ohm < 0.0*unit::ohm) {
    throw std::runtime_error("light.output_impedance_ohm must be non-negative.");
  }
  cfg.general_analysis_channels =
      readDPPChannelList(nodeLight["general_analysis_channels"],
                         "light.general_analysis_channels");
  cfg.pileup_analysis_channels =
      readDPPChannelList(nodeLight["pileup_analysis_channels"],
                         "light.pileup_analysis_channels");
  if (nodeLight["waveform_analysis"]) {
    cfg.light_waveform_analysis = normalizeLightWaveformAnalysis(
        nodeLight["waveform_analysis"].as<std::string>());
  }
  if (nodeLight["use_for_event_selection"]) {
    cfg.use_light_for_event_selection =
        nodeLight["use_for_event_selection"].as<bool>();
    if (cfg.use_light_for_event_selection) {
      cfg.light_event_selection_mode = LightEventSelectionMode::GammaRequired;
    } else {
      cfg.light_event_selection_mode = LightEventSelectionMode::Disabled;
    }
  }
  if (nodeLight["event_selection_mode"]) {
    cfg.light_event_selection_mode = parseLightEventSelectionMode(
        nodeLight["event_selection_mode"].as<std::string>());
  }
  if (cfg.light_event_selection_mode == LightEventSelectionMode::Disabled) {
    cfg.use_light_for_event_selection = false;
  } else {
    cfg.use_light_for_event_selection = true;
  }

  std::cout << "readLightConfig()" << std::endl;
  std::cout << "light_gamma_thr_mV:  "  << cfg.light_gamma_thr / (unit::volt/1000.0) << std::endl;
  std::cout << "light_cosmic_thr_mV:  " << cfg.light_cosmic_thr / (unit::volt/1000.0) << std::endl;
  std::cout << "pre_roi_window_us:  "   << cfg.pre_roi_window / unit::us << std::endl;
  std::cout << "post_roi_window_us: "   << cfg.post_roi_window / unit::us << std::endl;
  std::cout << "out_roi_peak_thr_mV: "  << cfg.out_roi_peak_thr  / (unit::volt/1000.0) << std::endl;
  std::cout << "transimpedance_feedback_resistance_ohm: "
            << cfg.light_transimpedance_feedback_resistance_ohm / unit::ohm<< std::endl;
  std::cout << "output_impedance_ohm: "
            << cfg.light_output_impedance_ohm / unit::ohm<< std::endl;
  std::cout << "waveform_analysis:   " << cfg.light_waveform_analysis << std::endl;
  std::cout << "event_selection_mode: "
            << lightEventSelectionModeName(cfg.light_event_selection_mode) << std::endl;
  std::cout << "use_for_event_selection: "
            << cfg.use_light_for_event_selection << std::endl;
  printDPPChannelList("general_analysis_channels",
                      cfg.general_analysis_channels);
  printDPPChannelList("pileup_analysis_channels",
                      cfg.pileup_analysis_channels);
}

void readChargeConfig(Config& cfg, const YAML::Node& node)
{
  const auto nodeCharge = node["charge"];
  readClusteringPixelRange(cfg, nodeCharge);
  cfg.core_noise_energy_th =
      readChargeThresholdKeV(nodeCharge, "noise_th_kev");
  cfg.spread_thr_energy =
      readChargeThresholdKeV(nodeCharge, "spread_thr_kev");
  if (nodeCharge["cross_fec_merge_drift_time_tolerance_us"]) {
    cfg.cross_fec_merge_drift_time_tolerance =
        nodeCharge["cross_fec_merge_drift_time_tolerance_us"].as<double>() * unit::us;
  }

  const AnodeChannelTopology topology = buildAnodeChannelTopology();
  const YAML::Node nodeCoreExcludePixel = coreExcludePixelNode(nodeCharge);
  for (const auto& item : nodeCoreExcludePixel) {
    const int fec = item.first.as<int>();
    if (fec < 0 || fec >= NUM_VATA) {
      throw std::runtime_error("core_exclude_pix contains an FEC outside 0-3.");
    }
    cfg.core_exclude_pix[fec] =
        readCoreExcludePixelList(item.second, fec, topology);
  }

  std::cout << "pix_min: "           << cfg.pix_min           << std::endl;
  std::cout << "pix_max: "           << cfg.pix_max           << std::endl;
  std::cout << "noise_th_kev: " << cfg.core_noise_energy_th / unit::keV << std::endl;
  std::cout << "spread_thr_kev: "    << cfg.spread_thr_energy / unit::keV << std::endl;
  if (cfg.cross_fec_merge_drift_time_tolerance >= 0.0) {
    std::cout << "cross_fec_merge_drift_time_tolerance_us: "
              << cfg.cross_fec_merge_drift_time_tolerance / unit::us << std::endl;
  } else {
    std::cout << "cross_fec_merge_drift_time_tolerance_us: disabled" << std::endl;
  }

  std::cout << "core_exclude_pix:" << std::endl;
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const std::vector<int> pix_vec = cfg.core_exclude_pix[fec];
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

  readGeneralConfig(cfg, configNode);
  readLightConfig(cfg, configNode);
  readChargeConfig(cfg, configNode);
}

void readDPPConfigFile(Config& cfg, const std::string& config_path)
{
  const auto configNode = YAML::LoadFile(config_path);
  const auto delayNode = configNode["savefile"]["listwave_delay"]["value"];
  const std::vector<int> delays = delayNode.as<std::vector<int>>();
  if (delays.size() != NUM_CH_DPP_MAX) {
    throw std::runtime_error(
        "savefile.listwave_delay.value in " + config_path +
        " must contain 8 DPP channel values.");
  }

  for (int ch = 0; ch < NUM_CH_DPP_MAX; ++ch) {
    if (delays[ch] < 0) {
      throw std::runtime_error(
          "savefile.listwave_delay.value contains a negative delay in " +
          config_path);
    }
    cfg.light_delay_counts[ch] = delays[ch];
  }

  std::cout << "readDPPConfig()" << std::endl;
  std::cout << "config_dpp: " << config_path << std::endl;
  std::cout << "listwave_delay: [ ";
  for (int ch = 0; ch < NUM_CH_DPP_MAX; ++ch) {
    if (ch != 0) {
      std::cout << ", ";
    }
    std::cout << cfg.light_delay_counts[ch];
  }
  std::cout << " ]" << std::endl;
}

void readDPPConfig(Config& cfg, const std::string& tpctree_file)
{
  const std::string config_path =
      (fs::path(tpctree_file).parent_path() / "config_dpp.yaml").string();
  readDPPConfigFile(cfg, config_path);
}

} /* namespace grams */
} /* namespace comptonsoft */
