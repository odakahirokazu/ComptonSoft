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

#include "IsotropicPrimaryGenerator.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

IsotropicPrimaryGenerator::IsotropicPrimaryGenerator()
  : center_position_(0.0, 0.0, 0.0),
    radius_(100.0*unit::cm), distance_(-1.0),
    center_direction_(0.0, 0.0, 1.0),
    theta_min_(0.0), theta_max_(CLHEP::pi),
    costheta0_(1.0), costheta1_(-1.0), covering_factor_(1.0),
    intensity_(1.0e-9*(unit::erg/unit::s/unit::cm2/unit::sr))
{
  add_alias("IsotropicPrimaryGenerator");
}

IsotropicPrimaryGenerator::~IsotropicPrimaryGenerator() = default;

ANLStatus IsotropicPrimaryGenerator::mod_define()
{
  BasicPrimaryGenerator::mod_define();

  enable_powerlaw_input();

  define_parameter("center_position", &mod_class::center_position_, unit::cm, "cm");
  set_parameter_description("Position of the sphere.");
  define_parameter("radius", &mod_class::radius_, unit::cm, "cm");
  set_parameter_description("Radius of a disk where primaries are generated.");
  define_parameter("distance", &mod_class::distance_, unit::cm, "cm");
  set_parameter_description("Distance between the sphere center and a disk where primaries are generated. If this value is negative, then this parameter is set to be the same as the disk radius [Radius].");
  define_parameter("center_direction", &mod_class::center_direction_);
  set_parameter_description("Center direction of the primaries.");
  define_parameter("theta_min", &mod_class::theta_min_, 1.0, "radian");
  set_parameter_description("Minimum angle between the primary direction and the center direction.");
  define_parameter("theta_max", &mod_class::theta_max_, 1.0, "radian");
  set_parameter_description("Maximum angle between the primary direction and the center direction.");
  define_parameter("intensity", &mod_class::intensity_, (unit::erg/unit::cm2/unit::s/unit::sr), "erg/cm2/s/sr");
  set_parameter_description("Energy intensity of the primaries. This parameter is used only for calculating real time correspoing to a simulation.");

  return AS_OK;
}

ANLStatus IsotropicPrimaryGenerator::mod_initialize()
{
  using std::cos;
  using std::acos;
  using std::sqrt;

  BasicPrimaryGenerator::mod_initialize();

  center_direction_ = center_direction_.unit();
  if (distance_ < 0.0) { distance_ = radius_; }

  costheta0_ = cos(theta_min_);
  costheta1_ = cos(theta_max_);
  covering_factor_ = 0.5*(costheta0_-costheta1_);

  std::cout
   << "--------\n"
   << "PrimaryGenerator status (isotropic)\n"
   << "  Center position: "
   << center_position_.x()/unit::cm << " "
   << center_position_.y()/unit::cm << " "
   << center_position_.z()/unit::cm << " cm\n"
   << "  Radius: " << radius_/unit::cm << " cm\n"
   << "  Distance: " << distance_/unit::cm << " cm\n"
   << "  Center direction: "
   << center_direction_.x() << " "
   << center_direction_.y() << " "
   << center_direction_.z() << '\n'
   << "  Theta: " << theta_min_/unit::degree << " - " << theta_max_/unit::degree << " deg\n";
  print_spectral_info();
  std::cout << std::endl;

  return AS_OK;
}

PrimarySetting IsotropicPrimaryGenerator::make_primary_setting() const
{
  using std::cos;
  using std::sin;
  using std::sqrt;

  // set position and direction
  const double costheta = costheta0_ + (costheta1_-costheta0_) * G4UniformRand();
  const double sintheta = sqrt(1.0-costheta*costheta);
  const double phi = CLHEP::twopi * G4UniformRand();
  G4ThreeVector v(distance_*sintheta*cos(phi),
                  distance_*sintheta*sin(phi),
                  distance_*costheta);
  v.rotateUz(-center_direction_);

  G4ThreeVector v2 = v.orthogonal();
  v2.setMag( radius_ * sqrt(G4UniformRand()) );
  const G4double chi = constant::twopi * G4UniformRand();
  v2.rotate(chi, v);

  const G4ThreeVector position = center_position_ + v + v2;
  const G4ThreeVector direction = (-v).unit();
  const double energy = sample_energy();
  const G4ThreeVector polarization = unpolarized_vector(direction);

  return make_primary(position, energy, direction, polarization);
}

ANLStatus IsotropicPrimaryGenerator::mod_end_run()
{
  const double area = constant::pi*radius_*radius_;
  const double solid_angle = 4*constant::pi*covering_factor_*unit::sr;

  double real_time = 0.0;
  double particle_intensity = 0.0;
  if (covering_factor_ != 0.0) {
    real_time = total_energy()/(intensity_*area*solid_angle);
    particle_intensity = number()/area/real_time/solid_angle;
  }

  set_real_time(real_time);

  std::cout.setf(std::ios::scientific);
  std::cout << "IsotropicPrimaryGenerator::mod_end_run \n"
            << "  Number: " << number() << "\n"
            << "  Intensity: " << intensity_/(unit::erg/unit::cm2/unit::s/unit::sr) << " erg/cm2/s/sr\n"
            << "  Total energy: " << total_energy()/unit::keV << " keV = "
            << total_energy()/unit::erg << " erg\n"
            << "  Covering factor: " << covering_factor_ << "\n"
            << "  Area: " << area/unit::cm2 << " cm2\n"
            << "  Real time: " << real_time/unit::s << " s\n"
            << "  Particle intensity: " << particle_intensity/(1.0/unit::cm2/unit::s/unit::sr) << " particles/cm2/s/sr\n"
            << std::endl;
  std::cout.unsetf(std::ios::scientific);

  return AS_OK;
}

} /* namespace anlgeant4 */
