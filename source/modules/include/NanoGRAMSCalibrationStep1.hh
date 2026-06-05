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

#ifndef COMPTONSOFT_NanoGRAMSCalibrationStep1_H
#define COMPTONSOFT_NanoGRAMSCalibrationStep1_H 1

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

#include "NanoGRAMSTPCTreeUtil.hh"
#include "VCSModule.hh"

class TFile;
class TSpline3;
class TTree;

namespace comptonsoft
{

namespace ngUtil
{
struct RawFECHit;
}

class HitTreeIOWithInitialInfo;
class NanoGRAMSDataReduction;

constexpr int kNanoGRAMSNumGainParams = 4;
using GainParamArray = std::array<double, kNanoGRAMSNumGainParams>;
using GainMatrix = std::array<GainParamArray, NUM_CH_CHARGE>;

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
  std::array<double, NUM_CHARGE_READOUT> tp_adc_values = {1.0, 1.0, 1.0, 1.0};
};

struct PositionCalibrationConfig
{
  double anode_pos_z_cm = 5.0;
};

struct CalibrationConfig
{
  EnergyCalibrationConfig energy;
  PositionCalibrationConfig position;
  std::filesystem::path config_dir;
};

class TPCResponse
{
public:
  TPCResponse();
  ~TPCResponse();

  void setDriftVelocity(double value);
  void setAnodePosZ(double value);

  double posXError() const;
  double posYError() const;
  double posZError() const;

  void loadParamCoulomb2keVForSpline3D(const std::filesystem::path& spline_path,
                                       int efield_v_cm);
  void loadParamGainMatrices(const std::filesystem::path& gain_info_path);
  void applyTemperatureCorrection(int tp_channel,
                                  int ccal,
                                  const std::array<double, NUM_CHARGE_READOUT>& tp_adc_values);

  double temperatureCorrectionFactor(int fec) const;
  double convertADC2keVWithSpline3D(int fec, int ch, double adc) const;
  double convertADC2CWithSpline3D(int fec, int ch, double adc) const;
  double convertDriftTime2PosZ(double drift_time) const;
  double convertDriftTime2PosZScale(double drift_time, double max_time) const;

private:
  static double cubic(double x, const GainParamArray& params);

  double drift_velocity_ = 0.0;
  double anode_pos_z_ = 0.0;
  double pixel_size_ = 0.0;
  double z_height_lartpc_ = 0.0;
  double pos_x_error_ = 0.0;
  double pos_y_error_ = 0.0;
  double pos_z_error_ = 0.0;
  TSpline3* spline_ = nullptr;
  double xmin_spline3d_ = 0.0;
  double xmax_spline3d_ = 0.0;
  std::array<GainMatrix, NUM_CHARGE_READOUT> gain_matrices_adc_to_c_{};
  std::array<GainMatrix, NUM_CHARGE_READOUT> gain_matrices_ccal_to_adc_{};
  std::unique_ptr<TFile> spline_file_;
  std::array<double, NUM_CHARGE_READOUT> temperature_correction_factors_{};
};

class NanoGRAMSCalibrationStep1 : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSCalibrationStep1, 1.0);

public:
  NanoGRAMSCalibrationStep1();
  ~NanoGRAMSCalibrationStep1() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  std::string config_file_;
  std::string rawhitdata_file_;
  std::string hittree_file_;

  const NanoGRAMSDataReduction* data_reduction_ = nullptr;
  std::unique_ptr<TFile> output_file_;
  TTree* hit_tree_ = nullptr;
  std::unique_ptr<HitTreeIOWithInitialInfo> tree_io_;
  CalibrationConfig calibration_config_;
  TPCResponse response_;
  int64_t written_events_ = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSCalibrationStep1_H */
