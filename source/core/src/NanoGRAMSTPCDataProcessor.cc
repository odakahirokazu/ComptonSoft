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

#include "NanoGRAMSTPCDataProcessor.hh"
#include "NanoGRAMSLightAnalysis.hh"

#include <TFile.h>
#include <TLeaf.h>
#include <TTree.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <utility>

namespace comptonsoft
{
namespace grams
{

namespace
{

int lowerBoundTimeIndex(double time_window,
                        double trigger_delay,
                        double dt,
                        int waveform_len)
{
  const double raw = (time_window + trigger_delay) / dt;
  int idx = static_cast<int>(std::ceil(raw - 1e-12));
  if (idx < 0) {
    idx = 0;
  }
  if (idx > waveform_len) {
    idx = waveform_len;
  }
  return idx;
}

double waveCompressToTimebin(uint16_t wave_compress)
{
    //if((0<=wave_compress)&&(wave_compress<=8)){
    //    return std::ldexp(1.0, static_cast<int>(wave_compress));
    //} else{
    //    std::cout << "Warning: wave_compress < 0 or wavecompress > 8" << std::endl;
    //    return 1.0;
    //}
    //return 32.0 * unit::ns; //temporary
    return static_cast<double>(wave_compress) * unit::ns;
}

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

double lowerMean(const PixelADU& values, int n_values)
{
  std::array<double, NUM_CH_EACH_VATA> sorted = values;
  std::sort(sorted.begin(), sorted.end());
  n_values = std::clamp(n_values, 1, NUM_CH_EACH_VATA);
  const double sum = std::accumulate(sorted.begin(),
                                     sorted.begin() + n_values,
                                     0.0);
  return sum / static_cast<double>(n_values);
}

std::pair<int, double> findCoreChannel(const PixelADU& adu_cmn_sub,
                                       const PixelMask& mask)
{
  int core_ch = 0;
  double core_value = -std::numeric_limits<double>::infinity();

  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    const double value = mask[ch] ? adu_cmn_sub[ch] :
                                    -std::numeric_limits<double>::infinity();
    if (value > core_value) {
      core_value = value;
      core_ch = ch;
    }
  }

  return {core_ch, core_value};
}

bool isPeripheryPixel(const FECChannelGeometry& geom, int fec, int ch)
{
  const auto& periphery = geom.periphery[fec];
  return std::find(periphery.begin(), periphery.end(), ch) != periphery.end();
}

bool containsPixel(const std::vector<std::pair<int, int>>& pixels,
                   int fec,
                   int ch)
{
  return std::find(pixels.begin(), pixels.end(), std::pair<int, int>{fec, ch}) !=
         pixels.end();
}

void addPixelIfNew(std::vector<std::pair<int, int>>& pixels, int fec, int ch)
{
  if (!containsPixel(pixels, fec, ch)) {
    pixels.push_back({fec, ch});
  }
}

bool collinear3Global(const FECChannelGeometry& geom,
                      const std::vector<std::pair<int, int>>& pixels)
{
  const auto [x1, y1] = geom.global_xy_of_ch[pixels[0].first][pixels[0].second];
  const auto [x2, y2] = geom.global_xy_of_ch[pixels[1].first][pixels[1].second];
  const auto [x3, y3] = geom.global_xy_of_ch[pixels[2].first][pixels[2].second];
  return (x2 - x1) * (y3 - y1) == (y2 - y1) * (x3 - x1);
}

int waveformTotalLength(TTree* tpc_tree)
{
  const int total_len = tpc_tree->GetLeaf("waveform")->GetLenStatic();
  if (total_len <= 0 || total_len % NUM_CH_DPP_ON != 0) {
    throw std::runtime_error("Unexpected waveform branch length.");
  }
  return total_len;
}

TPCTreeLayout inspectTPCTreeLayout(TTree* tpc_tree)
{
  TPCTreeLayout tpc_tree_layout;
  tpc_tree_layout.n_entries          = static_cast<int64_t>(tpc_tree->GetEntries());
  tpc_tree_layout.waveform_total_len = waveformTotalLength(tpc_tree);
  tpc_tree_layout.waveform_len       = tpc_tree_layout.waveform_total_len / NUM_CH_DPP_ON;

  std::cout << "inspectTPCTreeLayout()" << std::endl;
  std::cout << "n_entries:          "   << tpc_tree_layout.n_entries         << std::endl;
  std::cout << "num_dpp_enable_ch:  "   << tpc_tree_layout.num_dpp_enable_ch << std::endl;
  std::cout << "waveform_len:       "   << tpc_tree_layout.waveform_len      << std::endl;
  return tpc_tree_layout;
}

LightTimingState makeLightTimingState(const TPCTreeLayout& tpc_tree_layout)
{
  LightTimingState light_timing;
  light_timing.pre_roi_index.fill(0);
  light_timing.post_roi_index.fill(tpc_tree_layout.waveform_len);
  return light_timing;
}

void recordLightTimingFromCurrentEntry(LightTimingState& light_timing,
                                       const Config& cfg,
                                       const TPCTreeBuffer& tpc_tree_buffer)
{
  const TPCTreeLayout& tpc_tree_layout = tpc_tree_buffer.layout();
  std::cout << "[INFO] entries=" << tpc_tree_layout.n_entries
            << " waveform_len=" << tpc_tree_layout.waveform_len
            << " delay_counts=" << cfg.delay_counts << "\n";

  for (int light_ch = 0; light_ch < tpc_tree_layout.num_dpp_enable_ch; ++light_ch) {
    const uint16_t wave_compress = tpc_tree_buffer.wave_compress[light_ch];
    const double dt = waveCompressToTimebin(wave_compress);
    const double trigger_delay = static_cast<double>(cfg.delay_counts) * 8.0 * dt;

    light_timing.wave_compress[light_ch] = wave_compress;
    light_timing.timebin[light_ch] = dt;
    light_timing.pre_roi_index[light_ch] =
        lowerBoundTimeIndex(-cfg.pre_roi_window,
                            trigger_delay,
                            dt,
                            tpc_tree_layout.waveform_len);
    light_timing.post_roi_index[light_ch] =
        lowerBoundTimeIndex(cfg.post_roi_window,
                            trigger_delay,
                            dt,
                            tpc_tree_layout.waveform_len);

    std::cout << "[INFO] light_ch=" << light_ch << "\n";
    std::cout << " wave_compress="  << static_cast<int>(light_timing.wave_compress[light_ch]) << "\n";
    std::cout << " timebin_ns="     << light_timing.timebin[light_ch] / unit::ns << "\n";
    std::cout << " pre_roi_index="  << light_timing.pre_roi_index[light_ch]  << "\n";
    std::cout << " post_roi_index=" << light_timing.post_roi_index[light_ch] << "\n";
  }

  light_timing.ready = true;
}

} // namespace

FECTITracker::FECTITracker()
    : overflow_(NUM_VATA, 0),
      prev_ti_(NUM_VATA, 0),
      have_prev_ti_(NUM_VATA, 0)
{}

uint64_t FECTITracker::absoluteTi(int fec, uint32_t ti_value)
{
  if (have_prev_ti_[fec] && ti_value < prev_ti_[fec]) {
    overflow_[fec] += (uint64_t{1} << 32);
  }
  const uint64_t ti_abs = static_cast<uint64_t>(ti_value) + overflow_[fec];
  prev_ti_[fec]         = ti_value;
  have_prev_ti_[fec]    = 1;
  return ti_abs;
}

FECChargeSelector::FECChargeSelector(const Config& cfg)
    : cfg_(cfg),
      geom_(buildFECChannelGeometry()),
      include_diag_(cfg_.pix_max >= 3) //discussion needed
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    masks_[fec] = buildFECMask(cfg_, fec);
    const int perimeter = static_cast<int>(geom_.periphery[fec].size());
    min_periph_hits_[fec] =
        std::max(cfg_.circ_min_hits,
                 static_cast<int>(std::ceil(cfg_.circ_min_ratio * perimeter)));
  }
}

std::vector<RawFECHit>
FECChargeSelector::selectHits(const TPCTreeBuffer& tpc_tree_buffer,
                              FECTITracker& fec_ti_tracker,
                              bool subset_mask,
                              bool light_cosmic,
                              bool light_pileup) const
{
  std::vector<RawFECHit> event_hits;
  event_hits.reserve(static_cast<std::size_t>(NUM_VATA));

  std::array<PixelADU, NUM_VATA> adu_cmn_sub_values{};
  std::array<double, NUM_VATA> drift_times{};
  std::array<uint64_t, NUM_VATA> ti_values{};
  std::array<double, NUM_VATA> core_values{};
  std::array<int, NUM_VATA> fec_order{};

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    PixelADU adu_values{};
    for (int pix = 0; pix < NUM_CH_EACH_VATA; ++pix) {
      adu_values[pix] =
          static_cast<double>(tpc_tree_buffer.adc[fec * NUM_CH_EACH_VATA + pix]);
    }

    const double cmn = median64(adu_values);
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      adu_cmn_sub_values[fec][ch] = adu_values[ch] - cmn;
    }

    drift_times[fec] =
        static_cast<double>(tpc_tree_buffer.drift_time[fec]) * kClkToUs * unit::us +
        kDppResponseTime;
    ti_values[fec] = fec_ti_tracker.absoluteTi(fec, tpc_tree_buffer.ti[fec]);
    core_values[fec] = findCoreChannel(adu_cmn_sub_values[fec], masks_[fec]).second;
    fec_order[fec] = fec;
  }

  std::sort(fec_order.begin(), fec_order.end(), [&](int lhs, int rhs) {
    return core_values[lhs] > core_values[rhs];
  });

  std::array<PixelMask, NUM_VATA> claimed_pixels{};
  for (auto& mask : claimed_pixels) {
    mask.fill(0);
  }

  for (int fec : fec_order) {
    RawFECHit hit;
    hit.fec        = fec;
    hit.ti         = ti_values[fec];
    hit.drift_time = drift_times[fec];

    FECSelectionInput selection_input{adu_cmn_sub_values,
                                      drift_times,
                                      claimed_pixels,
                                      fec,
                                      subset_mask,
                                      light_cosmic,
                                      light_pileup};
    const bool accepted = fillSelectedChannels(selection_input, hit);
    if (accepted) {
      event_hits.push_back(std::move(hit));
    }
  }

  return event_hits;
}

bool FECChargeSelector::fillSelectedChannels(const FECSelectionInput& input,
                                             RawFECHit& hit) const
{
  hit.channel_fecs.clear();
  hit.channels.clear();
  hit.adus.clear();

  const int fec = input.fec;
  const PixelMask& mask_in  = masks_[fec];
  const int min_periph_hits = min_periph_hits_[fec];
  const PixelADU& adu_cmn_sub = input.adu_cmn_sub_values[fec];
  const double drift_time = input.drift_times[fec];

  bool pile_up = !std::isfinite(drift_time) || drift_time >= cfg_.drift_time_max;

  int count_periph = 0;
  for (int ch : geom_.periphery[fec]) {
    if (adu_cmn_sub[ch] > cfg_.circ_thr) {
      ++count_periph;
    }
  }

  bool circle_noise = count_periph >= min_periph_hits;
  const double edge_sum = (std::isfinite(adu_cmn_sub[0]) ? adu_cmn_sub[0] : 0.0) +
                          (std::isfinite(adu_cmn_sub[63]) ? adu_cmn_sub[63] : 0.0);
  circle_noise = circle_noise || (edge_sum > cfg_.noise_th);

  bool cosmic = input.light_cosmic;
  pile_up = pile_up || input.light_pileup;

  const auto [core_idx, core_val] = findCoreChannel(adu_cmn_sub, mask_in);
  const bool core_has_val =
      core_val > cfg_.adu_min && !input.claimed_pixels[fec][core_idx];

  int adopted_count = 0;
  bool extra_high = false;
  std::vector<std::pair<int, int>> selected_pixels;
  if (core_has_val) {
    selected_pixels.push_back({fec, core_idx});

    PixelMask allowed{};
    allowed.fill(0);
    allowed[core_idx] = 1;
    for (int ch : geom_.cross_neighbors[fec][core_idx]) {
      allowed[ch] = 1;
    }
    if (include_diag_) {
      for (int ch : geom_.diag_neighbors[fec][core_idx]) {
        allowed[ch] = 1;
      }
    }

    auto addCandidatePixel = [&](int candidate_fec, int ch) {
      if (masks_[candidate_fec][ch] &&
          !input.claimed_pixels[candidate_fec][ch] &&
          input.adu_cmn_sub_values[candidate_fec][ch] > cfg_.spread_thr) {
        addPixelIfNew(selected_pixels, candidate_fec, ch);
      }
    };

    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      if (mask_in[ch] && !allowed[ch] && adu_cmn_sub[ch] > cfg_.adu_min) {
        extra_high = true;
        break;
      }
    }

    for (int ch : geom_.cross_neighbors[fec][core_idx]) {
      addCandidatePixel(fec, ch);
    }
    if (include_diag_) {
      for (int ch : geom_.diag_neighbors[fec][core_idx]) {
        addCandidatePixel(fec, ch);
      }
    }

    const bool can_merge_neighbor_section =
        cfg_.cross_fec_merge_drift_time_tolerance >= 0.0 &&
        isPeripheryPixel(geom_, fec, core_idx);

    auto addNeighborSectionPixel = [&](const std::pair<int, int>& pixel) {
      if (!can_merge_neighbor_section) {
        return;
      }

      const int neighbor_fec = pixel.first;
      const int neighbor_ch  = pixel.second;
      const double drift_delta = std::abs(input.drift_times[neighbor_fec] - drift_time);
      if (std::isfinite(drift_delta) &&
          drift_delta <= cfg_.cross_fec_merge_drift_time_tolerance) {
        addCandidatePixel(neighbor_fec, neighbor_ch);
      }
    };

    for (const auto& pixel : geom_.cross_section_neighbors[fec][core_idx]) {
      addNeighborSectionPixel(pixel);
    }
    if (include_diag_) {
      for (const auto& pixel : geom_.diag_section_neighbors[fec][core_idx]) {
        addNeighborSectionPixel(pixel);
      }
    }

    adopted_count = static_cast<int>(selected_pixels.size());

    if (adopted_count == 3 && collinear3Global(geom_, selected_pixels)) {
      cosmic = true;
    }
  }

  const bool count_ok   = adopted_count >= cfg_.pix_min && adopted_count <= cfg_.pix_max;
  const bool base_valid = !pile_up && !circle_noise && core_has_val;
  const bool event_mask = base_valid && count_ok && !cosmic && !extra_high;

  if (event_mask && input.subset_mask) {
    for (const auto& pixel : selected_pixels) {
      const int selected_fec = pixel.first;
      const int ch = pixel.second;
      input.claimed_pixels[selected_fec][ch] = 1;
      hit.channel_fecs.push_back(static_cast<int16_t>(selected_fec));
      hit.channels.push_back(static_cast<int16_t>(ch));
      hit.adus.push_back(static_cast<float>(input.adu_cmn_sub_values[selected_fec][ch]));
    }
  }

  return event_mask && input.subset_mask && !hit.channels.empty();
}

TPCTreeBuffer::TPCTreeBuffer(TTree* tpc_tree)
    : tpc_tree_(tpc_tree)
{
  if (!tpc_tree_) {
    throw std::runtime_error("TPCTreeBuffer received a null TTree pointer.");
  }

  layout_ = inspectTPCTreeLayout(tpc_tree_);

  wave_compress.assign(NUM_CH_DPP_ON, 0);
  dpp_enable_channels = std::make_unique<bool[]>(NUM_CH_DPP_ON);
  adc.assign(NUM_VATA * NUM_CH_EACH_VATA, 0);
  drift_time.assign(NUM_VATA, 0);
  ti.assign(NUM_VATA, 0);
  waveform.assign(NUM_CH_DPP_ON * layout_.waveform_len, 0);

  bindBranches(tpc_tree_);
}

void TPCTreeBuffer::getEntry(int64_t entry)
{
  tpc_tree_->GetEntry(entry);
}

void TPCTreeBuffer::bindBranches(TTree* tpc_tree)
{
  tpc_tree->SetBranchAddress("adc",           adc.data());
  tpc_tree->SetBranchAddress("drift_time",    drift_time.data());
  tpc_tree->SetBranchAddress("ti",            ti.data());
  tpc_tree->SetBranchAddress("waveform",      waveform.data());
  tpc_tree->SetBranchAddress("wave_compress", wave_compress.data());
  tpc_tree->SetBranchAddress("registered",    dpp_enable_channels.get());
  tpc_tree->SetBranchAddress("error_flags",   &error_flags);
}

TPCTreeReader::TPCTreeReader(TTree* tpc_tree, const Config& cfg)
    : cfg_(cfg),
      tpc_tree_buffer_(tpc_tree),
      fec_selector_(cfg_),
      light_timing_(makeLightTimingState(tpc_tree_buffer_.layout())),
      fec_ti_tracker_()
{
  if (tpc_tree_buffer_.nEntries() > 0) {
    tpc_tree_buffer_.getEntry(0);
    recordLightTimingFromCurrentEntry(light_timing_, cfg_, tpc_tree_buffer_);
  }
}

TPCTreeReader::~TPCTreeReader() = default;

bool TPCTreeReader::processNext(int64_t& raw_event_id,
                                std::vector<RawFECHit>& event_hits)
{
  if (current_entry_ >= tpc_tree_buffer_.nEntries()) {
    return false;
  }

  raw_event_id = current_entry_;
  tpc_tree_buffer_.getEntry(current_entry_);

  //discuss intepretation of the error_flags
  const int err = static_cast<int>(tpc_tree_buffer_.error_flags);
  const bool tpc_ok   = (err == 0);
  const bool light_ok = (err == 0 || err == 4);
  const LightStatus light_status = analyzeLightEvent(cfg_, tpc_tree_buffer_,
                                                     light_timing_, light_ok);
  const bool light_pileup = light_status.hasPileup();
  const bool subset_mask  = tpc_ok && light_status.gamma;

  event_hits = fec_selector_.selectHits(tpc_tree_buffer_,
                                        fec_ti_tracker_,
                                        subset_mask,
                                        light_status.cosmic,
                                        light_pileup);
  if (!tpc_ok) {
    current_event_type_ = TPCEventType::Error;
  } else if (light_pileup) {
    current_event_type_ = TPCEventType::PileUp;
  } else if (light_status.cosmic) {
    current_event_type_ = TPCEventType::Cosmic;
  } else if (light_status.gamma && !event_hits.empty()) {
    current_event_type_ = TPCEventType::Gamma;
  } else {
    current_event_type_ = TPCEventType::Other;
  }
  ++current_entry_;
  return true;
}

RawHitTreeOutputWriter::RawHitTreeOutputWriter(const std::string& output_file_path)
    : output_path_(prepareOutputPath(output_file_path)),
      file_(std::make_unique<TFile>(output_path_.string().c_str(), "RECREATE")),
      rawhit_tree_(std::make_unique<TTree>(kRawHitTreeName, kRawHitTreeName))
{
  if (file_->IsZombie()) {
    throw std::runtime_error("Failed to create output ROOT file: " + output_path_.string());
  }

  // Keep ownership in rawhit_tree_; otherwise ROOT may delete it again with the TFile.
  rawhit_tree_->SetDirectory(nullptr);
  bindBranches();
}

RawHitTreeOutputWriter::~RawHitTreeOutputWriter() = default;

void RawHitTreeOutputWriter::fillEvent(int64_t event_id,
                                       int64_t raw_event_id,
                                       const std::vector<RawFECHit>& hits)
{
  eventid_    = event_id;
  raweventid_ = raw_event_id;
  num_hits_   = static_cast<int32_t>(hits.size());

  for (std::size_t ih=0;ih<hits.size();++ih) {
    const auto& hit = hits[ih];
    ihit_      = static_cast<int16_t>(ih);
    ti_        = static_cast<int64_t>(hit.ti);
    drifttime_ = static_cast<float>(hit.drift_time);

    for (std::size_t j = 0; j < hit.channels.size(); ++j) {
      fecid_ = j < hit.channel_fecs.size() ? hit.channel_fecs[j] :
                                            static_cast<int16_t>(hit.fec);
      ch_  = hit.channels[j];
      adu_ = j < hit.adus.size() ? hit.adus[j] : std::numeric_limits<float>::quiet_NaN();
      rawhit_tree_->Fill();
    }
  }
}

std::string RawHitTreeOutputWriter::close()
{
  file_->cd();
  rawhit_tree_->Write();
  file_->Write();
  const auto entries = rawhit_tree_->GetEntries();
  file_->Close();

  std::cout << "[ROOT] Saved file: " << output_path_.string()
            << " (entries=" << entries << ")\n";
  return output_path_.string();
}

void RawHitTreeOutputWriter::bindBranches()
{
  rawhit_tree_->Branch("eventid",     &eventid_,    "eventid/L");
  rawhit_tree_->Branch("raweventid",  &raweventid_, "raweventid/L");
  rawhit_tree_->Branch("ihit",        &ihit_,       "ihit/S");
  rawhit_tree_->Branch("ti",          &ti_,         "ti/L");
  rawhit_tree_->Branch("num_hits",    &num_hits_,   "num_hits/I");
  rawhit_tree_->Branch("adu",         &adu_,        "adu/F");
  rawhit_tree_->Branch("fecid",       &fecid_,      "fecid/S");
  rawhit_tree_->Branch("ch",          &ch_,         "ch/S");
  rawhit_tree_->Branch("drifttime",   &drifttime_,  "drifttime/F");
}

QuickLookTreeOutputWriter::QuickLookTreeOutputWriter(
    const std::string& output_file_path,
    const TPCTreeLayout& tpc_tree_layout)
    : output_path_(prepareOutputPath(output_file_path)),
      file_(std::make_unique<TFile>(output_path_.string().c_str(), "RECREATE")),
      quicklook_tree_(std::make_unique<TTree>(kQuickLookTreeName, kQuickLookTreeName)),
      waveform_len_(tpc_tree_layout.waveform_len),
      waveform_len_branch_(tpc_tree_layout.waveform_len)
{
  if (file_->IsZombie()) {
    throw std::runtime_error("Failed to create quicklook ROOT file: " +
                             output_path_.string());
  }

  adu_cmn_sub_.assign(NUM_VATA * NUM_CH_EACH_VATA, 0.0f);
  waveform_.assign(NUM_CH_DPP_ON * waveform_len_, 0);
  quicklook_tree_->SetDirectory(nullptr);
  bindBranches();
}

QuickLookTreeOutputWriter::~QuickLookTreeOutputWriter() = default;

void QuickLookTreeOutputWriter::fillEvent(int64_t raw_event_id,
                                          TPCEventType event_type,
                                          const TPCTreeBuffer& tpc_tree_buffer)
{
  if (event_type == TPCEventType::Error) {
    return;
  }

  raw_event_id_ = raw_event_id;
  event_type_   = static_cast<int16_t>(event_type);
  cmn_method_   = (event_type == TPCEventType::Cosmic) ? 1 : 0;

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    ti_[fec]         = tpc_tree_buffer.ti[fec];
    drift_time_[fec] = tpc_tree_buffer.drift_time[fec];
  }

  for (int ch = 0; ch < NUM_CH_DPP_ON; ++ch) {
    wave_compress_[ch] = tpc_tree_buffer.wave_compress[ch];
    registered_[ch]    = tpc_tree_buffer.dpp_enable_channels[ch];
  }
  waveform_ = tpc_tree_buffer.waveform;

  fillCmnSubtractedADU(event_type, tpc_tree_buffer);
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
  adu_leaflist_           = "adu_cmn_sub[" + std::to_string(NUM_VATA) + "][" +
                              std::to_string(NUM_CH_EACH_VATA) + "]/F";
  cmn_leaflist_           = "cmn[" + std::to_string(NUM_VATA) + "]/F";
  ti_leaflist_            = "ti[" + std::to_string(NUM_VATA) + "]/i";
  drift_leaflist_         = "drift_time[" + std::to_string(NUM_VATA) + "]/i";
  wave_compress_leaflist_ = "wave_compress[" + std::to_string(NUM_CH_DPP_ON) + "]/s";
  registered_leaflist_    = "registered[" + std::to_string(NUM_CH_DPP_ON) + "]/O";
  waveform_leaflist_      = "waveform[" + std::to_string(NUM_CH_DPP_ON) + "][" +
                            std::to_string(waveform_len_) + "]/S";

  quicklook_tree_->Branch("raw_event_id", &raw_event_id_, "raw_event_id/L");
  quicklook_tree_->Branch("event_type",   &event_type_,   "event_type/S");
  quicklook_tree_->Branch("cmn_method",   &cmn_method_,   "cmn_method/S");
  quicklook_tree_->Branch("waveform_len", &waveform_len_branch_, "waveform_len/I");
  quicklook_tree_->Branch("adu_cmn_sub",  adu_cmn_sub_.data(),  adu_leaflist_.c_str());
  quicklook_tree_->Branch("cmn",          cmn_.data(),          cmn_leaflist_.c_str());
  quicklook_tree_->Branch("ti",           ti_.data(),           ti_leaflist_.c_str());
  quicklook_tree_->Branch("drift_time",   drift_time_.data(),   drift_leaflist_.c_str());
  quicklook_tree_->Branch("wave_compress", wave_compress_.data(),
                                                                wave_compress_leaflist_.c_str());
  quicklook_tree_->Branch("registered",   registered_.data(),   registered_leaflist_.c_str());
  quicklook_tree_->Branch("waveform",     waveform_.data(),     waveform_leaflist_.c_str());
}

void QuickLookTreeOutputWriter::fillCmnSubtractedADU(
    TPCEventType event_type,
    const TPCTreeBuffer& tpc_tree_buffer)
{
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    PixelADU adu_values{};
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      adu_values[ch] =
          static_cast<double>(tpc_tree_buffer.adc[fec * NUM_CH_EACH_VATA + ch]);
    }

    const double cmn = (event_type == TPCEventType::Cosmic)
        ? lowerMean(adu_values, 10)
        : median64(adu_values);
    cmn_[fec] = static_cast<float>(cmn);

    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      adu_cmn_sub_[fec * NUM_CH_EACH_VATA + ch] =
          static_cast<float>(adu_values[ch] - cmn);
    }
  }
}

} /* namespace grams */
} /* namespace comptonsoft */
