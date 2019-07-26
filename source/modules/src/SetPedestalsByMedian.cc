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

#include "SetPedestalsByMedian.hh"
#include "FrameData.hh"
#include "ConstructFrame.hh"

using namespace anlnext;

namespace comptonsoft{

SetPedestalsByMedian::SetPedestalsByMedian()
  : frameBin_(100)
{
}

ANLStatus SetPedestalsByMedian::mod_define()
{
  define_parameter("frame_bin", &mod_class::frameBin_);
  return AS_OK;
}

ANLStatus SetPedestalsByMedian::mod_initialize()
{
  get_module_NC("ConstructFrame", &frame_owner_);
  return AS_OK;
}

ANLStatus SetPedestalsByMedian::mod_begin_run()
{
  FrameData& frameData = frame_owner_->getFrame();
  image_t& pedestals = frameData.getPedestals();
  nx_ = pedestals.shape()[0];
  ny_ = pedestals.shape()[1];
  frameStack_.resize(boost::extents[nx_][ny_]);

  return AS_OK;
}

ANLStatus SetPedestalsByMedian::mod_analyze()
{
  const int frameID = frame_owner_->FrameID();
  FrameData& frameData = frame_owner_->getFrame();
  image_t& pedestals = frameData.getPedestals();
  image_t& rawFrame = frameData.getRawFrame();

  if (frameID<frameBin_) {
    frameData.setBadFrame(true);
    for (int ix=0; ix<nx_; ix++){
      for (int iy=0; iy<ny_; iy++){
        frameStack_[ix][iy].emplace_back(frameID, rawFrame[ix][iy]);
      }
    }
  }

  if (frameID==frameBin_) {
    for (int ix=0; ix<nx_; ix++){
      for (int iy=0; iy<ny_; iy++){
        auto comp = [](const std::pair<int, double>& a, const std::pair<int, double>& b){
          return a.second < b.second;
        };
        std::sort(frameStack_[ix][iy].begin(), frameStack_[ix][iy].end(), comp);          
      }
    }
  }

  if (frameID>=frameBin_) {
    for (int ix=0; ix<nx_; ix++){
      for (int iy=0; iy<ny_; iy++){
        frameStack_[ix][iy].erase(remove_if(frameStack_[ix][iy].begin(), frameStack_[ix][iy].end(),
                                            [frameID, this](std::pair<int, double> x) { return x.first==frameID-frameBin_; }), 
                                  frameStack_[ix][iy].end());
        const std::pair<int, double> p = std::make_pair(frameID, rawFrame[ix][iy]);
        auto comp = [](const std::pair<int, double>& a, const std::pair<int, double>& b){
          return a.second < b.second;
        };
        auto it = std::upper_bound(frameStack_[ix][iy].begin(), frameStack_[ix][iy].end(), p, comp);
        frameStack_[ix][iy].insert(it, p);
        pedestals[ix][iy] = calculateMedian(frameStack_[ix][iy]);
      }
    }
  }

  return AS_OK;
}

double SetPedestalsByMedian::calculateMedian(std::vector<std::pair<int, double>>& v)
{
  const size_t size = v.size();
  if (size%2==1) {
    return v[size/2].second;
  }
  else {
    return 0.5 * (v[size/2].second + v[size/2-1].second);
  }
}

} /* namespace comptonsoft */
