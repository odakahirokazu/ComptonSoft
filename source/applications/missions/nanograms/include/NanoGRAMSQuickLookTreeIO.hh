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

#ifndef COMPTONSOFT_NanoGRAMSQuickLookTreeIO_H
#define COMPTONSOFT_NanoGRAMSQuickLookTreeIO_H 1

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "NanoGRAMSConstants.hh"

class TFile;
class TTree;

namespace comptonsoft {

class RealDetectorUnitNanoGRAMS;

namespace grams {

enum class TPCEventType : int16_t
{
  Error = -1,
  Other = 0,
  Gamma = 1,
  Cosmic = 2,
  PileUp = 3,
  TimeUp = 4,
  // breakdown of the events that were formerly classified as Other
  ExcludedCore = 5,     // the highest pixel of an FEC is an excluded pixel
  NoCluster = 6,        // no reconstructed cluster
  LightNotGamma = 7,    // light is not gamma-like (GammaRequired mode)
  RejPixelCount = 8,    // all the clusters rejected: number of pixels out of range
  RejCollinear = 9,     // all the clusters rejected: three pixels on a line
  RejMultiCluster = 10, // all the clusters rejected: another cluster in the same FEC
  RejTimeUp = 11,       // all the clusters rejected: drift time over the limit
};

/**
 * Writer of the NanoGRAMS quicklook tree, taking the data from the detector unit.
 * - adu_cmn_sub, cmn: raw ADC and the common mode noise (median) of the MCDs
 * - energy_cmn_sub: EPI of the MCDs (charge x W_ion) [keV]
 * - ti, drift_time: raw counters of the DAQ
 * - waveform: corrected light waveforms (LightData::Waveform) of the valid channels [mV]
 *   (waveform_len is the longest length; shorter channels are padded with their last sample,
 *    and the actual length of each channel is in waveform_ch_len)
 * - light_integrated_charge: light charge integrated in ROI of each DPP channel [C]
 *   (RealDetectorUnitNanoGRAMS::LightIntegratedCharge of the general and pileup analysis channels;
 *    0 for the other channels)
 * - hit_*: pixels of the selected clusters (reconstructed hits)
 * - rej_hit_*: pixels of the rejected clusters (same layout as hit_*)
 * - rej_cluster_type: rejection flags of each rejected cluster (bit0: TimeUp, bit1: PixelCountOutOfRange,
 *   bit2: Collinear, bit3: MultipleClustersInFEC)
 * @date 2026-09-24 | rewritten to take the data from RealDetectorUnitNanoGRAMS
 */
class QuickLookTreeIO
{
public:
  QuickLookTreeIO(const std::string& output_file_path, const RealDetectorUnitNanoGRAMS& detector,
                  bool save_waveforms = true);
  ~QuickLookTreeIO();

  /**
   * @param selected_clusters indices of the reconstructed hits to be written in hit_* branches
   * @param rejected_clusters indices of the reconstructed hits to be written in rej_hit_* branches.
   *        rej_cluster_type holds the rejection flags (bit0: TimeUp, bit1: PixelCountOutOfRange,
   *        bit2: Collinear, bit3: MultipleClustersInFEC) of each rejected cluster.
   */
  void fillEvent(int64_t raw_event_id, TPCEventType event_type, const RealDetectorUnitNanoGRAMS& detector,
                 const std::vector<int>& selected_clusters, const std::vector<int>& rejected_clusters);
  std::string close();

private:
  // pixels of the clusters; cluster_id is the index in the list of the clusters written
  struct ClusterBranches
  {
    std::vector<int16_t> pixel_fec;
    std::vector<int16_t> pixel_ch;
    std::vector<float> pixel_adu;
    std::vector<float> pixel_energy;
    std::vector<int16_t> pixel_cluster_id;
    std::vector<int16_t> num_pixels;

    void clear()
    {
      pixel_fec.clear();
      pixel_ch.clear();
      pixel_adu.clear();
      pixel_energy.clear();
      pixel_cluster_id.clear();
      num_pixels.clear();
    }
  };

  void bindBranches();
  void bindClusterBranches(const std::string& prefix, ClusterBranches& branches);
  void setClusterBranchAddresses(const std::string& prefix, ClusterBranches& branches);
  void fillClusters(const RealDetectorUnitNanoGRAMS& detector, const std::vector<int>& clusters,
                    ClusterBranches& branches) const;
  void setBranchAddresses();
  void flush();
  void fillChargeMaps(const RealDetectorUnitNanoGRAMS& detector);
  void fillWaveforms(const RealDetectorUnitNanoGRAMS& detector);
  static std::vector<int16_t> validLightChannels(const RealDetectorUnitNanoGRAMS& detector);

  std::filesystem::path output_path_;
  std::unique_ptr<TFile> file_;
  std::unique_ptr<TTree> quicklook_tree_;
  bool save_waveforms_ = true;
  int waveform_len_ = 0;
  int waveform_num_channels_ = 0;
  std::string adu_leaflist_;
  std::string energy_leaflist_;
  std::string cmn_leaflist_;
  std::string ti_leaflist_;
  std::string drift_leaflist_;
  std::string wave_compress_leaflist_;
  std::string light_integrated_charge_leaflist_;
  std::string light_peak_value_leaflist_;
  std::string waveform_dpp_ch_leaflist_;
  std::string waveform_ch_len_leaflist_;
  std::string waveform_leaflist_;

  int64_t raw_event_id_ = 0;
  int16_t event_type_ = 0;
  int16_t cmn_method_ = 0;
  int32_t waveform_len_branch_ = 0;
  int32_t waveform_num_channels_branch_ = 0;
  std::vector<float> adu_cmn_sub_;
  std::vector<float> energy_cmn_sub_;
  std::array<float, NUM_VATA> cmn_{};
  std::array<uint32_t, NUM_VATA> ti_{};
  std::array<uint32_t, NUM_VATA> drift_time_{};
  std::array<uint16_t, NUM_CH_DPP_MAX> wave_compress_{};
  std::array<double, NUM_CH_DPP_MAX> light_integrated_charge_{};
  std::array<double, NUM_CH_DPP_MAX> light_peak_value_{};
  std::vector<int16_t> waveform_dpp_ch_;
  std::vector<int32_t> waveform_ch_len_;
  std::vector<float> waveform_;
  ClusterBranches hit_;     // selected clusters
  ClusterBranches rej_hit_; // rejected clusters
  std::vector<uint32_t> rej_cluster_type_;
};

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSQuickLookTreeIO_H */
