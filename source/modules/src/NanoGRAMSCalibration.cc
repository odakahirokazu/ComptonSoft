/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                    *
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

#include "NanoGRAMSCalibration.hh"

#include "NanoGRAMSHitExtraction.hh"

#include <TFile.h>
#include <TTree.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "HitTreeIOWithInitialInfo.hh"

using namespace anlnext;

namespace comptonsoft
{

namespace unit = anlgeant4::unit;
namespace fs = std::filesystem;

namespace
{

constexpr double kPixelSize = 0.32 * unit::cm;

using PositionTable   = std::array<std::array<double, NUM_CH_EACH_VATA>, NUM_VATA>;
using PixelIndexTable = std::array<int16_t, NUM_CH_EACH_VATA>;

constexpr PositionTable buildPosX()
{
  PositionTable dict{};
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      if (fec == 0) {
        constexpr double offset_x = -1.28 * unit::cm;
        dict[fec][ch] = offset_x + kPixelSize * (ch / 8 - 3.5);
      } else if (fec == 1) {
        constexpr double offset_x = 1.28 * unit::cm;
        if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
          dict[fec][ch] = offset_x + kPixelSize * ((ch % 8) - 3.5);
        } else {
          dict[fec][ch] = offset_x + kPixelSize * (3.5 - (ch % 8));
        }
      } else if (fec == 2) {
        constexpr double offset_x = 1.28 * unit::cm;
        dict[fec][ch] = offset_x + kPixelSize * (3.5 - ch / 8);
      } else {
        constexpr double offset_x = -1.28 * unit::cm;
        if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
          dict[fec][ch] = offset_x + kPixelSize * (3.5 - (ch % 8));
        } else {
          dict[fec][ch] = offset_x + kPixelSize * ((ch % 8) - 3.5);
        }
      }
    }
  }
  return dict;
}

constexpr PositionTable buildPosY()
{
  PositionTable dict{};
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      if (fec == 0) {
        constexpr double offset_y = -1.28 * unit::cm;
        if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
          dict[fec][ch] = offset_y + kPixelSize * (3.5 - (ch % 8));
        } else {
          dict[fec][ch] = offset_y + kPixelSize * ((ch % 8) - 3.5);
        }
      } else if (fec == 1) {
        constexpr double offset_y = -1.28 * unit::cm;
        dict[fec][ch] = offset_y + kPixelSize * (ch / 8 - 3.5);
      } else if (fec == 2) {
        constexpr double offset_y = 1.28 * unit::cm;
        if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
          dict[fec][ch] = offset_y + kPixelSize * ((ch % 8) - 3.5);
        } else {
          dict[fec][ch] = offset_y + kPixelSize * (3.5 - (ch % 8));
        }
      } else {
        constexpr double offset_y = 1.28 * unit::cm;
        dict[fec][ch] = offset_y + kPixelSize * (3.5 - ch / 8);
      }
    }
  }
  return dict;
}

constexpr PixelIndexTable buildPixelX()
{
  PixelIndexTable dict{};
  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    dict[ch] = static_cast<int16_t>(ch / 8);
  }
  return dict;
}

constexpr PixelIndexTable buildPixelY()
{
  PixelIndexTable dict{};
  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
      dict[ch] = static_cast<int16_t>(7 - (ch % 8));
    } else {
      dict[ch] = static_cast<int16_t>(ch % 8);
    }
  }
  return dict;
}

const PositionTable kPosX     = buildPosX();
const PositionTable kPosY     = buildPosY();
const PixelIndexTable kPixelX = buildPixelX();
const PixelIndexTable kPixelY = buildPixelY();

std::filesystem::path prepareOutputPath(const std::string& output_file_path)
{
  if (output_file_path.empty()) {
    throw std::runtime_error("Hit tree output file path is empty.");
  }

  fs::path output_path(output_file_path);
  if (!output_path.parent_path().empty()) {
    fs::create_directories(output_path.parent_path());
  }
  return output_path;
}

fs::path resolvePath(const fs::path& base_dir, const std::string& value)
{
  fs::path path(value);
  if (path.is_absolute()) {
    return path;
  }
  return base_dir / path;
}

CalibrationConfig readCalibrationConfig(const std::string& config_file)
{
  const auto config_node = YAML::LoadFile(config_file);
  const auto node_calibration = config_node["calibration"];

  CalibrationConfig cfg;
  cfg.config_dir = fs::absolute(fs::path(config_file)).parent_path();

  const auto node_energy   = node_calibration["energy"];
  const auto node_position = node_calibration["position"];

  cfg.energy.gain_info_file       = node_energy["gain_info_file"].as<std::string>();
  cfg.energy.q_to_kev_spline_file = node_energy["q_to_kev_spline_file"].as<std::string>();
  cfg.energy.factor_energy        = node_energy["factor_energy"].as<double>();
  cfg.energy.max_time             = node_energy["max_time_us"].as<double>() * unit::us;
  cfg.energy.tp_channel           = node_energy["tp_channel"].as<int>();
  cfg.energy.ccal                 = node_energy["ccal"].as<int>();

  cfg.general.efield       = config_node["general"]["efield_v_cm"].as<int>() * unit::volt / unit::cm;
  cfg.general.temperature  = config_node["general"]["temperature_k"].as<double>() * unit::kelvin;
  cfg.position.anode_pos_z = node_position["anode_pos_z_cm"].as<double>() * unit::cm;

  std::cout << "gain_info_file: "       << cfg.energy.gain_info_file << std::endl;
  std::cout << "q_to_kev_spline_file: " << cfg.energy.q_to_kev_spline_file << std::endl;
  std::cout << "efield_v_cm: "          << cfg.general.efield / (unit::volt/unit::cm)  << std::endl;
  std::cout << "temperature_k: "        << cfg.general.temperature / unit::kelvin<< std::endl;
  std::cout << "factor_energy: "        << cfg.energy.factor_energy  << std::endl;
  std::cout << "max_time_us: "          << cfg.energy.max_time / unit::us    << std::endl;
  std::cout << "tp_channel: "           << cfg.energy.tp_channel     << std::endl;
  std::cout << "ccal: "                 << cfg.energy.ccal           << std::endl;

  std::cout << "anode_pos_z_cm: "       << cfg.position.anode_pos_z / unit::cm << std::endl;


  return cfg;
}

double electronDriftVelocity(double temperature, double e_field)
{
  const double temperature_nodim = temperature / unit::kelvin;
  const double efield_nodim = e_field / (1000.0 * unit::volt / unit::cm);

  constexpr double p1 = -0.01481;
  constexpr double p2 = -0.0075;
  constexpr double p3 = 0.141;
  constexpr double p4 = 12.4;
  constexpr double p5 = 1.627;
  constexpr double p6 = 0.317;
  constexpr double t0 = 90.371;
  const double vd_nodim =
      (p1 * (temperature_nodim - t0) + 1.0) *
          (p3 * efield_nodim * std::log(1.0 + p4 / efield_nodim) +
           p5 * std::pow(efield_nodim, p6)) +
      p2 * (temperature_nodim - t0);

  return vd_nodim * unit::mm / unit::us;
}

std::vector<DetectorHit_sptr> buildCalibratedHits(
    const CalibrationConfig& config,
    const TPCProperty& tpc_property,
    const std::vector<grams::RawFECHit>& raw_hits)
{
  std::vector<DetectorHit_sptr> hits;
  hits.reserve(raw_hits.size());

  const double max_time = config.energy.max_time;

  for (const auto& raw_hit : raw_hits) {
    if (raw_hit.fec < 0 || raw_hit.fec >= NUM_VATA) {
      throw std::runtime_error("FEC index out of range in NanoGRAMS calibration.");
    }

    const std::size_t n = std::min(raw_hit.channels.size(), raw_hit.adus.size());
    if (n == 0) {
      continue;
    }

    double total_energy   = 0.0 * unit::keV;
    double posx           = 0.0 * unit::cm;
    double posy           = 0.0 * unit::cm;
    double posz           = 0.0 * unit::cm;
    std::size_t max_index = 0;
    double max_energy     = -std::numeric_limits<double>::infinity() * unit::keV;

    std::vector<double> energies(n, 0.0);
    std::vector<int> channel_fecs(n, raw_hit.fec);
    for (std::size_t i = 0; i < n; ++i) {
      const int fec = i < raw_hit.channel_fecs.size() ? raw_hit.channel_fecs[i] :
                                                     raw_hit.fec;
      const int ch = raw_hit.channels[i];
      if (fec < 0 || fec >= NUM_VATA) {
        throw std::runtime_error("FEC index out of range in NanoGRAMS calibration.");
      }
      if (ch < 0 || ch >= NUM_CH_EACH_VATA) {
        throw std::runtime_error("Channel index out of range in NanoGRAMS calibration.");
      }

      const double corrected_adu =
          static_cast<double>(raw_hit.adus[i]) *
          tpc_property.temperatureCorrectionFactor(fec);
      const double energy =
          tpc_property.convertADC2keVWithSpline3D(fec, ch, corrected_adu);
      energies[i]   = energy;
      channel_fecs[i] = fec;
      total_energy += energy;

      if (energy > max_energy) {
        max_energy = energy;
        max_index  = i;
      }
    }

    if (!(total_energy > 0.0)) {
      continue;
    }

    for (std::size_t i = 0; i < n; ++i) {
      const int ch = raw_hit.channels[i];
      const int fec = channel_fecs[i];
      const double weight = energies[i] / total_energy;
      posx += weight * kPosX[fec][ch];
      posy += weight * kPosY[fec][ch];
      posz += weight *
              tpc_property.convertDriftTime2PosZScale(raw_hit.drift_time, max_time);
    }

    const int channel = raw_hit.channels[max_index];
    const int channel_fec = channel_fecs[max_index];
    auto hit = std::make_shared<DetectorHit>();
    hit->setTI(static_cast<int64_t>(raw_hit.ti));
    hit->setDetectorChannelID(ChannelID::Undefined, channel_fec, channel);

    if((-2.56 * unit::cm < posx)&&(posx < 2.56 * unit::cm)&&(-2.56 * unit::cm < posy)&&(posy < 2.56 * unit::cm)){
        if(posx > 0.0 * unit::cm){
            if(posy > 0.0 * unit::cm){
                hit->setDetectorID(2);
            } else {
                hit->setDetectorID(1);
            }

        } else{
            if(posy > 0.0 * unit::cm){
                hit->setDetectorID(3);
            } else {
                hit->setDetectorID(0);
            }
        }
    } else {
        hit->setDetectorID(-1);
    }

    //std::cout << "FEC" << raw_hit.fec << "/" << channel << "-ch/";
    //std::cout << raw_hit.drift_time/unit::us << "us: ";
    //std::cout << "x=" << posx/unit::cm << "cm, ";
    //std::cout << "y=" << posy/unit::cm << "cm, ";
    //std::cout << "z=" << posz/unit::cm << "cm" << std::endl;

    hit->setReadoutChannelID(channel_fec, channel_fec, channel);
    hit->setVoxel(kPixelX[channel], kPixelY[channel], VoxelID::Undefined);
    hit->setEnergy(total_energy * config.energy.factor_energy);
    hit->setPosition(posx, posy, posz);
    hit->setPositionError(tpc_property.posXError(),
                          tpc_property.posYError(),
                          tpc_property.posZError());
    hits.push_back(std::move(hit));
  }

  return hits;
}

std::string deriveHitTreeOutputPath(const std::string& explicit_path)
{
  if (!explicit_path.empty()) {
    return explicit_path;
  }

  return "hittree.root";
}

} // namespace

NanoGRAMSCalibration::NanoGRAMSCalibration() = default;

NanoGRAMSCalibration::~NanoGRAMSCalibration() = default;

ANLStatus NanoGRAMSCalibration::mod_define()
{
  define_parameter("hittree_file", &mod_class::hittree_file_);
  define_parameter("gain_tp_hash", &mod_class::gain_tp_dict_);
  define_map_key("fec", "0");
  add_value_element("gain", &mod_class::gain_tp_value_);
  return AS_OK;
}

ANLStatus NanoGRAMSCalibration::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  if (!exist_module("NanoGRAMSHitExtraction")) {
    throw std::runtime_error(
        "NanoGRAMSCalibration requires NanoGRAMSHitExtraction in the same ANL chain.");
  }
  get_module("NanoGRAMSHitExtraction", &data_reduction_);

  calibration_config_ = readCalibrationConfig(data_reduction_->configFilePath());

  for(int i=0;i<NUM_VATA;i++){
    std::string fec_id_str = std::to_string(i);
    calibration_config_.energy.tp_adc_values[i] = gain_tp_dict_[fec_id_str];
  }
  std::cout << "tp_adc_values [ ";
  for (std::size_t i = 0; i < calibration_config_.energy.tp_adc_values.size(); ++i) {
    if (i != 0) {
      std::cout << ", ";
    }
    std::cout << calibration_config_.energy.tp_adc_values[i];
  }
  std::cout << " ]" << std::endl;

  const fs::path gain_info_path =
      resolvePath(calibration_config_.config_dir, calibration_config_.energy.gain_info_file);
  const fs::path spline_path =
      resolvePath(calibration_config_.config_dir,
                  calibration_config_.energy.q_to_kev_spline_file);

  tpc_property_.loadParamCoulomb2keVForSpline3D(spline_path,
                                                calibration_config_.general.efield);
  tpc_property_.loadParamGainMatrices(gain_info_path);
  tpc_property_.setDriftVelocity(electronDriftVelocity(
                                    calibration_config_.general.temperature, 
                                    calibration_config_.general.efield));
  tpc_property_.setAnodePosZ(calibration_config_.position.anode_pos_z);
  tpc_property_.applyTemperatureCorrection(
      calibration_config_.energy.tp_channel,
      calibration_config_.energy.ccal,
      calibration_config_.energy.tp_adc_values);

  const fs::path output_path =
      prepareOutputPath(deriveHitTreeOutputPath(hittree_file_));

  output_file_ = std::make_unique<TFile>(output_path.string().c_str(), "RECREATE");
  if (!output_file_ || output_file_->IsZombie()) {
    throw std::runtime_error("Failed to create hit tree ROOT file: " + output_path.string());
  }

  hit_tree_ = new TTree(grams::kHitTreeName, grams::kHitTreeName);
  hit_tree_->SetDirectory(output_file_.get());

  tree_io_ = std::make_unique<HitTreeIOWithInitialInfo>();
  tree_io_->enableInitialInfoRecord();
  tree_io_->setTree(hit_tree_);
  tree_io_->defineBranches();
  tree_io_->setInitialInfo(0.0, vector3_t(0.0, 0.0, 0.0), 0.0, vector3_t(0.0, 0.0, 0.0));
  tree_io_->setInitialPolarization(0.0, 0.0, 0.0);
  tree_io_->setWeight(1.0);

  written_events_ = 0;
  define_evs("NanoGRAMSHitTree:Fill");
  return AS_OK;
}

ANLStatus NanoGRAMSCalibration::mod_analyze()
{
  if (!data_reduction_ || !data_reduction_->hasCurrentEvent()) {
    return AS_OK;
  }

  const auto hits =
      buildCalibratedHits(calibration_config_,
                          tpc_property_,
                          data_reduction_->currentEventHits());
  if (hits.empty()) {
    return AS_OK;
  }

  tree_io_->fillHits(data_reduction_->currentEventId(), hits);
  ++written_events_;
  set_evs("NanoGRAMSHitTree:Fill");
  return AS_OK;
}

ANLStatus NanoGRAMSCalibration::mod_end_run()
{
  if (output_file_ && hit_tree_) {
    output_file_->cd();
    hit_tree_->Write();
    output_file_->Write();
    std::cout << "[ROOT] Saved hit tree: " << output_file_->GetName()
              << " (events=" << written_events_
              << ", entries=" << hit_tree_->GetEntries() << ")\n";
    output_file_->Close();
  }

  tree_io_.reset();
  hit_tree_ = nullptr;
  output_file_.reset();
  data_reduction_ = nullptr;
  return AS_OK;
}

} /* namespace comptonsoft */
