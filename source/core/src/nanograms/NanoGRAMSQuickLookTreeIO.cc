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

#include "NanoGRAMSQuickLookTreeIO.hh"

#include "NanoGRAMSConstants.hh"
#include "NanoGRAMSTPCProperty.hh"

#include <TFile.h>
#include <TTree.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <format>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace comptonsoft
{
namespace grams
{

namespace
{

std::filesystem::path prepareOutputPath(const std::string& output_file_path)
{
  if (output_file_path.empty()) {
    throw std::runtime_error("Output file path is empty.");
  }

  std::filesystem::path output_path(output_file_path);
  const auto output_parent = output_path.parent_path();
  if (!output_parent.empty()) {
    std::filesystem::create_directories(output_parent);
  }
  return output_path;
}

double median64(const PixelADU& values)
{
  std::array<double, NUM_CH_EACH_VATA> sorted = values;
  std::sort(sorted.begin(), sorted.end());
  return 0.5 * (sorted[31] + sorted[32]);
}

std::vector<int16_t> registeredDPPChannels(const TPCTreeBuffer& tpc_tree_buffer)
{
  std::vector<int16_t> channels;
  channels.reserve(NUM_CH_DPP_MAX);
  for (int dpp_ch = 0; dpp_ch < NUM_CH_DPP_MAX; ++dpp_ch) {
    if (tpc_tree_buffer.registered_channels[dpp_ch]) {
      channels.push_back(static_cast<int16_t>(dpp_ch));
    }
  }
  if (channels.empty()) {
    throw std::runtime_error("No registered DPP channels for tpcquicklook waveform output.");
  }
  return channels;
}

} /* namespace */

QuickLookTreeOutputWriter::QuickLookTreeOutputWriter(
    const std::string& output_file_path,
    const TPCTreeBuffer& first_tpc_tree_buffer,
    const TPCProperty& tpc_property,
    bool save_waveforms)
    : output_path_(prepareOutputPath(output_file_path)),
      file_(std::make_unique<TFile>(output_path_.string().c_str(), "RECREATE")),
      quicklook_tree_(std::make_unique<TTree>(kQuickLookTreeName, kQuickLookTreeName)),
      tpc_property_(tpc_property),
      save_waveforms_(save_waveforms),
      waveform_len_(first_tpc_tree_buffer.layout().waveform_len),
      waveform_dpp_ch_(save_waveforms ? registeredDPPChannels(first_tpc_tree_buffer)
                                       : std::vector<int16_t>{})
{
  if (file_->IsZombie()) {
    throw std::runtime_error("Failed to create quicklook ROOT file: " +
                             output_path_.string());
  }

  waveform_num_channels_ = static_cast<int>(waveform_dpp_ch_.size());
  waveform_len_branch_ = waveform_len_;
  waveform_num_channels_branch_ = waveform_num_channels_;
  adu_cmn_sub_.assign(NUM_VATA * NUM_CH_EACH_VATA, 0.0f);
  energy_cmn_sub_.assign(NUM_VATA * NUM_CH_EACH_VATA, 0.0f);
  if (save_waveforms_) {
    waveform_.assign(waveform_num_channels_ * waveform_len_, 0);
  }
  quicklook_tree_->SetDirectory(nullptr);
  bindBranches();
}

QuickLookTreeOutputWriter::~QuickLookTreeOutputWriter() = default;

void QuickLookTreeOutputWriter::fillEvent(int64_t raw_event_id,
                                          TPCEventType event_type,
                                          const TPCTreeBuffer& tpc_tree_buffer,
                                          const std::vector<RawFECHit>& hits)
{
  raw_event_id_ = raw_event_id;
  event_type_   = static_cast<int16_t>(event_type);
  cmn_method_ = 0;
  if (event_type == TPCEventType::Cosmic) {
    cmn_method_ = 1;
  }

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    ti_[fec]         = tpc_tree_buffer.ti[fec];
    drift_time_[fec] = tpc_tree_buffer.drift_time[fec];
  }

  for (int ch = 0; ch < NUM_CH_DPP_MAX; ++ch) {
    wave_compress_[ch] = tpc_tree_buffer.wave_compress[ch];
    registered_[ch]    = tpc_tree_buffer.registered_channels[ch];
  }
  if (save_waveforms_) {
    fillRegisteredWaveforms(tpc_tree_buffer);
  }
  fillChargeMaps(event_type, tpc_tree_buffer);

  hit_pixel_fec_.clear();
  hit_pixel_ch_.clear();
  hit_pixel_adu_.clear();
  hit_pixel_energy_.clear();
  hit_pixel_cluster_id_.clear();
  hit_num_pixels_.clear();
  for (std::size_t ihit = 0; ihit < hits.size(); ++ihit) {
    const RawFECHit& hit = hits[ihit];
    hit_num_pixels_.push_back(static_cast<int16_t>(hit.channels.size()));
    for (std::size_t j = 0; j < hit.channels.size(); ++j) {
      if (j < hit.channel_fecs.size()) {
        hit_pixel_fec_.push_back(hit.channel_fecs[j]);
      } else {
        hit_pixel_fec_.push_back(static_cast<int16_t>(hit.fec));
      }
      hit_pixel_ch_.push_back(hit.channels[j]);
      if (j < hit.adus.size()) {
        hit_pixel_adu_.push_back(hit.adus[j]);
        hit_pixel_energy_.push_back(static_cast<float>(
            quicklookEnergy(hit_pixel_fec_.back(), hit.channels[j], hit.adus[j]) / unit::keV));
      } else {
        hit_pixel_adu_.push_back(std::numeric_limits<float>::quiet_NaN());
        hit_pixel_energy_.push_back(std::numeric_limits<float>::quiet_NaN());
      }
      hit_pixel_cluster_id_.push_back(static_cast<int16_t>(ihit));
    }
  }

  quicklook_tree_->Fill();
}

std::string QuickLookTreeOutputWriter::close()
{
  file_->cd();
  quicklook_tree_->Write();
  file_->Write();
  const auto entries = quicklook_tree_->GetEntries();
  file_->Close();

  std::cout << "[ROOT] Saved quicklook file: " << output_path_.string()
            << " (entries=" << entries << ")\n";
  return output_path_.string();
}

void QuickLookTreeOutputWriter::bindBranches()
{
  adu_leaflist_             = std::format("adu_cmn_sub[{}][{}]/F", NUM_VATA, NUM_CH_EACH_VATA);
  energy_leaflist_          = std::format("energy_cmn_sub[{}][{}]/F", NUM_VATA, NUM_CH_EACH_VATA);
  cmn_leaflist_             = std::format("cmn[{}]/F", NUM_VATA);
  ti_leaflist_              = std::format("ti[{}]/i", NUM_VATA);
  drift_leaflist_           = std::format("drift_time[{}]/i", NUM_VATA);
  wave_compress_leaflist_   = std::format("wave_compress[{}]/s", NUM_CH_DPP_MAX);
  registered_leaflist_      = std::format("registered[{}]/O", NUM_CH_DPP_MAX);
  quicklook_tree_->Branch("raw_event_id", &raw_event_id_, "raw_event_id/L");
  quicklook_tree_->Branch("event_type",   &event_type_,   "event_type/S");
  quicklook_tree_->Branch("cmn_method",   &cmn_method_,   "cmn_method/S");
  quicklook_tree_->Branch("adu_cmn_sub",  adu_cmn_sub_.data(),  adu_leaflist_.c_str());
  quicklook_tree_->Branch("energy_cmn_sub",
                          energy_cmn_sub_.data(),
                          energy_leaflist_.c_str());
  quicklook_tree_->Branch("cmn",          cmn_.data(),          cmn_leaflist_.c_str());
  quicklook_tree_->Branch("ti",           ti_.data(),           ti_leaflist_.c_str());
  quicklook_tree_->Branch("drift_time",   drift_time_.data(),   drift_leaflist_.c_str());
  quicklook_tree_->Branch("wave_compress", wave_compress_.data(),
                          wave_compress_leaflist_.c_str());
  quicklook_tree_->Branch("registered",   registered_.data(),   registered_leaflist_.c_str());
  if (save_waveforms_) {
    waveform_dpp_ch_leaflist_ = std::format("waveform_dpp_ch[{}]/S", waveform_num_channels_);
    waveform_leaflist_        = std::format("waveform[{}][{}]/S", waveform_num_channels_, waveform_len_);
    quicklook_tree_->Branch("waveform_len", &waveform_len_branch_, "waveform_len/I");
    quicklook_tree_->Branch("waveform_num_channels",
                            &waveform_num_channels_branch_,
                            "waveform_num_channels/I");
    quicklook_tree_->Branch("waveform_dpp_ch",
                            waveform_dpp_ch_.data(),
                            waveform_dpp_ch_leaflist_.c_str());
    quicklook_tree_->Branch("waveform",     waveform_.data(),     waveform_leaflist_.c_str());
  }
  quicklook_tree_->Branch("hit_pixel_fec",        &hit_pixel_fec_);
  quicklook_tree_->Branch("hit_pixel_ch",         &hit_pixel_ch_);
  quicklook_tree_->Branch("hit_pixel_adu",        &hit_pixel_adu_);
  quicklook_tree_->Branch("hit_pixel_energy",     &hit_pixel_energy_);
  quicklook_tree_->Branch("hit_pixel_cluster_id", &hit_pixel_cluster_id_);
  quicklook_tree_->Branch("hit_num_pixels",       &hit_num_pixels_);
}

void QuickLookTreeOutputWriter::fillChargeMaps(
    TPCEventType,
    const TPCTreeBuffer& tpc_tree_buffer)
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    PixelADU adu_values{};
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      adu_values[ch] =
          static_cast<double>(tpc_tree_buffer.adc[fec * NUM_CH_EACH_VATA + ch]);
    }

    double cmn = median64(adu_values);
    cmn_[fec] = static_cast<float>(cmn);

    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      const double adu_cmn_sub = adu_values[ch] - cmn;
      const int index = fec * NUM_CH_EACH_VATA + ch;
      adu_cmn_sub_[index] = static_cast<float>(adu_cmn_sub);
      energy_cmn_sub_[index] =
          static_cast<float>(quicklookEnergy(fec, ch, adu_cmn_sub) / unit::keV);
    }
  }
}

void QuickLookTreeOutputWriter::fillRegisteredWaveforms(
    const TPCTreeBuffer& tpc_tree_buffer)
{
  const std::vector<int16_t> current_channels =
      registeredDPPChannels(tpc_tree_buffer);
  if (current_channels != waveform_dpp_ch_) {
    throw std::runtime_error("Registered DPP channels changed inside one tpctree file.");
  }

  for (std::size_t output_slot = 0; output_slot < waveform_dpp_ch_.size(); ++output_slot) {
    const int dpp_ch = waveform_dpp_ch_[output_slot];
    const int input_slot = tpc_tree_buffer.waveformSlotForDPPChannel(dpp_ch);
    if (input_slot < 0) {
      throw std::runtime_error("Missing waveform slot for registered DPP channel.");
    }

    const int input_offset = input_slot * waveform_len_;
    const int output_offset = static_cast<int>(output_slot) * waveform_len_;
    if (input_offset + waveform_len_ > static_cast<int>(tpc_tree_buffer.waveform.size())) {
      throw std::runtime_error("Input waveform buffer is shorter than expected.");
    }
    std::copy(tpc_tree_buffer.waveform.begin() + input_offset,
              tpc_tree_buffer.waveform.begin() + input_offset + waveform_len_,
              waveform_.begin() + output_offset);
  }
}

double QuickLookTreeOutputWriter::quicklookEnergy(int fec,
                                                  int ch,
                                                  double adu_cmn_sub) const
{
  if (!std::isfinite(adu_cmn_sub) || adu_cmn_sub <= 0.0) {
    return 0.0 * unit::keV;
  }

  const double correction_factor = tpc_property_.temperatureCorrectionFactor(fec);
  if (!std::isfinite(correction_factor) || correction_factor <= 0.0) {
    return std::numeric_limits<double>::quiet_NaN();
  }

  const double corrected_adu = adu_cmn_sub * correction_factor;
  if (corrected_adu <= 0.0) {
    return 0.0 * unit::keV;
  }

  return tpc_property_.convertADC2keVWithSpline3D(fec, ch, corrected_adu);
}

} /* namespace grams */
} /* namespace comptonsoft */
