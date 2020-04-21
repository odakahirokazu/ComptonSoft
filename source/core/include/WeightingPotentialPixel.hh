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

#ifndef COMPTONSOFT_WeightingPotentialPixel_H
#define COMPTONSOFT_WeightingPotentialPixel_H 1

#include <array>
#include <boost/multi_array.hpp>

namespace comptonsoft {

/**
 * A class for calculating weighting potentials in a pixel detector.
 * @author Hirokazu Odaka
 */
class WeightingPotentialPixel
{
public:
  WeightingPotentialPixel();
  virtual ~WeightingPotentialPixel();
  WeightingPotentialPixel(const WeightingPotentialPixel&) = default;
  WeightingPotentialPixel(WeightingPotentialPixel&&) = default;
  WeightingPotentialPixel& operator=(const WeightingPotentialPixel&) = default;
  WeightingPotentialPixel& operator=(WeightingPotentialPixel&&) = default;
  
  /**
   * set geometry parameters
   * @param pitch_x the pixel pitch along the x axis of the detector
   * @param pitch_y the pixel pitch along the y axis of the detector
   * @param thickness the thickness of the detector
   * @param pixel_num the number of pixels used for the caluculation
   */
  void setGeometry(double pitch_x,
                   double pitch_y,
                   double thickness,
                   double pixel_num=5.0)
  {
    sizeX_ = pitch_x * pixel_num;
    sizeY_ = pitch_y * pixel_num;
    pitchX_ = pitch_x; 
    pitchY_ = pitch_y;
    thickness_ = thickness;
  }

  double SizeX() { return sizeX_; }
  double SizeY() { return sizeY_; }
  double PitchX() { return pitchX_; }
  double PitchY() { return pitchY_; }
  double Thickness() { return thickness_; }
  
  void initializeTable();
  void printTable();
  void setXY(double x0, double y0);
  double calculateWeightingPotential(double x0, double y0, double z0);
  double calculateWeightingPotential(double z0);

private:
  static const int NumGrids_ = 500;
  
  double sizeX_;
  double sizeY_;
  double pitchX_;
  double pitchY_;
  double thickness_;

  std::array<double, NumGrids_> alpha_;
  std::array<double, NumGrids_> beta_;
  boost::multi_array<double, 2> gamma_;
  boost::multi_array<double, 2> a0_;
  
  std::array<double, NumGrids_> sinAX_;
  std::array<double, NumGrids_> sinBY_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_WeightingPotentialPixel_H */
