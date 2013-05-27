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

#ifndef COMPTONSOFT_CalcWPStrip_H
#define COMPTONSOFT_CalcWPStrip_H 1

namespace comptonsoft {

/**
 * A class for calculating weighting potentials in a strip detector.
 * @author Hirokazu Odaka
 */

class CalcWPStrip
{
public:
  /**
   * set geometry parameters
   * @param pitch the strip pitch of the detector
   * @param thickness the thickness of the detector
   * @param strip_num the number of strips used for the caluculation
   */
  void set_geometry(double pitch,
                    double thickness,
                    double strip_num=10.0)
  {
    m_SizeX = pitch * strip_num;
    m_PitchX = pitch; 
    m_Thickness = thickness;
  }

  double SizeX() { return m_SizeX; }
  double PitchX() { return m_PitchX; }
  double Thickness() { return m_Thickness; }
  
  void init_table();
  void print_table();
  void set_x(double x0);
  double weighting_potential(double x0, double z0);
  double weighting_potential(double z0);

private:
  static const int NGRID = 10000;

  double m_SizeX;
  double m_PitchX;
  double m_Thickness;

  double Alpha[NGRID];
  double A0[NGRID];

  double SinAX[NGRID];
};

}

#endif /* COMPTONSOFT_CalcWPStrip_H */
