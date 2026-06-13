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

#include "NanoGRAMSConstants.hh"
#include "NanoGRAMSHitCalibrator.hh"
#include "NanoGRAMSHitExtraction.hh"

#include <TFile.h>
#include <TTree.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "HitTreeIOWithInitialInfo.hh"

using namespace anlnext;

namespace comptonsoft
{

namespace fs = std::filesystem;

namespace
{

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
  define_parameter("gain_tp_file", &mod_class::gain_tp_file_);
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

  if (!gain_tp_file_.empty()) {
    const fs::path gain_tp_path =
        resolveCalibrationPath(calibration_config_.config_dir, gain_tp_file_);
    const std::string tpc_time_id =
        timeIdFromTPCTreePath(data_reduction_->tpcTreeFilePath());
    calibration_config_.energy.tp_adc_values =
        interpolatedTestPulseGainsFromCsv(gain_tp_path, tpc_time_id);
    std::cout << "gain_tp_file: " << gain_tp_path << std::endl;
    std::cout << "TPC data time_id for gain interpolation: "
              << tpc_time_id << std::endl;
  } else {
    if (gain_tp_dict_.empty()) {
      throw std::runtime_error(
          "NanoGRAMSCalibration needs either gain_tp_file or gain_tp_hash.");
    }
    calibration_config_.energy.tp_adc_values =
        fixedTestPulseGainsFromHash(gain_tp_dict_);
    std::cout << "gain_tp_hash: using fixed FEC gain values." << std::endl;
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
      resolveCalibrationPath(calibration_config_.config_dir,
                             calibration_config_.energy.gain_info_file);
  const fs::path spline_path =
      resolveCalibrationPath(calibration_config_.config_dir,
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
  tree_io_->setInitialInfo(0.0,
                           vector3_t(0.0, 0.0, 0.0),
                           0.0,
                           vector3_t(0.0, 0.0, 0.0));
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
