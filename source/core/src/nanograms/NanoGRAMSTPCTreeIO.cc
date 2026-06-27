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

#include "NanoGRAMSTPCTreeIO.hh"

#include <TLeaf.h>
#include <TTree.h>

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace comptonsoft
{
namespace grams
{

namespace
{

int waveformFlattenedLength(TTree* tpc_tree)
{
  const TLeaf* leaf = tpc_tree->GetLeaf("waveform");
  if (!leaf) {
    throw std::runtime_error("Missing waveform branch.");
  }

  const int flattened_len = leaf->GetLenStatic();
  if (flattened_len <= 0) {
    throw std::runtime_error("Unexpected waveform branch length.");
  }
  return flattened_len;
}

int readStaticArrayDimension(const std::string& leaf_title, std::size_t& pos)
{
  const std::size_t begin = leaf_title.find('[', pos);
  const std::size_t end = leaf_title.find(']', begin);
  if (begin == std::string::npos || end == std::string::npos || end <= begin + 1) {
    throw std::runtime_error("Failed to parse waveform branch dimensions: " + leaf_title);
  }

  pos = end + 1;
  return std::stoi(leaf_title.substr(begin + 1, end - begin - 1));
}

std::pair<int, int> waveformStaticArrayShape(TTree* tpc_tree)
{
  const TLeaf* leaf = tpc_tree->GetLeaf("waveform");
  if (!leaf) {
    throw std::runtime_error("Missing waveform branch.");
  }

  const std::string leaf_title = leaf->GetTitle();
  std::size_t pos = 0;
  const int num_channels = readStaticArrayDimension(leaf_title, pos);
  const int waveform_len = readStaticArrayDimension(leaf_title, pos);
  if (num_channels <= 0 || waveform_len <= 0) {
    throw std::runtime_error("Unexpected waveform branch dimensions: " + leaf_title);
  }

  return {num_channels, waveform_len};
}

TPCTreeLayout inspectTPCTreeLayout(TTree* tpc_tree)
{
  TPCTreeLayout tpc_tree_layout;
  tpc_tree_layout.n_entries =
      static_cast<int64_t>(tpc_tree->GetEntries());
  tpc_tree_layout.waveform_flattened_length =
      waveformFlattenedLength(tpc_tree);
  const std::pair<int, int> waveform_shape = waveformStaticArrayShape(tpc_tree);
  tpc_tree_layout.waveform_num_channels = waveform_shape.first;
  tpc_tree_layout.waveform_len = waveform_shape.second;

  if (tpc_tree_layout.waveform_flattened_length !=
      tpc_tree_layout.waveform_num_channels * tpc_tree_layout.waveform_len) {
    throw std::runtime_error("Inconsistent waveform branch dimensions.");
  }

  std::cout << "inspectTPCTreeLayout()" << std::endl;
  std::cout << "n_entries:                   " << tpc_tree_layout.n_entries << std::endl;
  std::cout << "num_dpp_registered_slots:    "
            << tpc_tree_layout.num_dpp_registered_slots << std::endl;
  std::cout << "waveform_num_channels:       "
            << tpc_tree_layout.waveform_num_channels << std::endl;
  std::cout << "waveform_len:                "
            << tpc_tree_layout.waveform_len << std::endl;
  std::cout << "waveform_flattened_length:   "
            << tpc_tree_layout.waveform_flattened_length << std::endl;
  return tpc_tree_layout;
}

} /* namespace */

TPCTreeBuffer::TPCTreeBuffer(TTree* tpc_tree)
    : tpc_tree_(tpc_tree)
{
  if (!tpc_tree_) {
    throw std::runtime_error("TPCTreeBuffer received a null TTree pointer.");
  }

  layout_ = inspectTPCTreeLayout(tpc_tree_);

  waveform_slot_of_dpp_channel_.fill(-1);
  adc.assign(NUM_VATA * NUM_CH_EACH_VATA, 0);
  drift_time.assign(NUM_VATA, 0);
  ti.assign(NUM_VATA, 0);
  waveform.assign(layout_.waveform_flattened_length, 0);

  bindBranches(tpc_tree_);
}

uint32_t TPCTreeBuffer::representativeUnixTime() const
{
  for (const uint32_t value : unixtime) {
    if (value > 0) {
      return value;
    }
  }
  return 0;
}

void TPCTreeBuffer::getEntry(int64_t entry)
{
  tpc_tree_->GetEntry(entry);
}

void TPCTreeBuffer::updateWaveformLayoutFromRegisteredChannels()
{
  waveform_slot_of_dpp_channel_.fill(-1);

  if (layout_.waveform_num_channels == NUM_CH_DPP_MAX) {
    for (int dpp_ch = 0; dpp_ch < NUM_CH_DPP_MAX; ++dpp_ch) {
      if (registered_channels[dpp_ch]) {
        waveform_slot_of_dpp_channel_[dpp_ch] = dpp_ch;
      }
    }
    return;
  }

  int registered_count = 0;
  for (int dpp_ch = 0; dpp_ch < NUM_CH_DPP_MAX; ++dpp_ch) {
    if (registered_channels[dpp_ch]) {
      waveform_slot_of_dpp_channel_[dpp_ch] = registered_count;
      ++registered_count;
    }
  }

  if (registered_count <= 0 || layout_.waveform_num_channels != registered_count) {
    throw std::runtime_error("Unexpected waveform/registered channel layout.");
  }
}

int TPCTreeBuffer::waveformSlotForDPPChannel(int dpp_ch) const
{
  if (dpp_ch < 0 || dpp_ch >= NUM_CH_DPP_MAX) {
    return -1;
  }
  return waveform_slot_of_dpp_channel_[dpp_ch];
}

void TPCTreeBuffer::bindBranches(TTree* tpc_tree)
{
  tpc_tree->SetBranchAddress("adc",           adc.data());
  tpc_tree->SetBranchAddress("drift_time",    drift_time.data());
  tpc_tree->SetBranchAddress("ti",            ti.data());
  tpc_tree->SetBranchAddress("unixtime",      unixtime.data());
  tpc_tree->SetBranchAddress("waveform",      waveform.data());
  tpc_tree->SetBranchAddress("wave_compress", wave_compress.data());
  tpc_tree->SetBranchAddress("registered",    registered_channels.data());
  tpc_tree->SetBranchAddress("error_flags",   &error_flags);
}

} /* namespace grams */
} /* namespace comptonsoft */
