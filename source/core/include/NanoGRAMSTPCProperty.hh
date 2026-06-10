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

#ifndef COMPTONSOFT_NanoGRAMSTPCProperty_H
#define COMPTONSOFT_NanoGRAMSTPCProperty_H 1

#include <array>
#include <filesystem>
#include <memory>

#include "NanoGRAMSEvent.hh"

class TFile;
class TSpline3;

namespace comptonsoft
{

constexpr int kNanoGRAMSNumGainParams = 4;
using GainParamArray = std::array<double, kNanoGRAMSNumGainParams>;
using GainMatrix = std::array<GainParamArray, NUM_CH_EACH_VATA>;

class TPCProperty
{
public:
  TPCProperty();
  ~TPCProperty();

  void setDriftVelocity(double value);
  void setAnodePosZ(double value);

  double posXError() const;
  double posYError() const;
  double posZError() const;

  void loadParamCoulomb2keVForSpline3D(const std::filesystem::path& spline_path, double efield);
  void loadParamGainMatrices(const std::filesystem::path& gain_info_path);
  void applyTemperatureCorrection(int tp_channel,
                                  int ccal,
                                  const std::array<double, NUM_VATA>& tp_adc_values);

  double temperatureCorrectionFactor(int fec) const;
  double convertADC2keVWithSpline3D(int fec, int ch, double adc) const;
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
  std::array<GainMatrix, NUM_VATA> gain_matrices_adc_to_c_{};
  std::array<GainMatrix, NUM_VATA> gain_matrices_ccal_to_adc_{};
  std::unique_ptr<TFile> spline_file_;
  std::array<double, NUM_VATA> temperature_correction_factors_{};
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSTPCProperty_H */
