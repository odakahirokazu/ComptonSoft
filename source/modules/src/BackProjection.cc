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

#include "BackProjection.hh"

#include <cmath>
#include <random>
#include "AstroUnits.hh"
#include "TDirectory.h"
#include "BasicComptonEvent.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;
namespace constant = anlgeant4::constant;

namespace comptonsoft
{

BackProjection::BackProjection()
  : event_reconstruction_module_(nullptr),
    plane_normal_(0.0, 0.0, 1.0),
    plane_point_(0.0*unit::cm, 0.0*unit::cm, 10.0*unit::cm),
    num_pixel_x_(256), num_pixel_y_(256),
    range_x1_(-4.0*unit::cm), range_x2_(+4.0*unit::cm),
    range_y1_(-4.0*unit::cm), range_y2_(+4.0*unit::cm),
    pixel_unit_(unit::cm), pixel_unit_name_("cm"),
    num_points_(1000),
    arm_spread_(0.0*unit::degree)
{
}

BackProjection::~BackProjection() = default;

ANLStatus BackProjection::mod_define()
{
  define_parameter("plane_normal", &mod_class::plane_normal_);
  define_parameter("plane_point", &mod_class::plane_point_, 1, pixel_unit_name_);
  define_parameter("num_pixel_x", &mod_class::num_pixel_x_);
  define_parameter("num_pixel_y", &mod_class::num_pixel_y_);
  define_parameter("x_min", &mod_class::range_x1_, 1, pixel_unit_name_);
  define_parameter("x_max", &mod_class::range_x2_, 1, pixel_unit_name_);
  define_parameter("y_min", &mod_class::range_y1_, 1, pixel_unit_name_);
  define_parameter("y_max", &mod_class::range_y2_, 1, pixel_unit_name_);
  define_parameter("num_points", &mod_class::num_points_);
  define_parameter("arm", &mod_class::arm_spread_, unit::degree, "degree");
  
  return AS_OK;
}

ANLStatus BackProjection::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module_NC("EventReconstruction", &event_reconstruction_module_);
  
  mkdir();
  
  hist_bp_all_ = new TH2D("h_bp_all","Back Projection (All)",
                          num_pixel_x_, range_x1_, range_x2_,
                          num_pixel_y_, range_y1_, range_y2_);

  const std::vector<HitPattern>& hitpat_vec = getDetectorManager()->getHitPatterns();
  hist_vec_.resize(hitpat_vec.size());
  for (size_t i=0; i<hitpat_vec.size(); i++) {
    std::string hist_name = "h_bp_";
    std::string hist_title = "BackProjection (";
    hist_name += hitpat_vec[i].ShortName();
    hist_title += hitpat_vec[i].Name();
    hist_title += ")";
    hist_vec_[i] = new TH2D(hist_name.c_str(), hist_title.c_str(),
                          num_pixel_x_, range_x1_, range_x2_,
                          num_pixel_y_, range_y1_, range_y2_);
  }

  return AS_OK;
}

ANLStatus BackProjection::mod_analyze()
{
  static std::mt19937 randgen;
  std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
  std::normal_distribution<double> normal_dist(0.0, ARMSpread());

  const std::vector<BasicComptonEvent_sptr> events = getEventReconstructionModule()->getReconstructedEvents();
  for (const auto& event: events) {
    const double fraction = event->ReconstructionFraction();
    const size_t num_points = NumPoints();
    const double weight = fraction/num_points;

    const vector3_t cone_vertex = event->ConeVertex();
    const vector3_t cone_axis = event->ConeAxis();
    const vector3_t cone_axis_ortho = cone_axis.orthogonal();
    const double costhetaE = event->CosThetaE();
    const double thetaE0 = std::acos(costhetaE);

    for (size_t i=0; i<num_points; i++) {
      vector3_t cone1(cone_axis);
      const double delta_thetaE = normal_dist(randgen);
      const double thetaE = thetaE0 + delta_thetaE;
      cone1.rotate(thetaE, cone_axis_ortho);
      vector3_t cone_sample = cone1;
      const double phi = constant::twopi * uniform_dist(randgen);
      cone_sample.rotate(phi, cone_axis);

      vector3_t cone_projected;
      const bool positive = sectionConeAndPlane(cone_vertex, cone_sample, cone_projected);
      if (positive) {
        fillImage(cone_projected.x()/PixelUnit(), cone_projected.y()/PixelUnit(), weight);
      }
    }
  }
  
  return AS_OK;
}

void BackProjection::fillImage(double x, double y, double weight)
{
  hist_bp_all_->Fill(x, y, weight);
  for(size_t i=0; i<hist_vec_.size(); i++) {
    if(event_reconstruction_module_->HitPatternFlag(i)) {
      hist_vec_[i]->Fill(x, y, weight);
    }
  }
}

bool BackProjection::sectionConeAndPlane(const vector3_t& vertex, const vector3_t& cone, vector3_t& cone_projected)
{
  const double t = (plane_normal_*(plane_point_-vertex)) / (plane_normal_*cone);
  cone_projected = vertex + t*cone;
  if (t < 0.0) { return false; }
  return true;
}

} /* namespace comptonsoft */
