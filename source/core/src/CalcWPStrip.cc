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

#include "CalcWPStrip.hh"
#include <iostream>
#include "TMath.h"

namespace comptonsoft {

CalcWPStrip::CalcWPStrip()
  : sizeX_(1.0), pitchX_(1.0), thickness_(1.0)
{
}

CalcWPStrip::~CalcWPStrip() = default;

void CalcWPStrip::initializeTable()
{
  const double a = SizeX();
  const double U = PitchX();
  const double L = Thickness();
  
  for (int m=1; m<=NumGrids; m++) {
    alpha_[m-1] = (TMath::Pi()*m)/a;
  }

  double fm;
  for (int m=1; m<=NumGrids; m++) {
    fm = TMath::Cos(TMath::Pi()*m*(a-U)/(2.0*a)) - TMath::Cos(TMath::Pi()*m*(a+U)/(2.0*a));
    a0_[m-1] = 2.0/(TMath::Pi()*m*TMath::SinH(alpha_[m-1]*L))*fm;
  }
}

void CalcWPStrip::printTable()
{
  for (int m=0; m<NumGrids; m++) {
    printf("%4d   %+10.4e  %+10.4e\n", m, alpha_[m], a0_[m]);
  }
}

void CalcWPStrip::setX(double x0)
{
  double x = 0.5*SizeX() + x0;

  for (int m=1; m<=NumGrids; m++) {
    sinAX_[m-1] = TMath::Sin(alpha_[m-1]*x);
  }
}

double CalcWPStrip::WeightingPotential(double x0, double z0)
{
  setX(x0);
  return WeightingPotential(z0);
}

double CalcWPStrip::WeightingPotential(double z0)
{
  double z = 0.5*Thickness() + z0;
  double sinhg;
  double phi = 0.0;
  for (int m=NumGrids; m>=1; m--) {
    if (a0_[m-1]==0.0) continue;
    sinhg = TMath::SinH(alpha_[m-1]*z);
    phi += sinhg * a0_[m-1] * sinAX_[m-1];
  }
  
  return phi;
}

} /* namespace comptonsoft */
