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

#include "NanoGRAMSTPCProperty.hh"

#include <algorithm>
#include <cmath>
#include <format>
#include <hdf5.h>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "AstroUnits.hh"

namespace comptonsoft
{

namespace unit = anlgeant4::unit;
namespace fs = std::filesystem;

namespace
{

constexpr double kPixelSize = 0.32 * unit::cm;

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

  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    for (int par = 0; par < kNanoGRAMSNumGainParams; ++par) {
      matrix[ch][par] = values[static_cast<std::size_t>(kNanoGRAMSNumGainParams * ch + par)];
    }
  }

  return matrix;
}

} // namespace

TPCProperty::TPCProperty()
{
  drift_velocity_  = 1.0 * unit::mm / unit::us;
  anode_pos_z_     = 5.0 * unit::cm;
  pixel_size_      = kPixelSize;
  z_height_lartpc_ = 10.0 * unit::cm;
  pos_x_error_     = pixel_size_ / std::sqrt(12.0);
  pos_y_error_     = pixel_size_ / std::sqrt(12.0);
  pos_z_error_     = 1.0 * unit::mm;
  temperature_correction_factors_.fill(1.0);
}

TPCProperty::~TPCProperty() = default;

void TPCProperty::setChargeToEnergySpline(double efield)
{
  charge_to_energy_spline_.setElectricField(efield / (unit::volt / unit::cm));
}

void TPCProperty::loadParamGainMatrices(const fs::path& gain_info_path)
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const std::string prefix = std::format("/FEC{}", fec);
    gain_matrices_adc_to_c_[fec] =
        loadCalibrationMatrix(gain_info_path, std::format("{}/ADC2C", prefix));
    gain_matrices_ccal_to_adc_[fec] =
        loadCalibrationMatrix(gain_info_path, std::format("{}/ccal2ADC", prefix));
  }
}

void TPCProperty::applyTemperatureCorrection(
    int tp_channel,
    int ccal,
    const std::array<double, NUM_VATA>& tp_adc_values)
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const double measured = tp_adc_values[fec];
    if (!std::isfinite(measured)) {
      temperature_correction_factors_[fec] =
          std::numeric_limits<double>::quiet_NaN();
      continue;
    }
    if (measured <= 0.0) {
      throw std::runtime_error("Provided tp_adc_values must be positive.");
    }

    const GainParamArray& params = gain_matrices_ccal_to_adc_[fec][tp_channel];
    const double room_temp_adc   = cubic(ccal, params);
    temperature_correction_factors_[fec] = room_temp_adc / measured;
  }
}

double TPCProperty::temperatureCorrectionFactor(int fec) const
{
  return temperature_correction_factors_[fec];
}

double TPCProperty::convertADC2keV(int fec, int ch, double adc) const
{
  const GainParamArray& params = gain_matrices_adc_to_c_[fec][ch];
  double charge_coulomb = cubic(adc, params) - cubic(0.0, params);
  if (charge_coulomb <= 0.0) {
    return 0.0 * unit::keV;
  }
  return charge_to_energy_spline_.evaluate(charge_coulomb) * unit::keV;
}

double TPCProperty::convertDriftTime2PosZ(double drift_time) const
{
  return anode_pos_z_ - drift_time * drift_velocity_;
}

double TPCProperty::convertDriftTime2PosZScale(double drift_time, double max_time) const
{
  return z_height_lartpc_ * (0.5 - drift_time / max_time);
}

double TPCProperty::cubic(double x, const GainParamArray& params)
{
  return params[0] * x * x * x + params[1] * x * x + params[2] * x + params[3];
}

} /* namespace comptonsoft */
