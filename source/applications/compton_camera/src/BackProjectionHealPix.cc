#include "BackProjectionHealPix.hh"
#include "AstroUnits.hh"
#include "BasicComptonEvent.hh"
#include "EventReconstruction.hh"
#include "SaveFits.hh"
#include "healpix_cxx/healpix_map.h"
#include "healpix_cxx/healpix_map_fitsio.h"
#include <cmath>
#include <random>

using namespace anlnext;
namespace unit = anlgeant4::unit;
namespace constant = anlgeant4::constant;

namespace comptonsoft {
BackProjectionHealPix::BackProjectionHealPix()
  : map_(nullptr),
    nside_(3),
    num_points_(1000),
    arm_spread_(0.0),
    image_center_theta_(0.0 * unit::degree),
    image_center_phi_(90.0 * unit::degree),
    image_yaxis_theta_(0.0 * unit::degree),
    image_yaxis_phi_(90.0 * unit::degree),
    event_reconstruction_module_(nullptr),
    save_fits_module_(nullptr) {};

BackProjectionHealPix::~BackProjectionHealPix() = default;

ANLStatus BackProjectionHealPix::mod_define()
{
  define_parameter("nside", &mod_class::nside_);

  define_parameter("image_center_theta", &mod_class::image_center_theta_, unit::degree, "degree");
  define_parameter("image_center_phi", &mod_class::image_center_phi_, unit::degree, "degree");
  define_parameter("image_yaxis_theta", &mod_class::image_yaxis_theta_, unit::degree, "degree");
  define_parameter("image_yaxis_phi", &mod_class::image_yaxis_phi_, unit::degree, "degree");

  define_parameter("arm", &mod_class::arm_spread_, unit::degree, "degree");

  define_parameter("fits_filename", &mod_class::fits_filename_);
  return AS_OK;
}

ANLStatus BackProjectionHealPix::mod_initialize()
{
  using std::cos, std::sin;

  get_module_IF("EventReconstruction", &event_reconstruction_module_);
  get_module_IFNC("SaveFits", &save_fits_module_);

  if (!event_reconstruction_module_) {
    std::cerr << "EventReconstruction module not found" << std::endl;
    return AS_QUIT_ERROR;
  }
  if (!save_fits_module_) {
    std::cerr << "SaveFits module not found" << std::endl;
    return AS_QUIT_ERROR;
  }

  map_ = std::make_unique<Healpix_Map<double>>();
  map_->SetNside(nside_, Healpix_Ordering_Scheme::RING);

  const vector3_t zaxis(sin(image_center_theta_) * cos(image_center_phi_),
                        sin(image_center_theta_) * sin(image_center_phi_), cos(image_center_theta_));
  const vector3_t yaxis_proposed(sin(image_yaxis_theta_) * cos(image_yaxis_phi_),
                                 sin(image_yaxis_theta_) * sin(image_yaxis_phi_), cos(image_yaxis_theta_));

  const vector3_t yaxis = (yaxis_proposed - (yaxis_proposed.dot(zaxis)) * zaxis).unit();
  const vector3_t xaxis = yaxis.cross(zaxis);

  xaxis_ = xaxis;
  yaxis_ = yaxis;
  zaxis_ = zaxis;

  std::cout << "BackProjectionHealPix" << "\n"
            << "x-axis: (" << xaxis.x() << ", " << xaxis.y() << ", " << xaxis.z() << ")\n"
            << "y-axis: (" << yaxis.x() << ", " << yaxis.y() << ", " << yaxis.z() << ")\n"
            << "z-axis: (" << zaxis.x() << ", " << zaxis.y() << ", " << zaxis.z() << ")" << std::endl;
  return AS_OK;
}

ANLStatus BackProjectionHealPix::mod_begin_run()
{
  map_->fill(0);
  return AS_OK;
}

ANLStatus BackProjectionHealPix::mod_analyze()
{
  static std::mt19937 randgen;
  std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
  std::normal_distribution<double> normal_dist(0.0, ARMSpread());

  const std::vector<BasicComptonEvent_sptr> events = getEventReconstructionModule()->getReconstructedEvents();
  for (const auto& event : events) {
    const double fraction = event->ReconstructionFraction();
    const size_t num_points = NumPoints();
    const double weight = fraction / num_points;

    const vector3_t cone_vertex = event->ConeVertex();
    const vector3_t cone_axis = event->ConeAxis();
    const vector3_t cone_axis_ortho = cone_axis.orthogonal();
    const double costhetaE = event->CosThetaE();
    const double thetaE0 = std::acos(costhetaE);

    for (size_t i = 0; i < num_points; i++) {
      vector3_t cone0(cone_axis);
      const double delta_thetaE = normal_dist(randgen);
      const double thetaE = thetaE0 + delta_thetaE;
      cone0.rotate(thetaE, cone_axis_ortho);

      vector3_t cone1 = cone0;
      const double phi = constant::twopi * uniform_dist(randgen);
      cone1.rotate(phi, cone_axis);

      const double ux = cone1.dot(xaxis_);
      const double uy = cone1.dot(yaxis_);
      const double uz = cone1.dot(zaxis_);
      const double x = std::atan2(ux, uz);
      const double y = std::acos(uy);
      const pointing p(y / unit::radian, x / unit::radian);
      const int pix = map_->ang2pix(p);
      (*map_)[pix] += weight;
    }
  }

  return AS_OK;
}

ANLStatus BackProjectionHealPix::mod_end_run()
{
  save_fits_module_->writeMap(*map_, PLANCK_FLOAT64);
  return AS_OK;
}
} // namespace comptonsoft