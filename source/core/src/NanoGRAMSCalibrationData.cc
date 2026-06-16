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

#include "NanoGRAMSCalibrationData.hh"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <yaml-cpp/yaml.h>

namespace comptonsoft
{

namespace unit = anlgeant4::unit;
namespace fs = std::filesystem;

namespace
{

struct TestPulseGainRow
{
  std::string time_id;
  double time = 0.0;
  std::array<double, NUM_VATA> fec_gain{};
};

std::string trim(const std::string& text)
{
  const auto first = std::find_if_not(text.begin(), text.end(),
                                      [](unsigned char c) {
                                        return std::isspace(c);
                                      });
  const auto last = std::find_if_not(text.rbegin(), text.rend(),
                                     [](unsigned char c) {
                                       return std::isspace(c);
                                     }).base();
  if (first >= last) {
    return "";
  }
  return std::string(first, last);
}

std::vector<std::string> splitCsvLine(const std::string& line)
{
  std::vector<std::string> columns;
  std::string column;
  std::istringstream stream(line);
  while (std::getline(stream, column, ',')) {
    columns.push_back(trim(column));
  }
  return columns;
}

bool isTimeIdDate(const std::string& text)
{
  if (text.size() != 8) {
    return false;
  }
  return std::all_of(text.begin(), text.end(),
                     [](unsigned char c) { return std::isdigit(c); });
}

bool isTimeIdClock(const std::string& text)
{
  if (text.size() != 7 || text[4] != '_') {
    return false;
  }
  for (std::size_t i = 0; i < text.size(); ++i) {
    if (i == 4) {
      continue;
    }
    if (!std::isdigit(static_cast<unsigned char>(text[i]))) {
      return false;
    }
  }
  return true;
}

double parseTimestamp(const std::string& time_id)
{
  if (time_id.size() != 16 || time_id[8] != '/' || time_id[13] != '_') {
    throw std::runtime_error("Invalid NanoGRAMS time_id: " + time_id);
  }

  std::tm tm{};
  tm.tm_year = std::stoi(time_id.substr(0, 4)) - 1900;
  tm.tm_mon  = std::stoi(time_id.substr(4, 2)) - 1;
  tm.tm_mday = std::stoi(time_id.substr(6, 2));
  tm.tm_hour = std::stoi(time_id.substr(9, 2));
  tm.tm_min  = std::stoi(time_id.substr(11, 2));
  tm.tm_sec  = std::stoi(time_id.substr(14, 2));
  tm.tm_isdst = -1;

  const std::time_t t = std::mktime(&tm);
  if (t == static_cast<std::time_t>(-1)) {
    throw std::runtime_error("Failed to parse NanoGRAMS time_id: " + time_id);
  }
  return static_cast<double>(t);
}

double parseGainValue(const std::string& text)
{
  const std::string value = trim(text);
  if (value.empty()) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  return std::stod(value);
}

std::vector<TestPulseGainRow> readTestPulseGainTable(const fs::path& csv_path)
{
  std::ifstream input(csv_path);
  if (!input) {
    throw std::runtime_error("Failed to open test-pulse gain CSV: " + csv_path.string());
  }

  std::string line;
  if (!std::getline(input, line)) {
    throw std::runtime_error("Empty test-pulse gain CSV: " + csv_path.string());
  }

  const std::vector<std::string> header = splitCsvLine(line);
  std::array<int, NUM_VATA> fec_columns{};
  fec_columns.fill(-1);
  for (std::size_t i = 0; i < header.size(); ++i) {
    for (int fec = 0; fec < NUM_VATA; ++fec) {
      if (header[i] == std::format("FEC{}", fec)) {
        fec_columns[fec] = static_cast<int>(i);
      }
    }
  }
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    if (fec_columns[fec] < 0) {
      throw std::runtime_error(
          std::format("Missing FEC{} column in test-pulse gain CSV.", fec));
    }
  }

  std::vector<TestPulseGainRow> rows;
  while (std::getline(input, line)) {
    if (line.empty()) {
      continue;
    }
    const std::vector<std::string> columns = splitCsvLine(line);
    if (columns.empty()) {
      continue;
    }

    TestPulseGainRow row;
    row.time_id = columns[0];
    row.time = parseTimestamp(row.time_id);
    row.fec_gain.fill(std::numeric_limits<double>::quiet_NaN());
    for (int fec = 0; fec < NUM_VATA; ++fec) {
      const int column = fec_columns[fec];
      if (column < static_cast<int>(columns.size())) {
        row.fec_gain[fec] = parseGainValue(columns[column]);
      }
    }
    rows.push_back(row);
  }

  std::sort(rows.begin(), rows.end(),
            [](const TestPulseGainRow& lhs, const TestPulseGainRow& rhs) {
              return lhs.time < rhs.time;
            });
  if (rows.empty()) {
    throw std::runtime_error("No rows in test-pulse gain CSV: " + csv_path.string());
  }
  return rows;
}

std::array<double, NUM_VATA> interpolateTestPulseGains(
    const std::vector<TestPulseGainRow>& rows,
    double target_time)
{
  std::array<double, NUM_VATA> gains{};
  gains.fill(std::numeric_limits<double>::quiet_NaN());

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const TestPulseGainRow* before = nullptr;
    const TestPulseGainRow* after  = nullptr;
    for (const auto& row : rows) {
      if (!std::isfinite(row.fec_gain[fec])) {
        continue;
      }
      if (row.time <= target_time) {
        before = &row;
      }
      if (row.time >= target_time) {
        after = &row;
        break;
      }
    }

    if (before && after) {
      if (before == after || after->time == before->time) {
        gains[fec] = before->fec_gain[fec];
      } else {
        const double weight_after =
            (target_time - before->time) / (after->time - before->time);
        gains[fec] = (1.0 - weight_after) * before->fec_gain[fec] +
                     weight_after * after->fec_gain[fec];
      }
    } else if (before) {
      gains[fec] = before->fec_gain[fec];
    } else if (after) {
      gains[fec] = after->fec_gain[fec];
    }

    if (!std::isfinite(gains[fec]) || gains[fec] <= 0.0) {
      throw std::runtime_error(
          std::format("No valid test-pulse gain for FEC{}.", fec));
    }
  }

  return gains;
}

} // namespace

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
  cfg.energy.max_time             = node_energy["max_time_us"].as<double>() * unit::us;
  cfg.energy.tp_channel           = node_energy["tp_channel"].as<int>();
  cfg.energy.ccal                 = node_energy["ccal"].as<int>();

  cfg.general.efield = config_node["general"]["efield_v_cm"].as<int>() *
                       unit::volt / unit::cm;
  cfg.general.temperature  = config_node["general"]["temperature_k"].as<double>() *
                             unit::kelvin;
  cfg.position.anode_pos_z = node_position["anode_pos_z_cm"].as<double>() * unit::cm;

  std::cout << "gain_info_file: "       << cfg.energy.gain_info_file << std::endl;
  std::cout << "q_to_kev_spline_file: " << cfg.energy.q_to_kev_spline_file << std::endl;
  std::cout << "efield_v_cm: "          << cfg.general.efield / (unit::volt/unit::cm)  << std::endl;
  std::cout << "temperature_k: "        << cfg.general.temperature / unit::kelvin<< std::endl;
  std::cout << "max_time_us: "          << cfg.energy.max_time / unit::us    << std::endl;
  std::cout << "tp_channel: "           << cfg.energy.tp_channel     << std::endl;
  std::cout << "ccal: "                 << cfg.energy.ccal           << std::endl;
  std::cout << "anode_pos_z_cm: "       << cfg.position.anode_pos_z / unit::cm << std::endl;

  return cfg;
}

fs::path resolveCalibrationPath(const fs::path& base_dir, const std::string& value)
{
  fs::path path(value);
  if (path.is_absolute()) {
    return path;
  }
  return base_dir / path;
}

std::string timeIdFromTPCTreePath(const std::string& tpctree_file)
{
  const fs::path path(tpctree_file);
  const std::string clock = path.parent_path().filename().string();
  const std::string date  = path.parent_path().parent_path().filename().string();
  if (!isTimeIdDate(date) || !isTimeIdClock(clock)) {
    throw std::runtime_error("Cannot derive YYYYMMDD/HHMM_SS from TPC tree path: " +
                             tpctree_file);
  }
  return date + "/" + clock;
}

std::array<double, NUM_VATA> interpolatedTestPulseGainsFromCsv(
    const fs::path& csv_path,
    const std::string& target_time_id)
{
  return interpolateTestPulseGains(readTestPulseGainTable(csv_path),
                                   parseTimestamp(target_time_id));
}

std::array<double, NUM_VATA> fixedTestPulseGainsFromHash(
    const std::map<std::string, double>& gain_tp_dict)
{
  std::array<double, NUM_VATA> gains{};
  gains.fill(std::numeric_limits<double>::quiet_NaN());

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const std::string numeric_key = std::format("{}", fec);
    const std::string fec_key = std::format("FEC{}", fec);

    if (const auto it = gain_tp_dict.find(numeric_key); it != gain_tp_dict.end()) {
      gains[fec] = it->second;
    } else if (const auto it = gain_tp_dict.find(fec_key); it != gain_tp_dict.end()) {
      gains[fec] = it->second;
    }

    if (!std::isfinite(gains[fec]) || gains[fec] <= 0.0) {
      throw std::runtime_error(
          std::format("gain_tp_hash must provide positive values for FEC0-FEC3. "
                      "Missing or invalid FEC{}.",
                      fec));
    }
  }

  return gains;
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

} /* namespace comptonsoft */
