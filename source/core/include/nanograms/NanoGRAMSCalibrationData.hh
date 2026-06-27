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

#ifndef COMPTONSOFT_NanoGRAMSCalibrationData_H
#define COMPTONSOFT_NanoGRAMSCalibrationData_H 1

#include <array>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "AstroUnits.hh"
#include "NanoGRAMSEvent.hh"

namespace comptonsoft
{

struct EnergyCalibrationConfig
{
  std::string gain_info_file;
  std::string q_to_kev_spline_file;
  double max_time = 67.0 * anlgeant4::unit::us;
  int tp_channel = 17;
  int ccal = 8;
  std::array<double, NUM_VATA> tp_adc_values = {1.0, 1.0, 1.0, 1.0};
};

struct TestPulseGainRow
{
  std::string time_id;
  double time = 0.0;
  std::array<double, NUM_VATA> fec_gain{};
};

using TestPulseGainTable = std::vector<TestPulseGainRow>;

struct PositionCalibrationConfig
{
  double anode_pos_z = 5.0 * anlgeant4::unit::cm;
};

struct GeneralConfig
{
  double temperature = 0.0 * anlgeant4::unit::kelvin;
  double efield = 0.0 * anlgeant4::unit::volt / anlgeant4::unit::cm;
};

struct CalibrationConfig
{
  EnergyCalibrationConfig energy;
  PositionCalibrationConfig position;
  GeneralConfig general;
  std::filesystem::path config_dir;
};

CalibrationConfig readCalibrationConfig(const std::string& config_file);

std::filesystem::path resolveCalibrationPath(const std::filesystem::path& base_dir,
                                             const std::string& value);

std::string timeIdFromTPCTreePath(const std::string& tpctree_file);

TestPulseGainTable readTestPulseGainTable(const std::filesystem::path& csv_path);

std::array<double, NUM_VATA> interpolatedTestPulseGains(
    const TestPulseGainTable& rows,
    double target_time);

std::array<double, NUM_VATA> interpolatedTestPulseGainsFromCsv(
    const std::filesystem::path& csv_path,
    const std::string& target_time_id);

std::array<double, NUM_VATA> fixedTestPulseGainsFromHash(
    const std::map<std::string, double>& gain_tp_dict);

double electronDriftVelocity(double temperature, double e_field);

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSCalibrationData_H */
