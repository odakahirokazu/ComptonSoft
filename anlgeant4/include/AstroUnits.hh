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

#define INCLUDE_G4GLOBALS 1
#if INCLUDE_G4GLOBALS
#include "globals.hh"
#else
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#endif
#undef INCLUDE_G4GLOBALS

namespace anlgeant4 {

static const double erg = 1.E-07 * joule;

static const double ksec = 1.E+03 * second;
static const double ks = ksec;
static const double hour = 3600.0 * second;
static const double day = 24.0 * hour;
static const double year = 365.242199 * day;

static const double solar_mass = 1.98892E+30 * kg;
static const double astronomical_unit = 149597870.700 * km;
static const double AU = astronomical_unit;
static const double solar_radius = 6.955E+08 * m;

}

#endif /* ANLGEANT4_AstroUnits_H */
