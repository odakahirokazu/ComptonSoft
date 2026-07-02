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

// Hiro Odaka
// reference : Zumbiehl et al. NIM A 469 227-239 (2001)
// reference : Odaka et al. NIM A submitted (2009)

#include "WeightingPotentialStrip.hh"
#include <iostream>
#include <cmath>
#include <boost/math/constants/constants.hpp>

namespace comptonsoft {

WeightingPotentialStrip::WeightingPotentialStrip()
  : sizeX_(1.0), pitchX_(1.0), thickness_(1.0)
{
}

WeightingPotentialStrip::~WeightingPotentialStrip() = default;

void WeightingPotentialStrip::initializeTable()
{
  constexpr double pi = boost::math::constants::pi<double>();
  const double a = SizeX();
  const double U = PitchX();
  const double L = Thickness();
  
  for (int m=1; m<=NumGrids_; m++) {
    alpha_[m-1] = pi*m/a;
  }

  for (int m=1; m<=NumGrids_; m++) {
    const double fm = std::cos(pi*m*(a-U)/(2.0*a)) - std::cos(pi*m*(a+U)/(2.0*a));
    a0_[m-1] = 2.0/(pi*m*std::sinh(alpha_[m-1]*L))*fm;
  }
}

void WeightingPotentialStrip::printTable()
{
  for (int m=0; m<NumGrids_; m++) {
    printf("%4d   %+10.4e  %+10.4e\n", m, alpha_[m], a0_[m]);
  }
}

void WeightingPotentialStrip::setX(double x0)
{
  const double x = 0.5*SizeX() + x0;
  for (int m=1; m<=NumGrids_; m++) {
    sinAX_[m-1] = std::sin(alpha_[m-1]*x);
  }
}

double WeightingPotentialStrip::calculateWeightingPotential(double x0, double z0)
{
  setX(x0);
  return calculateWeightingPotential(z0);
}

double WeightingPotentialStrip::calculateWeightingPotential(double z0)
{
  const double z = 0.5*Thickness() + z0;
  double phi = 0.0;
  for (int m=NumGrids_; m>=1; m--) {
    if (a0_[m-1]==0.0) continue;
    const double sinhg = std::sinh(alpha_[m-1]*z);
    phi += sinhg * a0_[m-1] * sinAX_[m-1];
  }
  
  return phi;
}

} /* namespace comptonsoft */
