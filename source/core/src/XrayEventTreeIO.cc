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

#include "XrayEventTreeIO.hh"
#include "AstroUnits.hh"
#include "TTree.h"
#include "CSException.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

XrayEventTreeIO::XrayEventTreeIO()
  : tree_(nullptr),
    eventSize_(5)
{
}

XrayEventTreeIO::~XrayEventTreeIO() = default;

void XrayEventTreeIO::setEventSize(int v)
{
  if (v > MaxEventSize_) {
    throw CSException("XrayEventTreeIO: the event size given is larger than the allowed maximum (9).");
  }

  eventSize_ = v;
}

void XrayEventTreeIO::defineBranches()
{
  tree_->Branch("frameID",    &frameID_,    "frameID/I");
  tree_->Branch("time",       &time_,       "time/D");
  tree_->Branch("detectorID", &detectorID_, "detectorID/I");
  tree_->Branch("ix",         &ix_,         "ix/I");
  tree_->Branch("iy",         &iy_,         "iy/I");
  tree_->Branch("data",       &data_,       "data[9][9]/D");
  tree_->Branch("sumPH",      &sumPH_,      "sumPH/D");
  tree_->Branch("centerPH",   &centerPH_,   "centerPH/D");
  tree_->Branch("weight",     &weight_,     "weight/I");
  tree_->Branch("rank",       &rank_,       "rank/I");
  tree_->Branch("angle",      &angle_,      "angle/D");
  tree_->Branch("grade",      &grade_,      "grade/I");
}

void XrayEventTreeIO::setBranchAddresses()
{
  tree_->SetBranchAddress("frameID",    &frameID_);
  tree_->SetBranchAddress("time",       &time_);
  tree_->SetBranchAddress("detectorID", &detectorID_);
  tree_->SetBranchAddress("ix",         &ix_);
  tree_->SetBranchAddress("iy",         &iy_);
  tree_->SetBranchAddress("data",       &data_);
  tree_->SetBranchAddress("sumPH",      &sumPH_);
  tree_->SetBranchAddress("centerPH",   &centerPH_);
  tree_->SetBranchAddress("weight",     &weight_);
  tree_->SetBranchAddress("rank",       &rank_);
  tree_->SetBranchAddress("angle",      &angle_);
  tree_->SetBranchAddress("grade",      &grade_);
}

int XrayEventTreeIO::fillEvents(const XrayEventContainer& events)
{
  return fillEvents(events.begin(), events.end());
}

int XrayEventTreeIO::fillEvents(const XrayEventCIter events_begin, const XrayEventCIter events_end)
{
  int num = 0;
  for (auto it = events_begin; it!=events_end; ++it) {
    const const_XrayEvent_sptr event = *it;
    frameID_    = event->FrameID();
    time_       = event->Time();
    detectorID_ = event->DetectorID();
    ix_         = event->X();
    iy_         = event->Y();
    sumPH_      = event->SumPH();
    centerPH_   = event->CenterPH();
    weight_     = event->Weight();
    rank_       = event->Rank();
    angle_      = event->Angle();
    grade_      = event->Grade();

    for (int ix=0; ix<MaxEventSize_; ix++) {
      for (int iy=0; iy<MaxEventSize_; iy++) {
        data_[ix][iy] = 0.0;
      }
    }
    const comptonsoft::image_t& data = event->Data();
    const int nx = data.shape()[0];
    const int ny = data.shape()[1];
    for (int ix=0; ix<nx; ix++) {
      for (int iy=0; iy<ny; iy++) {
        data_[ix][iy] = data[ix][iy];
      }
    }
    
    tree_->Fill();
    num++;
  }
  return num;
}

XrayEvent_sptr XrayEventTreeIO::retrieveEvent() const
{
  const int eventSize = eventSize_;
  XrayEvent_sptr event(new XrayEvent(eventSize));

  event->setFrameID(frameID_);
  event->setTime(time_);
  event->setDetectorID(detectorID_);
  event->setX(ix_);
  event->setY(iy_);
  event->setSumPH(sumPH_);
  event->setCenterPH(centerPH_ );
  event->setWeight(weight_);
  event->setRank(rank_);
  event->setAngle(angle_);
  event->setGrade(grade_);
  for (int ix=0; ix<eventSize; ix++) {
    for (int iy=0; iy<eventSize; iy++){
      event->setData(ix, iy, data_[ix][iy]);
    }
  }

  return event;
}

std::list<XrayEvent_sptr> XrayEventTreeIO::retrieveEvents(int64_t& entry)
{
  if (frameID_ == -1) {
    tree_->GetEntry(entry);
  }
  
  const int32_t ThisFrameID = frameID_;

  std::list<XrayEvent_sptr> events;
  while (true) {
    tree_->GetEntry(entry);

    if (frameID_ != ThisFrameID) {
      break;
    }

    events.push_back(retrieveEvent());
    entry++;
    if (entry >= tree_->GetEntries()) {
      break;
    }
  }

  return events;
}

} /* namespace comptonsoft */
