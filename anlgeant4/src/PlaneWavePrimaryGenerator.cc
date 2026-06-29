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

#include "PlaneWavePrimaryGenerator.hh"
#include "Randomize.hh"
#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

PlaneWavePrimaryGenerator::PlaneWavePrimaryGenerator()
  : center_position_(0.0, 0.0, 0.0),
    direction_(0.0, 0.0, -1.0),
    direction_orthogonal_(0.0, 1.0, 0.0),
    radius_(100.0*unit::cm),
    polarization_(1.0, 0.0, 0.0),
    polarization_degree_(0.0),
    flux_(1.0e-9*(unit::erg/unit::s/unit::cm2))
{
  add_alias("PlaneWavePrimaryGenerator");
}

PlaneWavePrimaryGenerator::~PlaneWavePrimaryGenerator() = default;

ANLStatus PlaneWavePrimaryGenerator::mod_define()
{
  BasicPrimaryGenerator::mod_define();

  enable_powerlaw_input();
  define_parameter("position", &mod_class::center_position_, unit::cm, "cm");
  set_parameter_description("Center of the circle where primary particles are generated.");
  define_parameter("direction", &mod_class::direction_);
  set_parameter_description("Propagation direction of the plane wave.");
  define_parameter("radius", &mod_class::radius_, unit::cm, "cm");
  set_parameter_description("Radius of the circle where parimary particles are generated.");
  define_parameter("polarization_vector", &mod_class::polarization_);
  set_parameter_description("Polarization vector");
  define_parameter("degree_of_polarization", &mod_class::polarization_degree_);
  set_parameter_description("Degree of polarization");
  define_parameter("flux", &mod_class::flux_, unit::erg/unit::s/unit::cm2, "erg/s/cm2");
  set_parameter_description("Energy flux of the plane wave. This parameter is used only for calculating real time correspoing to a simulation.");


  return AS_OK;
}

ANLStatus PlaneWavePrimaryGenerator::mod_initialize()
{
  BasicPrimaryGenerator::mod_initialize();

  direction_ = direction_.unit();
  direction_orthogonal_ = direction_.orthogonal().unit();

  std::cout << "--------" << std::endl;
  std::cout << "PrimaryGenerator status (plane wave)" << std::endl;
  std::cout << "  Center position: "
            << center_position_.x()/unit::cm << " "
            << center_position_.y()/unit::cm << " "
            << center_position_.z()/unit::cm << " cm\n"
            << "  Direction: "
            << direction_.x() << " "
            << direction_.y() << " "
            << direction_.z() << '\n';
  print_spectral_info();
  std::cout << std::endl;

  return AS_OK;
}

PrimarySetting PlaneWavePrimaryGenerator::make_primary_setting() const
{
  const G4ThreeVector position = sample_position();
  const double energy = sample_energy();
  const G4ThreeVector direction = direction_;

  G4ThreeVector polarization;
  if (G4UniformRand() < polarization_degree_) {
    polarization = polarization_;
  }
  else {
    polarization = unpolarized_vector(direction);
  }
  return make_primary(position, energy, direction, polarization);
}

G4ThreeVector PlaneWavePrimaryGenerator::sample_position() const
{
  using std::sqrt;

  G4ThreeVector position(direction_orthogonal_);
  const double r = radius_ * sqrt(G4UniformRand());
  const double t = constant::twopi * G4UniformRand();
  position.rotate(t, direction_);
  position = center_position_ + r * position;
  return position;
}

ANLStatus PlaneWavePrimaryGenerator::mod_end_run()
{
  const double area = generation_area();
  const double real_time = total_energy()/(flux_*area);
  const double photon_flux = number()/area/real_time;

  set_real_time(real_time);

  std::cout.setf(std::ios::scientific);
  std::cout << "PlaneWavePrimaryGenerator::mod_end_run \n"
            << "  Number: " << number() << "\n"
            << "  Flux: " << flux_/(unit::erg/unit::cm2/unit::s) << " erg/cm2/s\n"
            << "  Total energy: " << total_energy()/unit::keV << " keV = "
            << total_energy()/unit::erg << " erg\n"
            << "  Area: " << area/unit::cm2 << " cm2\n"
            << "  Real time: " << real_time/unit::s << " s\n"
            << "  Photon flux: " << photon_flux/(1.0/unit::cm2/unit::s) << " photons/cm2/s\n"
            << std::endl;
  std::cout.unsetf(std::ios::scientific);

  return AS_OK;
}

double PlaneWavePrimaryGenerator::generation_area() const
{
  return constant::pi*radius_*radius_;
}

} /* namespace anlgeant4 */
