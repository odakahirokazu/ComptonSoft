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

#ifndef COMPTONSOFT_NanoGRAMSCalibration_H
#define COMPTONSOFT_NanoGRAMSCalibration_H 1

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <map>

#include "NanoGRAMSTPCProperty.hh"
#include "NanoGRAMSTPCDataProcessor.hh"
#include "VCSModule.hh"

class TFile;
class TTree;

namespace comptonsoft
{

namespace grams
{
struct RawFECHit;
}

class HitTreeIOWithInitialInfo;
class NanoGRAMSHitExtraction;

struct EnergyCalibrationConfig
{
  std::string gain_info_file;
  std::string q_to_kev_spline_file;
  double factor_energy = 1.0;
  double max_time = 67.0 * unit::us;
  int tp_channel = 17;
  int ccal = 8;
  std::array<double, NUM_VATA> tp_adc_values = {1.0, 1.0, 1.0, 1.0};
};

struct PositionCalibrationConfig
{
  double anode_pos_z = 5.0 * unit::cm;
};

struct GeneralConfig
{
  double temperature = 0.0 * unit::kelvin;
  double efield = 0 * unit::volt / unit::cm;
};

struct CalibrationConfig
{
  EnergyCalibrationConfig energy;
  PositionCalibrationConfig position;
  GeneralConfig general;
  std::filesystem::path config_dir;
};

class NanoGRAMSCalibration : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSCalibration, 1.0);

public:
  NanoGRAMSCalibration();
  ~NanoGRAMSCalibration() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  std::string hittree_file_;
  std::map<std::string, double> gain_tp_dict_;
  double gain_tp_value_ = 0.0;

  const NanoGRAMSHitExtraction* data_reduction_ = nullptr;
  std::unique_ptr<TFile> output_file_;
  TTree* hit_tree_ = nullptr;
  std::unique_ptr<HitTreeIOWithInitialInfo> tree_io_;
  CalibrationConfig calibration_config_;
  TPCProperty tpc_property_;
  int64_t written_events_ = 0;
  //std::vector<double> gain_tp_array_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSCalibration_H */
