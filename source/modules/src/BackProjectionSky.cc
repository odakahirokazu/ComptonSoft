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

#include "BackProjectionSky.hh"

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

BackProjectionSky::BackProjectionSky()
  : image_center_theta_(0.0*unit::degree),
    image_center_phi_(0.0*unit::degree),
    image_yaxis_theta_(90.0*unit::degree),
    image_yaxis_phi_(90.0*unit::degree)
{
  setUnit(unit::degree, "degree");
}

BackProjectionSky::~BackProjectionSky() = default;

ANLStatus BackProjectionSky::mod_define()
{
  BackProjection::mod_define();

  unregister_parameter("plane_normal");
  unregister_parameter("plane_point");
  
  define_parameter("image_center_theta", &mod_class::image_center_theta_, unit::degree, "degree");
  define_parameter("image_center_phi",   &mod_class::image_center_phi_,   unit::degree, "degree");
  define_parameter("image_yaxis_theta",  &mod_class::image_yaxis_theta_,  unit::degree, "degree");
  define_parameter("image_yaxis_phi",    &mod_class::image_yaxis_phi_,    unit::degree, "degree");

  return AS_OK;
}

ANLStatus BackProjectionSky::mod_initialize()
{
  using std::cos, std::sin;

  BackProjection::mod_initialize();

  const vector3_t zaxis(sin(image_center_theta_)*cos(image_center_phi_),
                        sin(image_center_theta_)*sin(image_center_phi_),
                        cos(image_center_theta_));
  const vector3_t yaxis_proposed(sin(image_yaxis_theta_)*cos(image_yaxis_phi_),
                                 sin(image_yaxis_theta_)*sin(image_yaxis_phi_),
                                 cos(image_yaxis_theta_));
  
  const vector3_t yaxis = (yaxis_proposed - (yaxis_proposed.dot(yaxis_proposed))*zaxis).unit();
  const vector3_t xaxis = yaxis.cross(zaxis);

  xaxis_ = xaxis;
  yaxis_ = yaxis;
  zaxis_ = zaxis;

  std::cout << "BackProjectionSky" << "\n"
            << "x-axis: (" << xaxis.x() << ", " << xaxis.y() << ", " << xaxis.z() << ")\n"
            << "y-axis: (" << yaxis.x() << ", " << yaxis.y() << ", " << yaxis.z() << ")\n"
            << "z-axis: (" << zaxis.x() << ", " << zaxis.y() << ", " << zaxis.z() << ")" << std::endl;

  return AS_OK;
}

ANLStatus BackProjectionSky::mod_analyze()
{
  static std::mt19937 randgen;
  std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);

  const std::vector<BasicComptonEvent_sptr> events = getEventReconstructionModule()->getReconstructedEvents();
  for (const auto& event: events) {
    const double fraction = event->ReconstructionFraction();
    const int num_points = 1000;
    const double weight = fraction/num_points;
  
    const vector3_t cone_vertex = event->ConeVertex();
    const vector3_t cone_axis = event->ConeAxis();
    const vector3_t cone_axis_ortho = cone_axis.orthogonal();
    const double costhetaE = event->CosThetaE();

    vector3_t cone1(cone_axis);
    const double thetaE = TMath::ACos(costhetaE);
    cone1.rotate(thetaE, cone_axis_ortho);

    for (int t=0; t<num_points; t++) {
      vector3_t cone_sample = cone1;
      const double phi = constant::twopi * uniform_dist(randgen);
      cone_sample.rotate(phi, cone_axis);
      const double ux = cone_sample.dot(xaxis_);
      const double uy = cone_sample.dot(yaxis_);
      const double uz = cone_sample.dot(zaxis_);
      const double x = std::atan2(ux, uz);
      const double y = 0.5*CLHEP::pi - std::acos(uy);
      fillImage(x/PixelUnit(), y/PixelUnit(), weight);
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
