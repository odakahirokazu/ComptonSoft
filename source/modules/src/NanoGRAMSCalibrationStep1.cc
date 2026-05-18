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

#include "NanoGRAMSCalibrationStep1.hh"

#include "NanoGRAMSDataReduction.hh"

#include <TFile.h>
#include <TSpline.h>
#include <TTree.h>

#include <algorithm>
#include <array>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cmath>
#include <filesystem>
#include <hdf5.h>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

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

constexpr int kNumFec = ngUtil::kDefaultFecNum;
constexpr int kNumChannels = ngUtil::kNPixel;
constexpr int kNumGainParams = 4;
constexpr double kPixelSize = 0.32 * unit::cm;

using GainParamArray = std::array<double, kNumGainParams>;
using GainMatrix = std::array<GainParamArray, kNumChannels>;
using PositionTable = std::array<std::array<double, kNumChannels>, kNumFec>;
using PixelIndexTable = std::array<std::int16_t, kNumChannels>;

struct EnergyCalibrationConfig
{
  std::string gain_info_file;
  std::string q_to_kev_spline_file;
  int efield_v_cm = 500;
  double temperature_k = 87.0;
  double factor_energy = 1.0;
  double max_time_us = 67.0;
  int tp_channel = 17;
  int ccal = 8;
  std::array<double, kNumFec> tp_adc_values = {1.0, 1.0, 1.0, 1.0};
};

struct PositionCalibrationConfig
{
  double anode_pos_z_cm = 5.0;
};

struct CalibrationConfig
{
  EnergyCalibrationConfig energy;
  PositionCalibrationConfig position;
  fs::path config_dir;
};

constexpr PositionTable buildPosX()
{
  PositionTable dict{};
  for (int fec = 0; fec < kNumFec; ++fec) {
    for (int ch = 0; ch < kNumChannels; ++ch) {
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
  for (int fec = 0; fec < kNumFec; ++fec) {
    for (int ch = 0; ch < kNumChannels; ++ch) {
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
  for (int ch = 0; ch < kNumChannels; ++ch) {
    dict[ch] = static_cast<std::int16_t>(ch / 8);
  }
  return dict;
}

constexpr PixelIndexTable buildPixelY()
{
  PixelIndexTable dict{};
  for (int ch = 0; ch < kNumChannels; ++ch) {
    if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
      dict[ch] = static_cast<std::int16_t>(7 - (ch % 8));
    } else {
      dict[ch] = static_cast<std::int16_t>(ch % 8);
    }
  }
  return dict;
}

const PositionTable kPosX = buildPosX();
const PositionTable kPosY = buildPosY();
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

std::array<double, kNumFec> readTpAdcValues(const boost::property_tree::ptree& pt,
                                            const std::array<double, kNumFec>& default_value)
{
  std::array<double, kNumFec> values = default_value;
  const auto node = pt.get_child_optional("tp_adc_values");
  if (!node) {
    return values;
  }

  std::size_t index = 0;
  for (const auto& item : *node) {
    if (index >= values.size()) {
      break;
    }
    values[index] = item.second.get_value<double>();
    ++index;
  }
  return values;
}

CalibrationConfig readCalibrationConfig(const std::string& config_file)
{
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(config_file, pt);

  const auto calibration = pt.get_child_optional("calibration");
  if (!calibration) {
    throw std::runtime_error("Missing 'calibration' section in config file: " + config_file);
  }

  CalibrationConfig cfg;
  cfg.config_dir = fs::absolute(fs::path(config_file)).parent_path();

  const auto energy = calibration->get_child_optional("energy");
  if (!energy) {
    throw std::runtime_error("Missing 'calibration.energy' section in config file: " + config_file);
  }

  cfg.energy.gain_info_file = energy->get<std::string>("gain_info_file");
  cfg.energy.q_to_kev_spline_file = energy->get<std::string>("q_to_kev_spline_file");
  cfg.energy.efield_v_cm = energy->get<int>("efield_v_cm", cfg.energy.efield_v_cm);
  cfg.energy.temperature_k = energy->get<double>("temperature_k", cfg.energy.temperature_k);
  cfg.energy.factor_energy = energy->get<double>("factor_energy", cfg.energy.factor_energy);
  cfg.energy.max_time_us = energy->get<double>("max_time_us", cfg.energy.max_time_us);
  cfg.energy.tp_channel = energy->get<int>("tp_channel", cfg.energy.tp_channel);
  cfg.energy.ccal = energy->get<int>("ccal", cfg.energy.ccal);
  cfg.energy.tp_adc_values = readTpAdcValues(*energy, cfg.energy.tp_adc_values);

  if (const auto position = calibration->get_child_optional("position")) {
    cfg.position.anode_pos_z_cm =
        position->get<double>("anode_pos_z_cm", cfg.position.anode_pos_z_cm);
  }

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

GainMatrix loadCalibrationMatrix(const fs::path& gain_info_path, const std::string& dataset_path)
{
  GainMatrix matrix{};

  const hid_t file = H5Fopen(gain_info_path.string().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  if (file < 0) {
    throw std::runtime_error("Failed to open HDF5 file: " + gain_info_path.string());
  }

  const hid_t dataset = H5Dopen2(file, dataset_path.c_str(), H5P_DEFAULT);
  if (dataset < 0) {
    H5Fclose(file);
    throw std::runtime_error("Failed to open HDF5 dataset: " + dataset_path);
  }

  const hid_t space = H5Dget_space(dataset);
  if (space < 0) {
    H5Dclose(dataset);
    H5Fclose(file);
    throw std::runtime_error("Failed to query HDF5 dataspace: " + dataset_path);
  }

  const int ndims = H5Sget_simple_extent_ndims(space);
  std::vector<hsize_t> dims(static_cast<std::size_t>(ndims), 0);
  H5Sget_simple_extent_dims(space, dims.data(), nullptr);

  std::vector<double> values(dims[0] * dims[1], 0.0);
  const herr_t status =
      H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, values.data());

  H5Sclose(space);
  H5Dclose(dataset);
  H5Fclose(file);

  if (status < 0) {
    throw std::runtime_error("Failed to read HDF5 dataset: " + dataset_path);
  }

  for (int ch = 0; ch < kNumChannels; ++ch) {
    for (int par = 0; par < kNumGainParams; ++par) {
      matrix[ch][par] = values[static_cast<std::size_t>(kNumGainParams * ch + par)];
    }
  }

  return matrix;
}

class TPCResponse
{
public:
  TPCResponse()
  {
    pos_x_error_ = pixel_size_ / std::sqrt(12.0);
    pos_y_error_ = pixel_size_ / std::sqrt(12.0);
    temperature_correction_factors_.fill(1.0);
  }

  void setDriftVelocity(double value) { drift_velocity_ = value; }
  void setAnodePosZ(double value) { anode_pos_z_ = value; }

  double posXError() const { return pos_x_error_; }
  double posYError() const { return pos_y_error_; }
  double posZError() const { return pos_z_error_; }

  void loadParamCoulomb2keVForSpline3D(const fs::path& spline_path, int efield_v_cm)
  {
    spline_file_ = std::make_unique<TFile>(spline_path.string().c_str(), "READ");
    if (!spline_file_ || spline_file_->IsZombie()) {
      throw std::runtime_error("Failed to open spline ROOT file: " + spline_path.string());
    }

    const std::string efield_name = "E" + std::to_string(efield_v_cm);
    spline_ = dynamic_cast<TSpline3*>(spline_file_->Get(efield_name.c_str()));
    if (!spline_) {
      throw std::runtime_error("Missing TSpline3 '" + efield_name + "' in " +
                               spline_path.string());
    }

    const int knots = spline_->GetNp();
    double y0 = 0.0;
    double y1 = 0.0;
    spline_->GetKnot(0, xmin_spline3d_, y0);
    spline_->GetKnot(knots - 1, xmax_spline3d_, y1);
  }

  void loadParamGainMatrices(const fs::path& gain_info_path)
  {
    for (int fec = 0; fec < kNumFec; ++fec) {
      const std::string prefix = "/FEC" + std::to_string(fec);
      gain_matrices_adc_to_c_[fec] = loadCalibrationMatrix(gain_info_path, prefix + "/ADC2C");
      gain_matrices_ccal_to_adc_[fec] =
          loadCalibrationMatrix(gain_info_path, prefix + "/ccal2ADC");
    }
  }

  void applyTemperatureCorrection(int tp_channel,
                                  int ccal,
                                  const std::array<double, kNumFec>& tp_adc_values)
  {
    for (int fec = 0; fec < kNumFec; ++fec) {
      const double measured = tp_adc_values[fec];
      if (measured <= 0.0) {
        throw std::runtime_error("tp_adc_values must be positive for all FECs.");
      }

      const GainParamArray& params = gain_matrices_ccal_to_adc_[fec][tp_channel];
      const double room_temp_adc = cubic(ccal, params);
      temperature_correction_factors_[fec] = room_temp_adc / measured;
    }
  }

  double temperatureCorrectionFactor(int fec) const
  {
    return temperature_correction_factors_[fec];
  }

  double convertADC2keVWithSpline3D(int fec, int ch, double adc) const
  {
    const double charge_coulomb = convertADC2CWithSpline3D(fec, ch, adc) / unit::coulomb;
    return spline_->Eval(charge_coulomb) * unit::keV;
  }

  double convertADC2CWithSpline3D(int fec, int ch, double adc) const
  {
    const GainParamArray& params = gain_matrices_adc_to_c_[fec][ch];
    double charge_coulomb = cubic(adc, params);
    charge_coulomb = std::clamp(charge_coulomb, xmin_spline3d_, xmax_spline3d_);
    return charge_coulomb * unit::coulomb;
  }

  double convertDriftTime2PosZ(double drift_time) const
  {
    return anode_pos_z_ - drift_time * drift_velocity_;
  }

  double convertDriftTime2PosZScale(double drift_time, double max_time) const
  {
    return z_height_lartpc_ * (0.5 - drift_time / max_time);
  }

private:
  static double cubic(double x, const GainParamArray& params)
  {
    return params[0] * x * x * x + params[1] * x * x + params[2] * x + params[3];
  }

  double drift_velocity_ = 1.0e5 * unit::cm / unit::s;
  double anode_pos_z_ = 5.0 * unit::cm;
  double pixel_size_ = kPixelSize;
  double z_height_lartpc_ = 10.0 * unit::cm;
  double pos_x_error_ = 0.0;
  double pos_y_error_ = 0.0;
  double pos_z_error_ = 0.1 * unit::cm;
  TSpline3* spline_ = nullptr;
  double xmin_spline3d_ = 0.0;
  double xmax_spline3d_ = 0.0;
  std::array<GainMatrix, kNumFec> gain_matrices_adc_to_c_{};
  std::array<GainMatrix, kNumFec> gain_matrices_ccal_to_adc_{};
  std::unique_ptr<TFile> spline_file_;
  std::array<double, kNumFec> temperature_correction_factors_{};
};

std::vector<DetectorHit_sptr> buildCalibratedHits(
    const CalibrationConfig& config,
    const TPCResponse& response,
    const std::vector<ngUtil::StreamingFecHit>& raw_hits)
{
  std::vector<DetectorHit_sptr> hits;
  hits.reserve(raw_hits.size());

  const double max_time = config.energy.max_time_us * unit::us;

  for (const auto& raw_hit : raw_hits) {
    if (raw_hit.fec < 0 || raw_hit.fec >= kNumFec) {
      throw std::runtime_error("FEC index out of range in NanoGRAMS calibration.");
    }

    const std::size_t n = std::min(raw_hit.channels.size(), raw_hit.adcs.size());
    if (n == 0) {
      continue;
    }

    double total_energy = 0.0;
    double posx = 0.0;
    double posy = 0.0;
    double posz = 0.0;
    std::size_t max_index = 0;
    double max_energy = -std::numeric_limits<double>::infinity();

    std::vector<double> energies(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
      const int ch = raw_hit.channels[i];
      if (ch < 0 || ch >= kNumChannels) {
        throw std::runtime_error("Channel index out of range in NanoGRAMS calibration.");
      }

      const double corrected_adc =
          static_cast<double>(raw_hit.adcs[i]) * response.temperatureCorrectionFactor(raw_hit.fec);
      const double energy =
          response.convertADC2keVWithSpline3D(raw_hit.fec, ch, corrected_adc);
      energies[i] = energy;
      total_energy += energy;

      if (energy > max_energy) {
        max_energy = energy;
        max_index = i;
      }
    }

    if (!(total_energy > 0.0)) {
      continue;
    }

    for (std::size_t i = 0; i < n; ++i) {
      const int ch = raw_hit.channels[i];
      const double weight = energies[i] / total_energy;
      posx += weight * kPosX[raw_hit.fec][ch];
      posy += weight * kPosY[raw_hit.fec][ch];
      posz += weight *
              response.convertDriftTime2PosZScale(raw_hit.drift_us * unit::us, max_time);
    }

    const int channel = raw_hit.channels[max_index];
    auto hit = std::make_shared<DetectorHit>();
    hit->setTI(static_cast<std::int64_t>(raw_hit.ti));
    hit->setDetectorChannelID(ChannelID::Undefined, raw_hit.fec, channel);
    hit->setReadoutChannelID(raw_hit.fec, raw_hit.fec, channel);
    hit->setVoxel(kPixelX[channel], kPixelY[channel], VoxelID::Undefined);
    hit->setEnergy(total_energy * config.energy.factor_energy);
    hit->setPosition(posx, posy, posz);
    hit->setPositionError(response.posXError(), response.posYError(), response.posZError());
    hits.push_back(std::move(hit));
  }

  return hits;
}

std::string deriveHitTreeOutputPath(const std::string& explicit_path,
                                    const std::string& rawhit_path)
{
  if (!explicit_path.empty()) {
    return explicit_path;
  }

  if (!rawhit_path.empty()) {
    const fs::path raw_path(rawhit_path);
    const std::string stem = raw_path.stem().string();
    if (stem == "rawhit" || stem == "rawvata") {
      return (raw_path.parent_path() / "hittree.root").string();
    }
    return (raw_path.parent_path() / (stem + "_hittree.root")).string();
  }

  return "hittree.root";
}

} // namespace

class NanoGRAMSCalibrationStep1::Impl
{
public:
  explicit Impl(const std::string& config_file)
      : config_(readCalibrationConfig(config_file))
  {
    const fs::path gain_info_path = resolvePath(config_.config_dir, config_.energy.gain_info_file);
    const fs::path spline_path =
        resolvePath(config_.config_dir, config_.energy.q_to_kev_spline_file);

    response_.loadParamCoulomb2keVForSpline3D(spline_path, config_.energy.efield_v_cm);
    response_.loadParamGainMatrices(gain_info_path);
    response_.setDriftVelocity(electronDriftVelocity(
        config_.energy.temperature_k * unit::kelvin,
        config_.energy.efield_v_cm * unit::volt / unit::cm));
    response_.setAnodePosZ(config_.position.anode_pos_z_cm * unit::cm);
    response_.applyTemperatureCorrection(
        config_.energy.tp_channel,
        config_.energy.ccal,
        config_.energy.tp_adc_values);
  }

  std::vector<DetectorHit_sptr> buildHits(
      const std::vector<ngUtil::StreamingFecHit>& raw_hits) const
  {
    return buildCalibratedHits(config_, response_, raw_hits);
  }

private:
  CalibrationConfig config_;
  TPCResponse response_;
};

NanoGRAMSCalibrationStep1::NanoGRAMSCalibrationStep1() = default;

NanoGRAMSCalibrationStep1::~NanoGRAMSCalibrationStep1() = default;

ANLStatus NanoGRAMSCalibrationStep1::mod_define()
{
  define_parameter("config_file", &mod_class::config_file_);
  define_parameter("rawhitdata_file", &mod_class::rawhitdata_file_);
  define_parameter("hittree_file", &mod_class::hittree_file_);
  define_parameter("hitdata_file", &mod_class::hittree_file_);
  return AS_OK;
}

ANLStatus NanoGRAMSCalibrationStep1::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  if (config_file_.empty()) {
    throw std::runtime_error("Calibration config_file is empty.");
  }

  if (!exist_module("NanoGRAMSDataReduction")) {
    throw std::runtime_error(
        "NanoGRAMSCalibrationStep1 requires NanoGRAMSDataReduction in the same ANL chain.");
  }
  get_module("NanoGRAMSDataReduction", &data_reduction_);

  impl_ = std::make_unique<Impl>(config_file_);

  const std::string rawhit_path =
      rawhitdata_file_.empty() ? data_reduction_->rawHitFilePath() : rawhitdata_file_;
  const fs::path output_path =
      prepareOutputPath(deriveHitTreeOutputPath(hittree_file_, rawhit_path));

  output_file_ = std::make_unique<TFile>(output_path.string().c_str(), "RECREATE");
  if (!output_file_ || output_file_->IsZombie()) {
    throw std::runtime_error("Failed to create hit tree ROOT file: " + output_path.string());
  }

  hittree_ = new TTree("hittree", "hittree");
  hittree_->SetDirectory(output_file_.get());

  tree_io_ = std::make_unique<HitTreeIOWithInitialInfo>();
  tree_io_->enableInitialInfoRecord();
  tree_io_->setTree(hittree_);
  tree_io_->defineBranches();
  tree_io_->setInitialInfo(0.0, vector3_t(0.0, 0.0, 0.0), 0.0, vector3_t(0.0, 0.0, 0.0));
  tree_io_->setInitialPolarization(0.0, 0.0, 0.0);
  tree_io_->setWeight(1.0);

  written_events_ = 0;
  define_evs("NanoGRAMSHitTree:Fill");
  return AS_OK;
}

ANLStatus NanoGRAMSCalibrationStep1::mod_analyze()
{
  if (!data_reduction_ || !data_reduction_->hasCurrentEvent()) {
    return AS_OK;
  }

  const auto hits = impl_->buildHits(data_reduction_->currentEventHits());
  if (hits.empty()) {
    return AS_OK;
  }

  tree_io_->fillHits(data_reduction_->currentEventId(), hits);
  ++written_events_;
  set_evs("NanoGRAMSHitTree:Fill");
  return AS_OK;
}

ANLStatus NanoGRAMSCalibrationStep1::mod_end_run()
{
  if (output_file_ && hittree_) {
    output_file_->cd();
    hittree_->Write();
    output_file_->Write();
    std::cout << "[ROOT] Saved hit tree: " << output_file_->GetName()
              << " (events=" << written_events_
              << ", entries=" << hittree_->GetEntries() << ")\n";
    output_file_->Close();
  }

  tree_io_.reset();
  hittree_ = nullptr;
  output_file_.reset();
  impl_.reset();
  data_reduction_ = nullptr;
  return AS_OK;
}

} /* namespace comptonsoft */
