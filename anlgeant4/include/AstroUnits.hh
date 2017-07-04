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

using CLHEP::joule;
constexpr double erg = 1.E-07 * joule;
using CLHEP::MeV;
using CLHEP::keV;
using CLHEP::eV;
using CLHEP::GeV;

using CLHEP::s;
using CLHEP::second;
using CLHEP::ns;
using CLHEP::us;
using CLHEP::ms;
constexpr double ks = 1000.0 * s;
constexpr double hour = 3600.0 * s;
constexpr double day = 24.0 * hour;
constexpr double year = 365.25 * day;

constexpr double Hz = 1.0/second;

using CLHEP::g;
using CLHEP::kg;
constexpr double solar_mass = 1.98892E+30 * kg;

using CLHEP::mm;
using CLHEP::cm;
using CLHEP::m;
using CLHEP::km;
constexpr double astronomical_unit = 149597870.700 * km;
constexpr double AU = astronomical_unit;
constexpr double solar_radius = 6.955E+08 * m;
using CLHEP::um;

using CLHEP::cm2;
using CLHEP::cm3;
using CLHEP::m2;
using CLHEP::m3;

constexpr double classical_electron_radius = CLHEP::classic_electr_radius;
constexpr double sigma_thomson =
  (8.0*CLHEP::pi/3.0)*CLHEP::classic_electr_radius*CLHEP::classic_electr_radius;

using CLHEP::degree;
using CLHEP::radian;
using CLHEP::rad;
using CLHEP::steradian;
using CLHEP::sr;

using CLHEP::volt;

using CLHEP::kelvin;

} /* namespace unit */
} /* namespace anlgeant4 */

#endif /* ANLGEANT4_AstroUnits_H */
