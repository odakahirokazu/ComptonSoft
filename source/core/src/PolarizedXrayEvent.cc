/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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

#include "PolarizedXrayEvent.hh"

#include <iostream>
#include <cmath>

#include "AstroUnits.hh"

namespace comptonsoft
{

PolarizedXrayEvent::PolarizedXrayEvent(int size)
  : XrayEvent(size)
{
}

PolarizedXrayEvent::~PolarizedXrayEvent() = default;

/**
   Event angle is determined by the maximum of 2nd momentum
 */
double PolarizedXrayEvent::calculateEventAngle() const
{
  using std::atan;
  using std::cos;
  using std::sin;
  using anlgeant4::constant::halfpi;

  const int size = EventSize();
  const int center = size/2;

  double xsum(0.0), ysum(0.0), esum(0.0);
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      const double epi = Data()[i][j];
      const double x1 = static_cast<double>(i-center);
      const double y1 = static_cast<double>(j-center);
      xsum += x1*epi;
      ysum += y1*epi;
      esum += epi;
    }
  }

  const double xcm = xsum/esum;
  const double ycm = ysum/esum;

  double u11(0.0), u20(0.0), u02(0.0);
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      const double epi = Data()[i][j];
      u11 += (i - xcm) * (j - ycm) * epi;
      u20 += (i - xcm) * (i - xcm) * epi;
      u02 += (j - ycm) * (j - ycm) * epi;
    }
  }

  const double tan2phi = 2 * u11/(u20 - u02);
  const double two_phi1 = atan(tan2phi);
  const double phi1 = 0.5*two_phi1;
  const double phi2 = 0.5*two_phi1 + halfpi;

  const double A = (u20-u02)*cos(2.0*phi1) + u11*sin(2.0*phi1);
  const double phi = (A>=0.0) ? phi1 : phi2;
  return phi;
}

} /* namespace comptonsoft */
