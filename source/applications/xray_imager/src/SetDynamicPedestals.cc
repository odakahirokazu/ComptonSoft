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

#include "SetDynamicPedestals.hh"
#include "FrameData.hh"
#include "ConstructFrame.hh"

using namespace anlnext;

namespace comptonsoft {

SetDynamicPedestals::SetDynamicPedestals()
  : frameStoreCapacity_(1)
{
}

ANLStatus SetDynamicPedestals::mod_define()
{
  define_parameter("frame_store_size", &mod_class::frameStoreCapacity_);
  return AS_OK;
}

ANLStatus SetDynamicPedestals::mod_initialize()
{
  VCSModule::mod_initialize();

  if (frameStoreCapacity_ <= 0) {
    std::cout << "Error: frame_store_size <= 0." << std::endl;
    return AS_QUIT_ERROR;
  }

  return AS_OK;
}

ANLStatus SetDynamicPedestals::mod_begin_run()
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
  frameSum_.resize(boost::extents[nx_][ny_]);
  for (int ix=0; ix<nx_; ix++){
    for (int iy=0; iy<ny_; iy++){
      frameSum_[ix][iy] = 0.0;
    }
  }
  frameData_->setBadFrame(true);

  return AS_OK;
}

ANLStatus SetDynamicPedestals::mod_analyze()
{
  image_t& pedestals = frameData_->getPedestals();
  const image_t& rawFrame = frameData_->getRawFrame();

  const std::size_t frameStoreCapacity = frameStoreCapacity_;
  if (frameStore_.size() < frameStoreCapacity) {
    frameData_->setBadFrame(true);
  }
  else {
    frameData_->setBadFrame(false);
    while (frameStore_.size() > frameStoreCapacity) {
      image_t& removeFrame = frameStore_.front();
      for (int ix=0; ix<nx_; ix++) {
        for (int iy=0; iy<ny_; iy++) {
          frameSum_[ix][iy] -= removeFrame[ix][iy];
        }
      }
      frameStore_.pop_front();
    }
    for (int ix=0; ix<nx_; ix++) {
      for (int iy=0; iy<ny_; iy++) {
        pedestals[ix][iy] = frameSum_[ix][iy]/static_cast<double>(frameStoreCapacity);
      }
    }
  }

  frameStore_.emplace_back(rawFrame);
  for (int ix=0; ix<nx_; ix++){
    for (int iy=0; iy<ny_; iy++){
      frameSum_[ix][iy] += rawFrame[ix][iy];
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
