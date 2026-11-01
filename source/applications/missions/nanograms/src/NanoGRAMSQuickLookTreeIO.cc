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
 *************************************************************************/

#include "NanoGRAMSQuickLookTreeIO.hh"

#include <TFile.h>
#include <TTree.h>

#include "boost/format.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include "FlagDefinition.hh"
#include "LightData.hh"
#include "NanoGRAMSConstants.hh"
#include "NanoGRAMSMultiChannelData.hh"
#include "RealDetectorUnitNanoGRAMS.hh"

namespace comptonsoft {
namespace grams {

namespace {

namespace unit = anlgeant4::unit;

std::filesystem::path prepareOutputPath(const std::string& output_file_path)
{
  std::filesystem::path output_path(output_file_path);
  const auto output_parent = output_path.parent_path();
  if (!output_parent.empty()) {
    std::filesystem::create_directories(output_parent);
  }
  return output_path;
}

// raw ADC - common mode noise (median), i.e. PHA before the temperature correction
double aduCMNSubtracted(const NanoGRAMSMultiChannelData& mcd, int ch)
{
  return static_cast<double>(mcd.getRawADC(ch)) - mcd.getCommonModeNoise();
}

} /* namespace */

std::vector<int16_t> QuickLookTreeIO::validLightChannels(const RealDetectorUnitNanoGRAMS& detector)
{
  std::vector<int16_t> channels;
  for (int dpp_ch = 0; dpp_ch < detector.NumberOfLightData(); ++dpp_ch) {
    if (detector.getLightData(dpp_ch)->isValid()) {
      channels.push_back(static_cast<int16_t>(dpp_ch));
    }
  }
  return channels;
}

QuickLookTreeIO::QuickLookTreeIO(const std::string& output_file_path, const RealDetectorUnitNanoGRAMS& detector,
                                 bool save_waveforms)
  : output_path_(prepareOutputPath(output_file_path)),
    file_(std::make_unique<TFile>(output_path_.string().c_str(), "RECREATE")),
    quicklook_tree_(std::make_unique<TTree>(kQuickLookTreeName, kQuickLookTreeName)),
    save_waveforms_(save_waveforms)
{
  if (file_->IsZombie()) {
    throw std::runtime_error("Failed to create quicklook ROOT file: " + output_path_.string());
  }

  if (save_waveforms_) {
    waveform_dpp_ch_ = validLightChannels(detector);
    // the waveform lengths can differ between the channels (e.g. different wave_compress)
    for (const int16_t dpp_ch : waveform_dpp_ch_) {
      const int len = detector.getLightData(dpp_ch)->NumberOfPoints();
      waveform_ch_len_.push_back(len);
      waveform_len_ = std::max(waveform_len_, len);
    }
  }
  waveform_num_channels_ = static_cast<int>(waveform_dpp_ch_.size());
  waveform_len_branch_ = waveform_len_;
  waveform_num_channels_branch_ = waveform_num_channels_;
  adu_cmn_sub_.assign(NUM_VATA * NUM_CH_EACH_VATA, 0.0f);
  energy_cmn_sub_.assign(NUM_VATA * NUM_CH_EACH_VATA, 0.0f);
  if (save_waveforms_) {
    waveform_.assign(waveform_num_channels_ * waveform_len_, 0.0f);
  }
  quicklook_tree_->SetDirectory(file_.get());
  bindBranches();
}

QuickLookTreeIO::~QuickLookTreeIO() = default;

void QuickLookTreeIO::fillEvent(int64_t raw_event_id, TPCEventType event_type,
                                const RealDetectorUnitNanoGRAMS& detector, const std::vector<int>& selected_clusters,
                                const std::vector<int>& rejected_clusters)
{
  raw_event_id_ = raw_event_id;
  event_type_ = static_cast<int16_t>(event_type);
  cmn_method_ = (event_type == TPCEventType::Cosmic) ? 1 : 0;

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    ti_[fec] = static_cast<uint32_t>(std::max<int64_t>(0, detector.RawTI(fec)));
    drift_time_[fec] = static_cast<uint32_t>(std::max<int64_t>(0, detector.RawDriftTime(fec)));
  }

  for (int dpp_ch = 0; dpp_ch < NUM_CH_DPP_MAX; ++dpp_ch) {
    light_integrated_charge_[dpp_ch] =
        (dpp_ch < detector.NumberOfLightData()) ? detector.LightIntegratedCharge(dpp_ch) / unit::eplus : 0.0;
    light_peak_value_[dpp_ch] =
        (dpp_ch < detector.NumberOfLightData()) ? detector.LightPeakValue(dpp_ch) / unit::volt * 1000.0 : 0.0;
    wave_compress_[dpp_ch] = 0;
    if (dpp_ch < detector.NumberOfLightData() && detector.getLightData(dpp_ch)->isValid()) {
      wave_compress_[dpp_ch] =
          static_cast<uint16_t>(std::lround(detector.getLightData(dpp_ch)->TimeWidth() / unit::ns));
    }
  }
  if (save_waveforms_) {
    fillWaveforms(detector);
  }
  fillChargeMaps(detector);

  fillClusters(detector, selected_clusters, hit_);
  fillClusters(detector, rejected_clusters, rej_hit_);

  // rejection reasons of each rejected cluster (bit0: TimeUp, ..., bit3: MultipleClustersInFEC)
  constexpr uint64_t CLUSTER_TYPE_MSK = flag::NanoGRAMSTimeUp | flag::NanoGRAMSPixelCountOutOfRange |
                                        flag::NanoGRAMSCollinear | flag::NanoGRAMSMultipleClustersInFEC;
  constexpr int CLUSTER_TYPE_SHIFT = 21;
  static_assert(flag::NanoGRAMSTimeUp == (uint64_t{1} << CLUSTER_TYPE_SHIFT));
  rej_cluster_type_.clear();
  for (const int cluster_index : rejected_clusters) {
    const uint64_t flags = detector.getReconstructedHit(cluster_index)->Flags();
    rej_cluster_type_.push_back(static_cast<uint32_t>((flags & CLUSTER_TYPE_MSK) >> CLUSTER_TYPE_SHIFT));
  }

  quicklook_tree_->Fill();
}

void QuickLookTreeIO::fillClusters(const RealDetectorUnitNanoGRAMS& detector, const std::vector<int>& clusters,
                                   ClusterBranches& branches) const
{
  branches.clear();
  const std::vector<std::vector<int>>& correspondence = detector.ClusterCorrespondence();
  for (std::size_t icluster = 0; icluster < clusters.size(); ++icluster) {
    const std::vector<int>& pixels = correspondence.at(clusters[icluster]);
    branches.num_pixels.push_back(static_cast<int16_t>(pixels.size()));

    for (const int pixel_index : pixels) {
      const DetectorHit_sptr& pixel = detector.getDetectorHit(pixel_index);
      const int fec = pixel->DetectorSection();
      const int ch = pixel->DetectorChannel();
      branches.pixel_fec.push_back(static_cast<int16_t>(fec));
      branches.pixel_ch.push_back(static_cast<int16_t>(ch));
      branches.pixel_adu.push_back(
          static_cast<float>(aduCMNSubtracted(*detector.getNanoGRAMSMultiChannelData(fec), ch)));
      branches.pixel_energy.push_back(static_cast<float>(pixel->EPI() / unit::keV));
      branches.pixel_cluster_id.push_back(static_cast<int16_t>(icluster));
    }
  }
}

void QuickLookTreeIO::flush()
{
  file_->cd();
  quicklook_tree_->FlushBaskets();
  quicklook_tree_->AutoSave();
  file_->Flush();
}

std::string QuickLookTreeIO::close()
{
  file_->cd();
  quicklook_tree_->Write();
  file_->Write();
  const auto entries = quicklook_tree_->GetEntries();
  quicklook_tree_->SetDirectory(nullptr);
  file_->Close();

  std::cout << "[ROOT] Saved quicklook file: " << output_path_.string() << " (entries=" << entries << ")\n";
  return output_path_.string();
}

void QuickLookTreeIO::bindBranches()
{
  adu_leaflist_ = (boost::format("adu_cmn_sub[%1%][%2%]/F") % NUM_VATA % NUM_CH_EACH_VATA).str();
  energy_leaflist_ = (boost::format("energy_cmn_sub[%1%][%2%]/F") % NUM_VATA % NUM_CH_EACH_VATA).str();
  cmn_leaflist_ = (boost::format("cmn[%1%]/F") % NUM_VATA).str();
  ti_leaflist_ = (boost::format("ti[%1%]/i") % NUM_VATA).str();
  drift_leaflist_ = (boost::format("drift_time[%1%]/i") % NUM_VATA).str();
  wave_compress_leaflist_ = (boost::format("wave_compress[%1%]/s") % NUM_CH_DPP_MAX).str();
  light_integrated_charge_leaflist_ = (boost::format("light_integrated_charge[%1%]/D") % NUM_CH_DPP_MAX).str();
  light_peak_value_leaflist_ = (boost::format("light_peak_value[%1%]/D") % NUM_CH_DPP_MAX).str();

  quicklook_tree_->Branch("raw_event_id", &raw_event_id_, "raw_event_id/L");
  quicklook_tree_->Branch("event_type", &event_type_, "event_type/S");
  quicklook_tree_->Branch("cmn_method", &cmn_method_, "cmn_method/S");
  quicklook_tree_->Branch("adu_cmn_sub", adu_cmn_sub_.data(), adu_leaflist_.c_str());
  quicklook_tree_->Branch("energy_cmn_sub", energy_cmn_sub_.data(), energy_leaflist_.c_str());
  quicklook_tree_->Branch("cmn", cmn_.data(), cmn_leaflist_.c_str());
  quicklook_tree_->Branch("ti", ti_.data(), ti_leaflist_.c_str());
  quicklook_tree_->Branch("drift_time", drift_time_.data(), drift_leaflist_.c_str());
  quicklook_tree_->Branch("wave_compress", wave_compress_.data(), wave_compress_leaflist_.c_str());
  quicklook_tree_->Branch("light_integrated_charge", light_integrated_charge_.data(),
                          light_integrated_charge_leaflist_.c_str());
  quicklook_tree_->Branch("light_peak_value", light_peak_value_.data(), light_peak_value_leaflist_.c_str());
  if (save_waveforms_) {
    waveform_dpp_ch_leaflist_ = (boost::format("waveform_dpp_ch[%1%]/S") % waveform_num_channels_).str();
    waveform_ch_len_leaflist_ = (boost::format("waveform_ch_len[%1%]/I") % waveform_num_channels_).str();
    waveform_leaflist_ = (boost::format("waveform[%1%][%2%]/F") % waveform_num_channels_ % waveform_len_).str();
    quicklook_tree_->Branch("waveform_len", &waveform_len_branch_, "waveform_len/I");
    quicklook_tree_->Branch("waveform_num_channels", &waveform_num_channels_branch_, "waveform_num_channels/I");
    quicklook_tree_->Branch("waveform_dpp_ch", waveform_dpp_ch_.data(), waveform_dpp_ch_leaflist_.c_str());
    quicklook_tree_->Branch("waveform_ch_len", waveform_ch_len_.data(), waveform_ch_len_leaflist_.c_str());
    quicklook_tree_->Branch("waveform", waveform_.data(), waveform_leaflist_.c_str());
  }
  bindClusterBranches("hit", hit_);
  bindClusterBranches("rej_hit", rej_hit_);
  quicklook_tree_->Branch("rej_cluster_type", &rej_cluster_type_);
}

void QuickLookTreeIO::bindClusterBranches(const std::string& prefix, ClusterBranches& branches)
{
  quicklook_tree_->Branch((prefix + "_pixel_fec").c_str(), &branches.pixel_fec);
  quicklook_tree_->Branch((prefix + "_pixel_ch").c_str(), &branches.pixel_ch);
  quicklook_tree_->Branch((prefix + "_pixel_adu").c_str(), &branches.pixel_adu);
  quicklook_tree_->Branch((prefix + "_pixel_energy").c_str(), &branches.pixel_energy);
  quicklook_tree_->Branch((prefix + "_pixel_cluster_id").c_str(), &branches.pixel_cluster_id);
  quicklook_tree_->Branch((prefix + "_num_pixels").c_str(), &branches.num_pixels);
}

void QuickLookTreeIO::setClusterBranchAddresses(const std::string& prefix, ClusterBranches& branches)
{
  quicklook_tree_->SetBranchAddress((prefix + "_pixel_fec").c_str(), &branches.pixel_fec);
  quicklook_tree_->SetBranchAddress((prefix + "_pixel_ch").c_str(), &branches.pixel_ch);
  quicklook_tree_->SetBranchAddress((prefix + "_pixel_adu").c_str(), &branches.pixel_adu);
  quicklook_tree_->SetBranchAddress((prefix + "_pixel_energy").c_str(), &branches.pixel_energy);
  quicklook_tree_->SetBranchAddress((prefix + "_pixel_cluster_id").c_str(), &branches.pixel_cluster_id);
  quicklook_tree_->SetBranchAddress((prefix + "_num_pixels").c_str(), &branches.num_pixels);
}

void QuickLookTreeIO::setBranchAddresses()
{
  quicklook_tree_->SetBranchAddress("raw_event_id", &raw_event_id_);
  quicklook_tree_->SetBranchAddress("event_type", &event_type_);
  quicklook_tree_->SetBranchAddress("cmn_method", &cmn_method_);
  quicklook_tree_->SetBranchAddress("adu_cmn_sub", adu_cmn_sub_.data());
  quicklook_tree_->SetBranchAddress("energy_cmn_sub", energy_cmn_sub_.data());
  quicklook_tree_->SetBranchAddress("cmn", cmn_.data());
  quicklook_tree_->SetBranchAddress("ti", ti_.data());
  quicklook_tree_->SetBranchAddress("drift_time", &drift_time_);
  quicklook_tree_->SetBranchAddress("wave_compress", wave_compress_.data());
  quicklook_tree_->SetBranchAddress("light_integrated_charge", light_integrated_charge_.data());
  quicklook_tree_->SetBranchAddress("light_peak_value", light_peak_value_.data());
  if (quicklook_tree_->FindBranch("waveform_dpp_ch")) {
    quicklook_tree_->SetBranchAddress("waveform_len", &waveform_len_branch_);
    quicklook_tree_->SetBranchAddress("waveform_num_channels", &waveform_num_channels_branch_);
    quicklook_tree_->SetBranchAddress("waveform_dpp_ch", waveform_dpp_ch_.data());
    if (quicklook_tree_->FindBranch("waveform_ch_len")) {
      quicklook_tree_->SetBranchAddress("waveform_ch_len", waveform_ch_len_.data());
    }
    quicklook_tree_->SetBranchAddress("waveform", waveform_.data());
  }

  setClusterBranchAddresses("hit", hit_);
  setClusterBranchAddresses("rej_hit", rej_hit_);
  quicklook_tree_->SetBranchAddress("rej_cluster_type", &rej_cluster_type_);
}

void QuickLookTreeIO::fillChargeMaps(const RealDetectorUnitNanoGRAMS& detector)
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const NanoGRAMSMultiChannelData& mcd = *detector.getNanoGRAMSMultiChannelData(fec);
    cmn_[fec] = static_cast<float>(mcd.getCommonModeNoise());
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      const int index = fec * NUM_CH_EACH_VATA + ch;
      adu_cmn_sub_[index] = static_cast<float>(aduCMNSubtracted(mcd, ch));
      energy_cmn_sub_[index] = static_cast<float>(mcd.getEPI(ch) / unit::keV);
    }
  }
}

void QuickLookTreeIO::fillWaveforms(const RealDetectorUnitNanoGRAMS& detector)
{
  if (validLightChannels(detector) != waveform_dpp_ch_) {
    throw std::runtime_error(
        "Valid light channels changed; the quicklook waveform layout is fixed by the first event.");
  }

  for (std::size_t output_slot = 0; output_slot < waveform_dpp_ch_.size(); ++output_slot) {
    const std::vector<double>& waveform = detector.getLightData(waveform_dpp_ch_[output_slot])->Waveform();
    const int len = static_cast<int>(waveform.size());
    if (len != waveform_ch_len_[output_slot]) {
      throw std::runtime_error(
          "Light waveform length changed; the quicklook waveform layout is fixed by the first event.");
    }
    // shorter channels are padded with their last sample (empty -> 0)
    const std::size_t output_offset = output_slot * waveform_len_;
    for (int i = 0; i < waveform_len_; ++i) {
      const double value = (len == 0) ? 0.0 : waveform[std::min(i, len - 1)];
      waveform_[output_offset + i] = static_cast<float>(value / (unit::volt / 1000.0));
    }
  }
}

} /* namespace grams */
} /* namespace comptonsoft */
