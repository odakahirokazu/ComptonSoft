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
#include "FrameData.hh"

using namespace anlnext;

namespace comptonsoft {

AnalyzeDarkFrame::AnalyzeDarkFrame()
{
  add_alias("AnalyzeDarkFrame");
}

ANLStatus AnalyzeDarkFrame::mod_define()
{
  define_parameter("pedestal_level", &mod_class::pedestal_level_);
  define_parameter("event_threshold", &mod_class::event_threshold_);
  define_parameter("hotpix_threshold", &mod_class::hotPixelThreshold_);
  
  return AS_OK;
}

ANLStatus AnalyzeDarkFrame::mod_initialize()
{
  get_module_NC("ConstructFrame", &frame_owner_);

  return AS_OK;
}

ANLStatus AnalyzeDarkFrame::mod_begin_run()
{
  FrameData& frame = frame_owner_->getFrame();
  frame.setEventThreshold(event_threshold_);
  frame.setHotPixelThreshold(hotPixelThreshold_);
  frame.setPedestals(pedestal_level_);

  return AS_OK;
}

ANLStatus AnalyzeDarkFrame::mod_analyze()
{
  FrameData& frame = frame_owner_->getFrame();
  frame.stack();
  frame.detectHotPixels();

  return AS_OK;
}

ANLStatus AnalyzeDarkFrame::mod_end_run()
{
  FrameData& frame = frame_owner_->getFrame();
  frame.calculatePedestals();
  return AS_OK;
}

} /* namespace comptonsoft */
