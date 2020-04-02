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

#include "SetBadFrames.hh"

using namespace anlnext;

namespace comptonsoft{

SetBadFrames::SetBadFrames()
{
}

ANLStatus SetBadFrames::mod_define()
{
  define_parameter("detector_id", &mod_class::detectorID_);
  define_parameter("bad_frames", &mod_class::badFrames_);
  return AS_OK;
}

ANLStatus SetBadFrames::mod_initialize()
{
  VCSModule::mod_initialize();

  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detectorID_);
  if (detector == nullptr) {
    std::cout << "Detector " << detectorID_ << " does not exist." << std::endl;
    return AS_QUIT;
  }

  if (detector->hasFrameData()) {
    frame_ = detector->getFrameData();
  }
  else {
    std::cout << "Detector does not have a frame." << std::endl;
    return AS_QUIT;
  }

  if (badFrames_.empty()) {
    return AS_QUIT;
  }
  
  nextBadFrame_ = badFrames_.begin();

  return AS_OK;
}

ANLStatus SetBadFrames::mod_analyze()
{
  if (nextBadFrame_ == badFrames_.end()) {
    return AS_OK;
  }
  
  const int frameID = frame_->FrameID();
  if (frameID == *nextBadFrame_) {
    frame_->setBadFrame(true);
    ++nextBadFrame_;
  } 
  else {
    frame_->setBadFrame(false);
  }

  return AS_OK;
}

} /* namespace comptonsoft */
