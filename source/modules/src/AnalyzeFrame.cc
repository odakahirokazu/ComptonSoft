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

#include <fstream>
#include "AnalyzeFrame.hh"
#include "FrameData.hh"

using namespace anlnext;

namespace comptonsoft {

AnalyzeFrame::AnalyzeFrame()
  : event_size_(5),
    gainFile_("gain.txt")
{
  add_alias("AnalyzeFrame");
}

ANLStatus AnalyzeFrame::mod_define()
{
  define_parameter("pedestal_level", &mod_class::pedestal_level_);
  define_parameter("event_threshold", &mod_class::event_threshold_);
  define_parameter("split_threshold", &mod_class::split_threshold_);
  define_parameter("event_size", &mod_class::event_size_);
  define_parameter("set_gain", &mod_class::setGain_);
  define_parameter("gain_file", &mod_class::gainFile_);
  define_parameter("trim_size", &mod_class::trimSize_);
  
  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_initialize()
{
  get_module_NC("ConstructFrame", &frame_owner_);
  get_module_NC("XrayEventCollection", &collection_);

  FrameData& frame = frame_owner_->getFrame();
  const int nx = frame.NumPixelsX();
  const int ny = frame.NumPixelsY();
  gainCoefficient_.resize(boost::extents[nx][ny]);

  if (setGain_) {
    for (int ix=0; ix<nx; ix++) {
      for (int iy=0; iy<ny; iy++) {
        gainCoefficient_[ix][iy] = std::make_tuple(0.0, 1.0, 0.0, 0.0);
      }
    }
    std::ifstream fin(gainFile_);
    int x=0, y=0;
    double c0=0.0, c1=1.0, c2=0.0, c3=0.0;
    while (fin >> x >> y >> c0 >> c1 >> c2 >> c3) {
      if (x<0 || x>nx-1 || y<0 || y>ny-1) {
        return AS_ERROR;
      }
      gainCoefficient_[x][y] = std::make_tuple(c0, c1, c2, c3);
    }
  }

  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_begin_run()
{
  FrameData& frame = frame_owner_->getFrame();
  frame.setEventThreshold(event_threshold_);
  frame.setSplitThreshold(split_threshold_);
  frame.setPedestals(pedestal_level_);
  frame.setEventSize(event_size_);
  frame.setTrimSize(trimSize_);

  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_analyze()
{
  const int frameID = frame_owner_->FrameID();
  FrameData& frame = frame_owner_->getFrame();
  frame.stack();
  frame.subtractPedestals();

  if (setGain_) {
    const int nx = frame.NumPixelsX();
    const int ny = frame.NumPixelsY();
    image_t& frameImage = frame.getFrame();
    for (int ix=0; ix<nx; ix++) {
      for (int iy=0; iy<ny; iy++) {
        const double v = frameImage[ix][iy];
        const double& c0 = std::get<0>(gainCoefficient_[ix][iy]);
        const double& c1 = std::get<1>(gainCoefficient_[ix][iy]);
        const double& c2 = std::get<2>(gainCoefficient_[ix][iy]);
        const double& c3 = std::get<3>(gainCoefficient_[ix][iy]);
        frameImage[ix][iy] = c0 + c1*v + c2*v*v+ c3*v*v*v;
      }
    }
  }

  std::vector<comptonsoft::XrayEvent_sptr> es = frame.extractEvents();
  for (auto& event: es) {
    event->setFrameID(frameID);
    collection_->insertEvent(event);
  }

  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_end_run()
{
  FrameData& frame = frame_owner_->getFrame();
  frame.calculatePedestals();
  return AS_OK;
}

} /* namespace comptonsoft */
