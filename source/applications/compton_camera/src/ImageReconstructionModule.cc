#include "ImageReconstructionModule.hh"
#include "AstroUnits.hh"
#include "EventReconstruction.hh"
#include "LMMLEM.hh"
#include "healpix_cxx/fitshandle.h"
#include "healpix_cxx/healpix_map.h"
#include "healpix_cxx/healpix_map_fitsio.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

ImageReconstructionModule::ImageReconstructionModule()
  : nside_(32),
    image_center_theta_(0.0 * unit::degree),
    image_center_phi_(0.0 * unit::degree),
    image_yaxis_theta_(90.0 * unit::degree),
    image_yaxis_phi_(90.0 * unit::degree),
    input_image_(""),
    sensitivity_map_(""),
    output_image_("image.fits"),
    iteration_(0),
    event_reconstruction_(nullptr),
    algorithm_(nullptr)
{
  define_submodule("algorithm");
  ImageReconstructionModule::set_submodule_by_key("algorithm", "LMMLEM");
}

ImageReconstructionModule::~ImageReconstructionModule() = default;

void ImageReconstructionModule::set_submodule_by_key(const std::string& name, const std::string& key)
{
  if (name != "algorithm") {
    std::cerr << module_id() << ": unknown submodule name: " << name << std::endl;
    BOOST_THROW_EXCEPTION(SubModuleUndefinedError(this, name));
  }

  if (key == "LMMLEM") {
    auto algorithm = std::make_unique<LMMLEM>();
    algorithm_ = algorithm.get();
    set_submodule(name, std::move(algorithm));
  }
  else {
    std::cerr << module_id() << ": unknown algorithm: " << key << std::endl;
    BOOST_THROW_EXCEPTION(SubModuleUndefinedError(this, name + ":" + key));
  }
}

ANLStatus ImageReconstructionModule::mod_define()
{
  define_parameter("nside", &mod_class::nside_);
  define_parameter("image_center_theta", &mod_class::image_center_theta_, unit::degree, "degree");
  define_parameter("image_center_phi", &mod_class::image_center_phi_, unit::degree, "degree");
  define_parameter("image_yaxis_theta", &mod_class::image_yaxis_theta_, unit::degree, "degree");
  define_parameter("image_yaxis_phi", &mod_class::image_yaxis_phi_, unit::degree, "degree");
  define_parameter("input_image", &mod_class::input_image_);
  set_parameter_description("FITS file of the prior image. If empty, a uniform image is used.");
  define_parameter("sensitivity_map", &mod_class::sensitivity_map_);
  set_parameter_description("FITS file of the sensitivity map. If empty, a uniform map is used.");
  define_parameter("output_image", &mod_class::output_image_);
  define_parameter("iteration", &mod_class::iteration_);
  set_parameter_description("Iteration number (recorded in the output).");
  return AS_OK;
}

ANLStatus ImageReconstructionModule::mod_initialize()
{
  using std::cos, std::sin;

  get_module_IF("EventReconstruction", &event_reconstruction_);

  if (algorithm_ == nullptr) {
    std::cerr << module_id() << ": algorithm submodule is not set." << std::endl;
    return AS_QUIT_ERROR;
  }

  // sky frame (same convention as BackProjectionHealPix)
  const vector3_t zaxis(sin(image_center_theta_) * cos(image_center_phi_),
                        sin(image_center_theta_) * sin(image_center_phi_),
                        cos(image_center_theta_));
  const vector3_t yaxis_proposed(sin(image_yaxis_theta_) * cos(image_yaxis_phi_),
                                 sin(image_yaxis_theta_) * sin(image_yaxis_phi_),
                                 cos(image_yaxis_theta_));
  const vector3_t yaxis = (yaxis_proposed - (yaxis_proposed.dot(zaxis)) * zaxis).unit();
  const vector3_t xaxis = yaxis.cross(zaxis);

  space_ = std::make_unique<HealPixImageSpace>(nside_, xaxis, yaxis, zaxis);
  const int num_pixels = space_->num_pixels();

  // prior image
  std::vector<double> image;
  if (input_image_.empty()) {
    image.assign(num_pixels, 1.0);
  }
  else if (!readMap(input_image_, image)) {
    return AS_QUIT_ERROR;
  }

  // sensitivity map
  std::vector<double> sensitivity;
  if (sensitivity_map_.empty()) {
    sensitivity.assign(num_pixels, 1.0);
  }
  else if (!readMap(sensitivity_map_, sensitivity)) {
    return AS_QUIT_ERROR;
  }

  algorithm_->initialize(*space_, image, sensitivity);

  std::cout << module_id() << "\n"
            << "  algorithm: " << algorithm_->submodule_name() << "\n"
            << "  iteration: " << iteration_ << "\n"
            << "  nside: " << nside_ << " (" << num_pixels << " pixels)\n"
            << "  input image: " << (input_image_.empty() ? "(uniform)" : input_image_) << "\n"
            << "  sensitivity: " << (sensitivity_map_.empty() ? "(uniform)" : sensitivity_map_) << "\n"
            << "  x-axis: (" << xaxis.x() << ", " << xaxis.y() << ", " << xaxis.z() << ")\n"
            << "  y-axis: (" << yaxis.x() << ", " << yaxis.y() << ", " << yaxis.z() << ")\n"
            << "  z-axis: (" << zaxis.x() << ", " << zaxis.y() << ", " << zaxis.z() << ")" << std::endl;

  return AS_OK;
}

ANLStatus ImageReconstructionModule::mod_analyze()
{
  const std::vector<BasicComptonEvent_sptr>& events = event_reconstruction_->getReconstructedEvents();
  if (events.empty()) {
    return AS_OK;
  }
  algorithm_->calculate(events);
  return AS_OK;
}

ANLStatus ImageReconstructionModule::mod_end_run()
{
  std::vector<double> updated_image;
  algorithm_->update(updated_image);

  std::cout << module_id() << ": iteration " << iteration_
            << "  events = " << algorithm_->num_events()
            << "  skipped = " << algorithm_->num_skipped_events()
            << "  log likelihood = " << std::setprecision(12) << algorithm_->log_likelihood()
            << std::endl;

  writeImage(updated_image);
  return AS_OK;
}

bool ImageReconstructionModule::readMap(const std::string& filename, std::vector<double>& values) const
{
  Healpix_Map<double> map;
  read_Healpix_map_from_fits(filename, map);
  if (map.Nside() != nside_) {
    std::cerr << module_id() << ": nside of " << filename << " (" << map.Nside()
              << ") does not match the parameter nside (" << nside_ << ")." << std::endl;
    return false;
  }
  if (map.Scheme() != RING) {
    map.swap_scheme();
  }

  const int num_pixels = map.Npix();
  values.assign(num_pixels, 0.0);
  for (int j = 0; j < num_pixels; j++) {
    values[j] = map[j];
  }
  return true;
}

void ImageReconstructionModule::writeImage(const std::vector<double>& values) const
{
  Healpix_Map<double> map(nside_, RING, SET_NSIDE);
  const int num_pixels = map.Npix();
  for (int j = 0; j < num_pixels; j++) {
    map[j] = values[j];
  }

  fitshandle handle;
  handle.create("!" + output_image_);
  write_Healpix_map_to_fits(handle, map, PLANCK_FLOAT64);
  handle.set_key("ITER", iteration_, "iteration number");
  handle.set_key("NEVENTS", static_cast<int>(algorithm_->num_events()), "number of events used");
  handle.set_key("NSKIPPED", static_cast<int>(algorithm_->num_skipped_events()), "number of events skipped");
  handle.set_key("LOGL", algorithm_->log_likelihood(), "log likelihood of the input image");
  handle.set_key("ALGORITH", algorithm_->submodule_name(), "reconstruction algorithm");
  handle.set_key("INIMAGE", (input_image_.empty() ? std::string("uniform") : input_image_), "input image");
  handle.set_key("SENSMAP", (sensitivity_map_.empty() ? std::string("uniform") : sensitivity_map_), "sensitivity map");
  handle.close();
}

} // namespace comptonsoft
