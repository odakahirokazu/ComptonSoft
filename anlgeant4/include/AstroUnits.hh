/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Shin Watanabe, Hirokazu Odaka                      *
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

#ifndef ANLGEANT4_AstroUnits_H
#define ANLGEANT4_AstroUnits_H 1

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace anlgeant4 {

namespace unit {

using CLHEP::pi;
using CLHEP::twopi;

using CLHEP::nm;
using CLHEP::um;
using CLHEP::mm;
using CLHEP::cm;
using CLHEP::m;
using CLHEP::km;

using CLHEP::centimeter;
using CLHEP::meter;

constexpr double astronomical_unit = 149597870.700 * km;
constexpr double AU = astronomical_unit;
constexpr double solar_radius = 6.955E+08 * m;

using CLHEP::parsec;
using CLHEP::pc;

using CLHEP::mm2;
using CLHEP::mm3;
using CLHEP::cm2;
using CLHEP::cm3;
using CLHEP::m2;
using CLHEP::m3;

using CLHEP::barn;
using CLHEP::millibarn;

using CLHEP::ps;
using CLHEP::ns;
using CLHEP::us;
using CLHEP::ms;
using CLHEP::s;

using CLHEP::second;

constexpr double ks = 1000.0 * s;
constexpr double hour = 3600.0 * s;
constexpr double day = 24.0 * hour;
constexpr double year = 365.25 * day;

constexpr double Hz = 1.0/second;

using CLHEP::eV;
using CLHEP::keV;
using CLHEP::MeV;
using CLHEP::GeV;
using CLHEP::TeV;
using CLHEP::PeV;

using CLHEP::joule;
constexpr double erg = 1.0E-07 * joule;

using CLHEP::g;
using CLHEP::kg;
using CLHEP::gram;
using CLHEP::kilogram;
constexpr double solar_mass = 1.98892E+30 * kg;

using CLHEP::degree;
using CLHEP::radian;
using CLHEP::rad;
using CLHEP::steradian;
using CLHEP::sr;

using CLHEP::newton;

constexpr double pascal = newton/m2;

using CLHEP::watt;

using CLHEP::eplus;
using CLHEP::coulomb;

using CLHEP::volt;

using CLHEP::tesla;
using CLHEP::gauss;

using CLHEP::kelvin;

using CLHEP::mole;

} /* namespace unit */

namespace constant
{
using CLHEP::pi;
using CLHEP::twopi;

using CLHEP::Avogadro;

using CLHEP::c_light;
using CLHEP::c_squared;

using CLHEP::h_Planck;
using CLHEP::hbar_Planck;
using CLHEP::hbarc;
using CLHEP::hbarc_squared;

using CLHEP::eplus;

using CLHEP::electron_mass_c2;
using CLHEP::proton_mass_c2;
using CLHEP::neutron_mass_c2;
using CLHEP::amu_c2;
using CLHEP::amu;

using CLHEP::fine_structure_const;
using CLHEP::classic_electr_radius;
using CLHEP::Bohr_radius;
using CLHEP::alpha_rcl2;

constexpr double classical_electron_radius = CLHEP::classic_electr_radius;
constexpr double sigma_thomson =
  (8.0*CLHEP::pi/3.0)*CLHEP::classic_electr_radius*CLHEP::classic_electr_radius;

using CLHEP::k_Boltzmann;

} /* namespace constant */

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_AstroUnits_H */
