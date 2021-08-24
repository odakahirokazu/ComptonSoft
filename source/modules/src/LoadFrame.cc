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

#include "LoadFrame.hh"
#include <algorithm>
#include "FrameData.hh"
#include "ConstructDetector.hh"
#include "DetectorSystem.hh"
#include "VRealDetectorUnit.hh"

using namespace anlnext;

namespace comptonsoft {

LoadFrame::LoadFrame()
{
}

ANLStatus LoadFrame::mod_define()
{
  define_parameter("byte_order", &mod_class::byte_order_);
  define_parameter("odd_row_pixel_shift", &mod_class::odd_row_pixel_shift_);
  define_parameter("start_position", &mod_class::start_position_);
  define_parameter("read_direction_x", &mod_class::read_direction_x_);
  define_parameter("detector_id", &mod_class::detector_id_);
  define_parameter("files", &mod_class::files_);
  
  return AS_OK;
}

ANLStatus LoadFrame::mod_initialize()
{
  ConstructDetector* detectorOwner;
  get_module_NC("ConstructDetector", &detectorOwner);
  VRealDetectorUnit* detector = detectorOwner->getDetectorManager()->getDetectorByID(detector_id_);
  if (detector == nullptr) {
    std::cout << "Detector " << detector_id_ << " does not exist." << std::endl;
    return AS_QUIT;
  }

  if (detector->hasFrameData()) {
    frame_ = detector->getFrameData();
    frame_->setByteOrder(byte_order_);
    frame_->setOddRowPixelShift(odd_row_pixel_shift_);
    frame_->setStartPosition(FrameData::CornerID(start_position_));
    frame_->setReadDirectionX(read_direction_x_);
  }
  else {
    std::cout << "Detector does not have a frame." << std::endl;
    return AS_QUIT;
  }

  return AS_OK;
}

ANLStatus LoadFrame::mod_analyze()
{
  if (isDone()) {
    return AS_QUIT;
  }

  const std::size_t fileIndex = get_loop_index();
  const std::string filename = files_[fileIndex];
  std::cout << "[LoadFrame] filename: " << filename << std::endl;

  frame_->setFrameID(fileIndex);
  bool status = frame_->load(filename);
  if (!status) {
    return AS_ERROR;
  }

  return AS_OK;
}

void LoadFrame::addFile(const std::string& filename)
{
  files_.push_back(filename);
  past_files_.insert(filename);
}

bool LoadFrame::hasFile(const std::string& filename) const
{
  return past_files_.count(filename) != 0;
}

bool LoadFrame::isDone() const
{
  const std::size_t fileIndex = get_loop_index();
  return (fileIndex == files_.size());
}

} /* namespace comptonsoft */
