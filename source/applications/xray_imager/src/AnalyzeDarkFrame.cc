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

#include "AnalyzeDarkFrame.hh"
#include <limits>
#include "FrameData.hh"

using namespace anlnext;

namespace comptonsoft {

AnalyzeDarkFrame::AnalyzeDarkFrame()
  : good_pixel_mean_min_(std::numeric_limits<double>::lowest()),
    good_pixel_mean_max_(std::numeric_limits<double>::max()),
    good_pixel_sigma_min_(std::numeric_limits<double>::lowest()),
    good_pixel_sigma_max_(std::numeric_limits<double>::max())
{
  add_alias("AnalyzeDarkFrame");
}

ANLStatus AnalyzeDarkFrame::mod_define()
{
  define_parameter("pedestal_level", &mod_class::pedestal_level_);
  define_parameter("num_exclusion_low", &mod_class::num_exclusion_low_);
  define_parameter("num_exclusion_high", &mod_class::num_exclusion_high_);
  define_parameter("good_pixel_mean_min", &mod_class::good_pixel_mean_min_);
  define_parameter("good_pixel_mean_max", &mod_class::good_pixel_mean_max_);
  define_parameter("good_pixel_sigma_min", &mod_class::good_pixel_sigma_min_);
  define_parameter("good_pixel_sigma_max", &mod_class::good_pixel_sigma_max_);
  
  return AS_OK;
}

ANLStatus AnalyzeDarkFrame::mod_begin_run()
{
  auto& detectors = getDetectorManager()->getDetectors();
  for (auto& detector: detectors) {
    if (detector->hasFrameData()) {
      FrameData* frame = detector->getFrameData();
      frame->setPedestals(pedestal_level_);
      frame->setStatisticsExclusionNumbers(num_exclusion_low_, num_exclusion_high_);
    }
  }

  return AS_OK;
}

ANLStatus AnalyzeDarkFrame::mod_analyze()
{
  auto& detectors = getDetectorManager()->getDetectors();
  for (auto& detector: detectors) {
    if (detector->hasFrameData()) {
      FrameData* frame = detector->getFrameData();
      frame->stack();
    }
  }

  return AS_OK;
}

ANLStatus AnalyzeDarkFrame::mod_end_run()
{
  auto& detectors = getDetectorManager()->getDetectors();
  for (auto& detector: detectors) {
    if (detector->hasFrameData()) {
      FrameData* frame = detector->getFrameData();
      frame->calculateStatistics();
      frame->selectGoodPixels(good_pixel_mean_min_,
                              good_pixel_mean_max_,
                              good_pixel_sigma_min_,
                              good_pixel_sigma_max_);
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
