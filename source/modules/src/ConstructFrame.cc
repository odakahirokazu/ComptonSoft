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

#include "ConstructFrame.hh"
#include "FrameData.hh"
#include <memory>
#include "RealDetectorUnit2DPixel.hh"

using namespace anlnext;

namespace comptonsoft {

ConstructFrame::ConstructFrame()
  : ids_{1}
{
  add_alias("ConstructFrame");
}

ANLStatus ConstructFrame::mod_define()
{
  ConstructDetector::mod_define();
  
  define_parameter("num_pixels_x", &mod_class::num_pixels_x_);
  define_parameter("num_pixels_y", &mod_class::num_pixels_y_);
  define_parameter("detector_list", &mod_class::ids_);

  hide_parameter("detector_configuration");
  
  return AS_OK;
}

ANLStatus ConstructFrame::mod_initialize()
{
  DetectorSystem* detectorManager = getDetectorManager();

  for (int i: ids_) {
    auto detector = std::make_unique<RealDetectorUnit2DPixel>();
    detector->setID(i);
    auto frame = createFrameData();
    frame->setPedestals(0.0);
    detector->registerFrameData(std::move(frame));
    detectorManager->addDetector(std::move(detector));
    detectorManager->setConstructed();
  }
  
  return ConstructDetector::mod_initialize();
}

std::unique_ptr<FrameData> ConstructFrame::createFrameData()
{
  return std::make_unique<FrameData>(NumPixelsX(), NumPixelsY());
}

} /* namespace comptonsoft */
