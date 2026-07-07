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

#include "WeightingPotentialPixel.hh"
#include <iostream>
#include <cmath>
#include <boost/math/constants/constants.hpp>

namespace comptonsoft {

WeightingPotentialPixel::WeightingPotentialPixel()
  : sizeX_(1.0), sizeY_(1.0), pitchX_(1.0), pitchY_(1.0), thickness_(0.0),
    gamma_(boost::extents[NumGrids_][NumGrids_]),
    a0_(boost::extents[NumGrids_][NumGrids_])
{
}

WeightingPotentialPixel::~WeightingPotentialPixel() = default;

void WeightingPotentialPixel::initializeTable()
{
  constexpr double pi = boost::math::constants::pi<double>();
  const double a = SizeX();
  const double U = PitchX();
  const double b = SizeY();
  const double V = PitchY();
  const double L = Thickness();
  
  for (int m=1; m<=NumGrids_; m++) {
    alpha_[m-1] = pi*m/a;
  }

  for (int n=1; n<=NumGrids_; n++) {
    beta_[n-1] = pi*n/b;
  }
  
  for (int m=1; m<=NumGrids_; m++) {
    const double fm = std::cos(pi*m*(a-U)/(2.0*a)) - std::cos(pi*m*(a+U)/(2.0*a));
    for (int n=1; n<=NumGrids_; n++) {
      const double fn = std::cos(pi*n*(b-V)/(2.0*b)) - std::cos(pi*n*(b+V)/(2.0*b));
      gamma_[m-1][n-1] = std::sqrt(alpha_[m-1]*alpha_[m-1]+beta_[n-1]*beta_[n-1]);
      a0_[m-1][n-1] = 4.0/(pi*pi*m*n*std::sinh(gamma_[m-1][n-1]*L))*fm*fn;
    }
  }
}

void WeightingPotentialPixel::printTable()
{
  for (int m=0; m<NumGrids_; m++) {
    for (int n=0; n<NumGrids_; n++) {
      printf("%4d %4d   %+10.4e  %+10.4e  %+10.4e  %+10.4e\n", m, n, alpha_[m], beta_[n], gamma_[m][n], a0_[m][n]);
    }
  }
}

void WeightingPotentialPixel::setXY(double x0, double y0)
{
  const double x = 0.5*SizeX() + x0;
  const double y = 0.5*SizeY() + y0;

  for (int m=1; m<=NumGrids_; m++) {
    sinAX_[m-1] = std::sin(alpha_[m-1]*x);
  }

  for (int n=1; n<=NumGrids_; n++) {
    sinBY_[n-1] = std::sin(beta_[n-1]*y);
  }
}

double WeightingPotentialPixel::calculateWeightingPotential(double x0, double y0, double z0)
{
  setXY(x0, y0);
  return calculateWeightingPotential(z0);
}

double WeightingPotentialPixel::calculateWeightingPotential(double z0)
{
  const double z = 0.5*Thickness() + z0;
  double phi = 0.0;
  for (int m=NumGrids_; m>=1; m--) {
    for (int n=NumGrids_; n>=1; n--) {
      if (a0_[m-1][n-1]==0.0) continue;
      const double sinhg = std::sinh(gamma_[m-1][n-1]*z);
      phi += sinhg * a0_[m-1][n-1] * sinAX_[m-1] * sinBY_[n-1];
    }
  }
  return phi;
}

} /* namespace comptonsoft */
