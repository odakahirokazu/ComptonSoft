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

void CalcWPPixel::init_table()
{
  const double a = SizeX();
  const double U = PitchX();
  const double b = SizeY();
  const double V = PitchY();
  const double L = Thickness();
  
  int m,n;
  for (m=1; m<=NGRID; m++) {
    Alpha[m-1] = (TMath::Pi()*m)/a;
  }

  for (n=1; n<=NGRID; n++) {
    Beta[n-1] = (TMath::Pi()*n)/b;
  }
  
  double fm ,fn;
  for (m=1; m<=NGRID; m++) {
    fm = TMath::Cos(TMath::Pi()*m*(a-U)/(2.0*a)) - TMath::Cos(TMath::Pi()*m*(a+U)/(2.0*a));
    for (n=1; n<=NGRID; n++) {
      fn = TMath::Cos(TMath::Pi()*n*(b-V)/(2.0*b)) - TMath::Cos(TMath::Pi()*n*(b+V)/(2.0*b));
      Gamma[m-1][n-1] = TMath::Sqrt(Alpha[m-1]*Alpha[m-1]+Beta[n-1]*Beta[n-1]);
      A0[m-1][n-1] = 4.0/(TMath::Pi()*TMath::Pi()*m*n*TMath::SinH(Gamma[m-1][n-1]*L))*fm*fn;
    }
  }
}


void CalcWPPixel::print_table()
{
  for (int m=0; m<NGRID; m++) {
    for (int n=0; n<NGRID; n++) {
      printf("%4d %4d   %+10.4e  %+10.4e  %+10.4e  %+10.4e\n", m, n, Alpha[m], Beta[n], Gamma[m][n], A0[m][n]);
    }
  }
}


void CalcWPPixel::set_xy(double x0, double y0)
{
  double x = 0.5*SizeX() + x0;
  double y = 0.5*SizeY() + y0;

  for (int m=1; m<=NGRID; m++) {
    SinAX[m-1] = TMath::Sin(Alpha[m-1]*x);
  }

  for (int n=1; n<=NGRID; n++) {
    SinBY[n-1] = TMath::Sin(Beta[n-1]*y);
  }
}


double CalcWPPixel::weighting_potential(double x0, double y0, double z0)
{
  set_xy(x0, y0);
  return weighting_potential(z0);
}


double CalcWPPixel::weighting_potential(double z0)
{
  double z = 0.5*Thickness() + z0;
  double sinhg;
  double phi = 0.0;
  for (int m=NGRID; m>=1; m--) {
    for (int n=NGRID; n>=1; n--) {
      if (A0[m-1][n-1]==0.0) continue;
      sinhg = TMath::SinH(Gamma[m-1][n-1]*z);
      phi += sinhg * A0[m-1][n-1] * SinAX[m-1] * SinBY[n-1];
    }
  }
  return phi;
}

}
