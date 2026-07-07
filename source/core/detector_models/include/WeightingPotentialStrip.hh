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

#ifndef COMPTONSOFT_WeightingPotentialStrip_H
#define COMPTONSOFT_WeightingPotentialStrip_H 1

#include <array>

namespace comptonsoft {

/**
 * A class for calculating weighting potentials in a strip detector.
 * @author Hirokazu Odaka
 */
class WeightingPotentialStrip
{
public:
  WeightingPotentialStrip();
  virtual ~WeightingPotentialStrip();
  WeightingPotentialStrip(const WeightingPotentialStrip&) = default;
  WeightingPotentialStrip(WeightingPotentialStrip&&) = default;
  WeightingPotentialStrip& operator=(const WeightingPotentialStrip&) = default;
  WeightingPotentialStrip& operator=(WeightingPotentialStrip&&) = default;

  /**
   * set geometry parameters
   * @param pitch the strip pitch of the detector
   * @param thickness the thickness of the detector
   * @param strip_num the number of strips used for the caluculation
   */
  void setGeometry(double pitch,
                   double thickness,
                   double strip_num=10.0)
  {
    sizeX_ = pitch * strip_num;
    pitchX_ = pitch; 
    thickness_ = thickness;
  }

  double SizeX() { return sizeX_; }
  double PitchX() { return pitchX_; }
  double Thickness() { return thickness_; }
  
  void initializeTable();
  void printTable();
  void setX(double x0);
  double calculateWeightingPotential(double x0, double z0);
  double calculateWeightingPotential(double z0);

private:
  static const int NumGrids_ = 500;

  double sizeX_;
  double pitchX_;
  double thickness_;

  std::array<double, NumGrids_> alpha_;
  std::array<double, NumGrids_> a0_;

  std::array<double, NumGrids_> sinAX_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_WeightingPotentialStrip_H */
