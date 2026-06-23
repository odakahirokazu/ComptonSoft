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

#include "PointSourcePrimaryGenerator.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"
#include "VANLGeometry.hh"
#include <G4ThreeVector.hh>

using namespace anlnext;

namespace anlgeant4
{

PointSourcePrimaryGenerator::PointSourcePrimaryGenerator()
  : source_position_(0.0, 0.0, 0.0),
    center_direction_(0.0, 0.0, -1.0),
    theta0_(0.0),
    theta1_(constant::pi),
    costheta0_(1.0),
    costheta1_(-1.0),
    covering_factor_(1.0),
    luminosity_(1.0*(unit::erg/unit::s))
{
  add_alias("PointSourcePrimaryGenerator");
}

PointSourcePrimaryGenerator::~PointSourcePrimaryGenerator() = default;

ANLStatus PointSourcePrimaryGenerator::mod_define()
{
  BasicPrimaryGenerator::mod_define();

  enable_powerlaw_input();

  define_parameter("position", &mod_class::source_position_, unit::cm, "cm");
  set_parameter_description("Position of the source.");
  define_parameter("direction", &mod_class::center_direction_);
  set_parameter_description("Center direction of the primaries.");
  define_parameter("theta_min", &mod_class::theta0_, 1.0, "radian");
  set_parameter_description("Minimum angle between the primary direction and the center direction.");
  define_parameter("theta_max", &mod_class::theta1_, 1.0, "radian");
  set_parameter_description("Maximum angle between the primary direction and the center direction.");
  define_parameter("luminosity", &mod_class::luminosity_, unit::erg/unit::s, "erg/s");
  set_parameter_description("Energy luminosity of the point source. This parameter is used only for calculating real time correspoing to a simulation.");

  return AS_OK;
}

ANLStatus PointSourcePrimaryGenerator::mod_initialize()
{
  using std::cos;

  BasicPrimaryGenerator::mod_initialize();

  center_direction_ = center_direction_.unit();
  costheta0_ = cos(theta0_);
  costheta1_ = cos(theta1_);
  covering_factor_ = 0.5*(costheta0_-costheta1_);

  std::cout
    << "--------\n"
    << "PrimaryGenerator status (point source)\n"
    << "  Source Position: "
    << source_position_.x()/unit::cm << " "
    << source_position_.y()/unit::cm << " "
    << source_position_.z()/unit::cm << " cm\n"
    << "  Direction: "
    << center_direction_.x() << " "
    << center_direction_.y() << " "
    << center_direction_.z() << '\n'
    << "  theta: " << theta0_/unit::degree << " - " << theta1_/unit::degree << " degree "
    << "( covering factor: " << covering_factor_ << " )\n";
  print_spectral_info();
  std::cout << std::endl;

  return AS_OK;
}

PrimarySetting PointSourcePrimaryGenerator::make_primary_setting() const
{
  const G4ThreeVector position = sample_position();
  const G4ThreeVector direction = sample_direction();
  const double energy = sample_energy();
  const G4ThreeVector polarization = unpolarized_vector(direction);
  return make_primary(position, energy, direction, polarization);
}

G4ThreeVector PointSourcePrimaryGenerator::sample_direction() const
{
  G4ThreeVector direction = sample_direction_from_cosine_range();
  direction.rotateUz(center_direction_);
  return direction;
}

G4ThreeVector PointSourcePrimaryGenerator::sample_direction_from_cosine_range() const
{
  return sample_direction_from_cosine_range(costheta0_, costheta1_);
}

G4ThreeVector PointSourcePrimaryGenerator::sample_direction_from_cosine_range(double costheta0, double costheta1) const
{
  using std::cos;
  using std::sin;
  using std::sqrt;

  const double phi = constant::twopi * G4UniformRand();
  const double costheta = costheta0 + (costheta1 - costheta0) * G4UniformRand();
  const double sintheta = sqrt(1.0 - costheta * costheta);
  G4ThreeVector direction(sintheta*cos(phi), sintheta*sin(phi), costheta);
  return direction;
}

G4ThreeVector PointSourcePrimaryGenerator::sample_position() const
{
  return source_position_;
}

ANLStatus PointSourcePrimaryGenerator::mod_end_run()
{
  double real_time = 0.0;
  if (covering_factor_ != 0.0) {
    real_time = (total_energy()/covering_factor_)/luminosity_;
  }
  set_real_time(real_time);

  std::cout.setf(std::ios::scientific);
  std::cout << "PointSourcePrimaryGenerator::mod_end_run \n"
            << "  Number: " << number() << "\n"
            << "  Luminosity: " << luminosity_/(unit::erg/unit::s) << " erg/s\n"
            << "  Total energy: " << total_energy()/unit::keV << " keV = "
            << total_energy()/unit::erg << " erg\n"
            << "  Covering factor: " << covering_factor_ << "\n"
            << "  Real Time: " << real_time/unit::s << " s\n"
            << std::flush;
  std::cout.unsetf(std::ios::scientific);

  return AS_OK;
}

} /* namespace anlgeant4 */
