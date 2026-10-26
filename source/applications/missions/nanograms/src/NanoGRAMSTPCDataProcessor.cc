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

namespace comptonsoft {
namespace grams {

bool isTPCDataUsable(int error_flags)
{
  if ((error_flags == 0) || (error_flags == 4)) {
    return true;
  }
  else {
    return false;
  }
}

TPCTreeReader::TPCTreeReader(TTree* tpc_tree)
  : tpc_tree_buffer_(tpc_tree)
{
  if (tpc_tree_buffer_.nEntries() > 0) {
    // the waveform layout (registered DPP channels) is taken from the first entry
    tpc_tree_buffer_.getEntry(0);
  }
}

TPCTreeReader::~TPCTreeReader() = default;

bool TPCTreeReader::readNextEntry(int64_t& raw_event_id)
{
  if (current_entry_ >= tpc_tree_buffer_.nEntries()) {
    return false;
  }

  raw_event_id = current_entry_;
  tpc_tree_buffer_.getEntry(current_entry_);
  current_unix_time_ = tpc_tree_buffer_.representativeUnixTime();
  ++current_entry_;
  return true;
}

} /* namespace grams */
} /* namespace comptonsoft */
