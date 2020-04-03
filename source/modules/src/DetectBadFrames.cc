/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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

#include "DetectBadFrames.hh"

using namespace anlnext;

namespace comptonsoft {

DetectBadFrames::DetectBadFrames()
  : thresholdSigma_(3.0)
{
}

ANLStatus DetectBadFrames::mod_define()
{
  define_parameter("detector_id", &mod_class::detectorID_);
  define_parameter("threshold_sigma", &mod_class::thresholdSigma_);
  define_result("bad_frames", &mod_class::badFrames_);
  return AS_OK;
}

ANLStatus DetectBadFrames::mod_initialize()
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

  return AS_OK;
}

ANLStatus DetectBadFrames::mod_analyze()
{
  const double v = frame_->RawFrameMedian();
  rawFrameMedians_.push_back(v);

  return AS_OK;
}

ANLStatus DetectBadFrames::mod_end_run()
{
  calculateStatistics();
  const double upperTh = average_ + thresholdSigma_*sigma_;
  const double lowerTh = average_ - thresholdSigma_*sigma_;
  for (size_t i=0; i<rawFrameMedians_.size(); i++) {
    const double value = rawFrameMedians_[i];
    if (value<lowerTh || upperTh<value) {
      badFrames_.push_back(i);
    }
  }
  return AS_OK;
}

void DetectBadFrames::calculateStatistics()
{
  const size_t size = rawFrameMedians_.size();
  double sum = 0.0;
  double sum2 = 0.0;
  for (size_t i=0; i<size; i++) {
    const double v = rawFrameMedians_[i];
    sum += v;
    sum2 += v*v;
  }
  average_ = sum/size;
  const double deviation = sum2/size-average_*average_;
  sigma_ = sqrt(deviation);
}

} /* namespace comptonsoft */
