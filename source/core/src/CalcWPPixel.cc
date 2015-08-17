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

#include "CalcWPPixel.hh"
#include <iostream>
#include "TMath.h"

namespace comptonsoft {

CalcWPPixel::CalcWPPixel()
  : sizeX_(1.0), sizeY_(1.0), pitchX_(1.0), pitchY_(1.0), thickness_(0.0)
{
}

CalcWPPixel::~CalcWPPixel() = default;

void CalcWPPixel::initializeTable()
{
  const double a = SizeX();
  const double U = PitchX();
  const double b = SizeY();
  const double V = PitchY();
  const double L = Thickness();
  
  for (int m=1; m<=NumGrids; m++) {
    alpha_[m-1] = (TMath::Pi()*m)/a;
  }

  for (int n=1; n<=NumGrids; n++) {
    beta_[n-1] = (TMath::Pi()*n)/b;
  }
  
  double fm ,fn;
  for (int m=1; m<=NumGrids; m++) {
    fm = TMath::Cos(TMath::Pi()*m*(a-U)/(2.0*a)) - TMath::Cos(TMath::Pi()*m*(a+U)/(2.0*a));
    for (int n=1; n<=NumGrids; n++) {
      fn = TMath::Cos(TMath::Pi()*n*(b-V)/(2.0*b)) - TMath::Cos(TMath::Pi()*n*(b+V)/(2.0*b));
      gamma_[m-1][n-1] = TMath::Sqrt(alpha_[m-1]*alpha_[m-1]+beta_[n-1]*beta_[n-1]);
      a0_[m-1][n-1] = 4.0/(TMath::Pi()*TMath::Pi()*m*n*TMath::SinH(gamma_[m-1][n-1]*L))*fm*fn;
    }
  }
}

void CalcWPPixel::printTable()
{
  for (int m=0; m<NumGrids; m++) {
    for (int n=0; n<NumGrids; n++) {
      printf("%4d %4d   %+10.4e  %+10.4e  %+10.4e  %+10.4e\n", m, n, alpha_[m], beta_[n], gamma_[m][n], a0_[m][n]);
    }
  }
}

void CalcWPPixel::setXY(double x0, double y0)
{
  double x = 0.5*SizeX() + x0;
  double y = 0.5*SizeY() + y0;

  for (int m=1; m<=NumGrids; m++) {
    sinAX_[m-1] = TMath::Sin(alpha_[m-1]*x);
  }

  for (int n=1; n<=NumGrids; n++) {
    sinBY_[n-1] = TMath::Sin(beta_[n-1]*y);
  }
}

double CalcWPPixel::WeightingPotential(double x0, double y0, double z0)
{
  setXY(x0, y0);
  return WeightingPotential(z0);
}

double CalcWPPixel::WeightingPotential(double z0)
{
  double z = 0.5*Thickness() + z0;
  double sinhg;
  double phi = 0.0;
  for (int m=NumGrids; m>=1; m--) {
    for (int n=NumGrids; n>=1; n--) {
      if (a0_[m-1][n-1]==0.0) continue;
      sinhg = TMath::SinH(gamma_[m-1][n-1]*z);
      phi += sinhg * a0_[m-1][n-1] * sinAX_[m-1] * sinBY_[n-1];
    }
  }
  return phi;
}

} /* namespace comptonsoft */
