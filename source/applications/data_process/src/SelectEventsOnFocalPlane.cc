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

#include "SelectEventsOnFocalPlane.hh"
#include "AstroUnits.hh"
#include "BasicComptonEvent.hh"
#include "EventReconstruction.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

SelectEventsOnFocalPlane::SelectEventsOnFocalPlane()
  : detector_id_(0),
    region_type_keyword_("rectangle"),
    region_type_(Region_t::Rectangle),
    center_(0.0, 0.0),
    size_x_(0.0),
    size_y_(0.0),
    radius_(0.0),
    radius_squared_(0.0)
{
}

ANLStatus SelectEventsOnFocalPlane::mod_define()
{
  define_parameter("detector_id", &mod_class::detector_id_);
  define_parameter("region_type", &mod_class::region_type_keyword_);
  define_parameter("center", &mod_class::center_, unit::cm, "cm");
  define_parameter("size_x", &mod_class::size_x_, unit::cm, "cm");
  define_parameter("size_y", &mod_class::size_y_, unit::cm, "cm");
  define_parameter("radius", &mod_class::radius_, unit::cm, "cm");

  return AS_OK;
}

ANLStatus SelectEventsOnFocalPlane::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module("EventReconstruction", &event_reconstruction_);

  define_evs("SelectEventsOnFocalPlane:Exception");

  if (region_type_keyword_=="rectangle") {
    region_type_ = Region_t::Rectangle;
  }
  else if (region_type_keyword_=="circle") {
    region_type_ = Region_t::Circle;
  }
  else {
    std::cout << "Unknown region type: " << region_type_keyword_ << std::endl;
    return AS_QUIT_ERROR;
  }

  radius_squared_ = radius_ * radius_;

  return AS_OK;
}

ANLStatus SelectEventsOnFocalPlane::mod_analyze()
{
  if (event_reconstruction_->NumberOfReconstructedEvents()!=1) {
    set_evs("SelectEventsOnFocalPlane:Exception");
    return AS_SKIP;
  }

  const_BasicComptonEvent_sptr comptonEvent = event_reconstruction_->getReconstructedEvents()[0];

  if (comptonEvent->Hit1DetectorID() != detector_id_) {
    return AS_OK;
  }

  const double x = comptonEvent->Hit1PositionX();
  const double y = comptonEvent->Hit1PositionY();

  if (region_type_==Region_t::Rectangle) {
    const double x0 = center_.x() - 0.5*size_x_;
    const double x1 = center_.x() + 0.5*size_x_;
    if (x < x0 || x1 < x) {
      return AS_SKIP;
    }

    const double y0 = center_.y() - 0.5*size_y_;
    const double y1 = center_.y() + 0.5*size_y_;
    if (y < y0 || y1 < y) {
      return AS_SKIP;
    }
  }
  else if (region_type_==Region_t::Circle) {
    const double dx = x-center_.x();
    const double dy = y-center_.y();
    if (dx*dx+dy*dy > radius_squared_) {
      return AS_SKIP;
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
