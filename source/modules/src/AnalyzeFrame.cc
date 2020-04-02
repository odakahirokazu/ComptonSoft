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
#include "VRealDetectorUnit.hh"
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
  define_parameter("trim_size", &mod_class::trim_size_);
  define_parameter("gain_correction", &mod_class::gain_correction_);
  
  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module_NC("XrayEventCollection", &collection_);

  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_begin_run()
{
  auto& detectors = getDetectorManager()->getDetectors();
  for (auto& detector: detectors) {
    if (detector->hasFrameData()) {
      FrameData* frame = detector->getFrameData();
      frame->setEventThreshold(event_threshold_);
      frame->setSplitThreshold(split_threshold_);
      frame->setPedestals(pedestal_level_);
      frame->setEventSize(event_size_);
      frame->setTrimSize(trim_size_);
    }
  }

  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_analyze()
{
  auto& detectors = getDetectorManager()->getDetectors();
  for (auto& detector: detectors) {
    if (detector->hasFrameData()) {
      FrameData* frame = detector->getFrameData();
      const int frameID = frame->FrameID();
      frame->stack();
      frame->subtractPedestals();
      if (gain_correction_) {
        frame->correctGains();
      }

      std::vector<comptonsoft::XrayEvent_sptr> es = frame->extractEvents();
      const int detectorID = detector->getID();
      for (auto& event: es) {
        event->setFrameID(frameID);
        event->setDetectorID(detectorID);
        collection_->insertEvent(event);
      }
    }
  }

  return AS_OK;
}

ANLStatus AnalyzeFrame::mod_end_run()
{
  auto& detectors = getDetectorManager()->getDetectors();
  for (auto& detector: detectors) {
    if (detector->hasFrameData()) {
      FrameData* frame = detector->getFrameData();
      frame->calculatePedestals();
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
