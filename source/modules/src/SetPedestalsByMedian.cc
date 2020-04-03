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

namespace comptonsoft {

SetPedestalsByMedian::SetPedestalsByMedian()
  : frameBin_(100)
{
}

ANLStatus SetPedestalsByMedian::mod_define()
{
  define_parameter("detector_id", &mod_class::detectorID_);
  define_parameter("frame_bin", &mod_class::frameBin_);
  return AS_OK;
}

ANLStatus SetPedestalsByMedian::mod_begin_run()
{
  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID_);
  if (detector == nullptr) {
    std::cout << "Detector " << detectorID_ << " does not exist." << std::endl;
    return AS_QUIT;
  }

  if (detector->hasFrameData()) {
    frameData_ = detector->getFrameData();
  }
  else {
    std::cout << "Detector does not have a frame." << std::endl;
    return AS_QUIT;
  }

  nx_ = frameData_->NumPixelsX();
  ny_ = frameData_->NumPixelsY();
  frameStack_.resize(boost::extents[nx_][ny_]);

  return AS_OK;
}

ANLStatus SetPedestalsByMedian::mod_analyze()
{
  const int frameID = frameData_->FrameID();
  image_t& pedestals = frameData_->getPedestals();
  image_t& rawFrame = frameData_->getRawFrame();

  if (frameID<frameBin_) {
    frameData_->setBadFrame(true);
    for (int ix=0; ix<nx_; ix++){
      for (int iy=0; iy<ny_; iy++){
        frameStack_[ix][iy].emplace_back(frameID, rawFrame[ix][iy]);
      }
    }
  }

  if (frameID==frameBin_) {
    frameData_->setBadFrame(false);
    for (int ix=0; ix<nx_; ix++){
      for (int iy=0; iy<ny_; iy++){
        auto comp = [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
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
