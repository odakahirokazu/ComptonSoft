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

#include "AnalyzeFrame.hh"
#include "FrameData.hh"

using namespace anlnext;

namespace comptonsoft {

AnalyzeFrame::AnalyzeFrame()
  : event_size_(5)
{
  add_alias("AnalyzeFrame");
}

ANLStatus AnalyzeFrame::mod_define()
{
  define_parameter("pedestal_level", &mod_class::pedestal_level_);
  define_parameter("event_threshold", &mod_class::event_threshold_);
  define_parameter("split_threshold", &mod_class::split_threshold_);
  define_parameter("event_size", &mod_class::event_size_);
  
  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_initialize()
{
  get_module_NC("ConstructFrame", &frame_owner_);

  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_begin_run()
{
  FrameData& frame = frame_owner_->getFrame();
  frame.setThresholds(event_threshold_, split_threshold_);
  frame.setPedestals(pedestal_level_);
  frame.setEventSize(event_size_);

  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_analyze()
{
  events_.clear();

  const int frameID = frame_owner_->FrameID();
  FrameData& frame = frame_owner_->getFrame();
  frame.stack();
  frame.subtractPedestals();

  std::vector<comptonsoft::XrayEvent_sptr> es = frame.extractEvents();
  for (auto& event: es) {
    event->setFrameID(frameID);
  }
  std::move(es.begin(), es.end(), std::back_inserter(events_));

  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_end_run()
{
  FrameData& frame = frame_owner_->getFrame();
  frame.calculatePedestals();
  return AS_OK;
}

} /* namespace comptonsoft */
