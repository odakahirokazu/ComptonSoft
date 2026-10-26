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

#include <TTree.h>

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "boost/format.hpp"
namespace comptonsoft {
namespace grams {
TPCTreeBuffer::TPCTreeBuffer(TTree* tpc_tree)
  : tpc_tree_(tpc_tree)
{
  if (!tpc_tree_) {
    throw std::runtime_error("TPCTreeBuffer received a null TTree pointer.");
  }

  adc.assign(NUM_VATA * NUM_CH_EACH_VATA, 0);
  drift_time.assign(NUM_VATA, 0);
  ti.assign(NUM_VATA, 0);

  bindBranches(tpc_tree_);
}

int64_t TPCTreeBuffer::nEntries() const
{
  return static_cast<int64_t>(tpc_tree_->GetEntries());
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

void TPCTreeBuffer::bindBranches(TTree* tpc_tree)
{
  tpc_tree->SetBranchAddress("adc", adc.data());
  tpc_tree->SetBranchAddress("drift_time", drift_time.data());
  tpc_tree->SetBranchAddress("ti", ti.data());
  tpc_tree->SetBranchAddress("unixtime", unixtime.data());
  for (int i = 0; i < NUM_CH_DPP_MAX; ++i) {
    const auto waveform_name = boost::format("waveform_%1%") % i;
    tpc_tree->SetBranchAddress(waveform_name.str().c_str(), waveform[i].data());
    const auto waveform_length = boost::format("wave_num_ch%1%") % i;
    tpc_tree->SetBranchAddress(waveform_length.str().c_str(), &wave_num[i]);
  }
  tpc_tree->SetBranchAddress("wave_compress", wave_compress.data());
  tpc_tree->SetBranchAddress("error_flags", &error_flags);
}

} /* namespace grams */
} /* namespace comptonsoft */
