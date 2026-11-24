#include "LMMLEM.hh"
#include "AstroUnits.hh"
#include "healpix_cxx/pointing.h"
#include "healpix_cxx/rangeset.h"
#include <algorithm>
#include <cmath>

namespace unit = anlgeant4::unit;
namespace constant = anlgeant4::constant;

namespace comptonsoft {

LMMLEM::LMMLEM()
  : arm_sigma_(3.0 * unit::degree),
    cutoff_sigma_(3.0),
    space_(nullptr),
    likelihood_(0.0),
    num_events_(0),
    num_skipped_events_(0)
{
}

LMMLEM::~LMMLEM() = default;

void LMMLEM::define_parameters()
{
  define_parameter("arm", &mod_class::arm_sigma_, unit::degree, "degree");
  set_parameter_description("Gaussian sigma of the ARM kernel.");
  define_parameter("cutoff_sigma", &mod_class::cutoff_sigma_);
  set_parameter_description("Pixels with |ARM| > cutoff_sigma * arm are ignored.");
}

void LMMLEM::initialize(const HealPixImageSpace& space,
                        const std::vector<double>& image,
                        const std::vector<double>& sensitivity)
{
  space_ = &space;
  image_ = image;
  sensitivity_ = sensitivity;
  backprojection_.assign(space.num_pixels(), 0.0);
  likelihood_ = 0.0;
  num_events_ = 0;
  num_skipped_events_ = 0;
}

bool LMMLEM::fillResponse(const HealPixImageSpace& space,
                          const BasicComptonEvent& event,
                          double weight,
                          std::vector<std::pair<int, double>>& response) const
{
  const double theta_e = event.ThetaE();
  if (!std::isfinite(theta_e)) {
    return false;
  }

  const vec3 axis = space.to_healpix_vector(event.ConeAxis().unit());
  const double cutoff = cutoff_sigma_ * arm_sigma_;
  const double radius = std::min(theta_e + cutoff, constant::pi);

  rangeset<int> pixels;
  space.base().query_disc_inclusive(pointing(axis), radius, pixels);

  const std::size_t start = response.size();
  double sum = 0.0;
  for (tsize r = 0; r < pixels.nranges(); r++) {
    for (int j = pixels.ivbegin(r); j < pixels.ivend(r); j++) {
      const vec3 v = space.base().pix2vec(j);
      const double cos_angle = std::clamp(dotprod(axis, v), -1.0, 1.0);
      const double arm = std::acos(cos_angle) - theta_e;
      if (std::abs(arm) > cutoff) { continue; }
      const double g = std::exp(-0.5 * (arm * arm) / (arm_sigma_ * arm_sigma_));
      response.emplace_back(j, g);
      sum += g;
    }
  }

  if (sum <= 0.0) {
    response.resize(start);
    return false;
  }

  const double factor = weight / sum;
  for (std::size_t k = start; k < response.size(); k++) {
    response[k].second *= factor;
  }
  return true;
}

void LMMLEM::calculate(const std::vector<BasicComptonEvent_sptr>& events)
{
  std::vector<std::pair<int, double>> response;
  for (const auto& event: events) {
    fillResponse(*space_, *event, event->ReconstructionFraction(), response);
  }

  double expectation = 0.0;
  for (const auto& [j, t]: response) {
    expectation += t * image_[j];
  }

  if (!(expectation > 0.0)) {
    ++num_skipped_events_;
    return;
  }

  for (const auto& [j, t]: response) {
    backprojection_[j] += t / expectation;
  }
  likelihood_ += std::log(expectation);
  ++num_events_;
}

void LMMLEM::update(std::vector<double>& updated_image)
{
  const int num_pixels = space_->num_pixels();
  updated_image.assign(num_pixels, 0.0);

  double expected_total = 0.0;
  for (int j = 0; j < num_pixels; j++) {
    const double s = sensitivity_[j];
    expected_total += s * image_[j];
    if (s > 0.0) {
      updated_image[j] = image_[j] * backprojection_[j] / s;
    }
  }
  likelihood_ -= expected_total;
}

} // namespace comptonsoft
