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

/**
 * @file NanoGRAMSQuickLookTreeIO.hh
 * @brief Output writer for NanoGRAMS tpcquicklook trees.
 */

#ifndef COMPTONSOFT_NanoGRAMSQuickLookTreeIO_H
#define COMPTONSOFT_NanoGRAMSQuickLookTreeIO_H 1

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "NanoGRAMSTPCDataProcessor.hh"

class TFile;
class TTree;

namespace comptonsoft
{

class TPCProperty;

namespace grams
{

class QuickLookTreeOutputWriter
{
public:
  explicit QuickLookTreeOutputWriter(const std::string& output_file_path,
                                     const TPCTreeBuffer& first_tpc_tree_buffer,
                                     const TPCProperty& tpc_property,
                                     bool save_waveforms = true,
                                     int flush_entries = 1000);
  ~QuickLookTreeOutputWriter();

  void fillEvent(int64_t raw_event_id,
                 TPCEventType event_type,
                 const TPCTreeBuffer& tpc_tree_buffer,
                 const std::vector<RawFECHit>& hits);
  std::string close();

private:
  void bindBranches();
  void flush();
  void fillChargeMaps(TPCEventType event_type,
                      const TPCTreeBuffer& tpc_tree_buffer);
  void fillRegisteredWaveforms(const TPCTreeBuffer& tpc_tree_buffer);
  double quicklookEnergy(int fec, int ch, double adu_cmn_sub) const;

  std::filesystem::path  output_path_;
  std::unique_ptr<TFile> file_;
  std::unique_ptr<TTree> quicklook_tree_;
  const TPCProperty& tpc_property_;
  bool save_waveforms_ = true;
  int flush_entries_ = 1000;
  int waveform_len_ = 0;
  int waveform_num_channels_ = 0;
  std::string adu_leaflist_;
  std::string energy_leaflist_;
  std::string cmn_leaflist_;
  std::string ti_leaflist_;
  std::string drift_leaflist_;
  std::string wave_compress_leaflist_;
  std::string registered_leaflist_;
  std::string waveform_dpp_ch_leaflist_;
  std::string waveform_leaflist_;

  int64_t raw_event_id_ = 0;
  int16_t event_type_   = 0;
  int16_t cmn_method_   = 0;
  int32_t waveform_len_branch_ = 0;
  int32_t waveform_num_channels_branch_ = 0;
  std::vector<float> adu_cmn_sub_;
  std::vector<float> energy_cmn_sub_;
  std::array<float, NUM_VATA> cmn_{};
  std::array<uint32_t, NUM_VATA> ti_{};
  std::array<uint32_t, NUM_VATA> drift_time_{};
  std::array<uint16_t, NUM_CH_DPP_MAX> wave_compress_{};
  std::array<bool, NUM_CH_DPP_MAX> registered_{};
  std::vector<int16_t> waveform_dpp_ch_;
  std::vector<int16_t> waveform_;
  std::vector<int16_t> hit_pixel_fec_;
  std::vector<int16_t> hit_pixel_ch_;
  std::vector<float> hit_pixel_adu_;
  std::vector<float> hit_pixel_energy_;
  std::vector<int16_t> hit_pixel_cluster_id_;
  std::vector<int16_t> hit_num_pixels_;
};

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSQuickLookTreeIO_H */
