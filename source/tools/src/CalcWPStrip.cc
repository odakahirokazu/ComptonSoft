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

void CalcWPStrip::init_table()
{
  const double a = SizeX();
  const double U = PitchX();
  const double L = Thickness();
  
  int m;
  for (m=1; m<=NGRID; m++) {
    Alpha[m-1] = (TMath::Pi()*m)/a;
  }

  double fm;
  for (m=1; m<=NGRID; m++) {
    fm = TMath::Cos(TMath::Pi()*m*(a-U)/(2.0*a)) - TMath::Cos(TMath::Pi()*m*(a+U)/(2.0*a));
    A0[m-1] = 2.0/(TMath::Pi()*m*TMath::SinH(Alpha[m-1]*L))*fm;
  }
}


void CalcWPStrip::print_table()
{
  for (int m=0; m<NGRID; m++) {
    printf("%4d   %+10.4e  %+10.4e\n", m, Alpha[m], A0[m]);
  }
}


void CalcWPStrip::set_x(double x0)
{
  double x = 0.5*SizeX() + x0;

  for (int m=1; m<=NGRID; m++) {
    SinAX[m-1] = TMath::Sin(Alpha[m-1]*x);
  }
}


double CalcWPStrip::weighting_potential(double x0, double z0)
{
  set_x(x0);
  return weighting_potential(z0);
}


double CalcWPStrip::weighting_potential(double z0)
{
  double z = 0.5*Thickness() + z0;
  double sinhg;
  double phi = 0.0;
  for (int m=NGRID; m>=1; m--) {
    if (A0[m-1]==0.0) continue;
    sinhg = TMath::SinH(Alpha[m-1]*z);
    phi += sinhg * A0[m-1] * SinAX[m-1];
  }
  
  return phi;
}

}
