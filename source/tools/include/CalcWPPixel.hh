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

#ifndef COMPTONSOFT_CalcWPPixel_H
#define COMPTONSOFT_CalcWPPixel_H 1

namespace comptonsoft {

/**
 * A class for calculating weighting potentials in a pixel detector.
 * @author Hirokazu Odaka
 */

class CalcWPPixel
{
public:
  /**
   * set geometry parameters
   * @param pitch_x the pixel pitch along the x axis of the detector
   * @param pitch_y the pixel pitch along the y axis of the detector
   * @param thickness the thickness of the detector
   * @param pixel_num the number of pixels used for the caluculation
   */
  void set_geometry(double pitch_x,
                    double pitch_y,
                    double thickness,
                    double pixel_num=5.0)
  {
    m_SizeX = pitch_x * pixel_num;
    m_SizeY = pitch_y * pixel_num;
    m_PitchX = pitch_x; 
    m_PitchY = pitch_y;
    m_Thickness = thickness;
  }

  double SizeX() { return m_SizeX; }
  double SizeY() { return m_SizeY; }
  double PitchX() { return m_PitchX; }
  double PitchY() { return m_PitchY; }
  double Thickness() { return m_Thickness; }
  
  void init_table();
  void print_table();
  void set_xy(double x0, double y0);
  double weighting_potential(double x0, double y0, double z0);
  double weighting_potential(double z0);

private:
  static const int NGRID = 500;

  double m_SizeX;
  double m_SizeY;
  double m_PitchX;
  double m_PitchY;
  double m_Thickness;

  double Alpha[NGRID];
  double Beta[NGRID];
  double Gamma[NGRID][NGRID];
  double A0[NGRID][NGRID];
  
  double SinAX[NGRID];
  double SinBY[NGRID];
};

}

#endif /* COMPTONSOFT_CalcWPPixel_H */
