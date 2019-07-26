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

#include "MakeBadFrames.hh"
#include <fstream>
#include "FrameData.hh"

using namespace anlnext;

namespace comptonsoft{

MakeBadFrames::MakeBadFrames()
  : filename_("badframes.txt"),
    thresholdSigma_(3.0)
{
}

ANLStatus MakeBadFrames::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  define_parameter("threshold_sigma", &mod_class::thresholdSigma_);
  return AS_OK;
}

ANLStatus MakeBadFrames::mod_initialize()
{
  get_module("ConstructFrame", &frame_owner_);
  return AS_OK;
}

ANLStatus MakeBadFrames::mod_analyze()
{
  const FrameData& frameData = frame_owner_->getFrame();
  const double v = frameData.rawFrameMedian();
  rawFrameMedian_.push_back(v);

  return AS_OK;
}

ANLStatus MakeBadFrames::mod_end_run()
{
  calculateStatistics();
  const double upperTh = average_ + thresholdSigma_*sigma_;
  const double lowerTh = average_ - thresholdSigma_*sigma_;
  std::ofstream outputfile(filename_.c_str());
  for (size_t i=0; i<rawFrameMedian_.size(); i++){
    if (rawFrameMedian_[i]>upperTh || rawFrameMedian_[i]<lowerTh){
      outputfile<<i;
      outputfile<<"\n";
    }
  }
  outputfile.close();
  return AS_OK;
}

void MakeBadFrames::calculateStatistics()
{
  const size_t size = rawFrameMedian_.size();
  double sum = 0.0;
  double sum2 = 0.0;
  for (size_t i=0; i<size; i++) {
    sum += rawFrameMedian_[i];
    sum2 += rawFrameMedian_[i]*rawFrameMedian_[i];
  }
  average_ = sum/size;
  const double deviation = sum2/size-average_*average_;
  sigma_ = sqrt(deviation);
}

} /* namespace comptonsoft */
