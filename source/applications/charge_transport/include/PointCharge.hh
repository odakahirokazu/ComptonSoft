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

#ifndef COMPTONSOFT_PointCharge_H
#define COMPTONSOFT_PointCharge_H 1

#include <boost/multi_array.hpp>
#include "CSTypes.hh"
#include "VGoalRegion.hh"
#include "SemiconductorModel.hh"

namespace comptonsoft {

/**
 * A point charge class for charge transport calculation
 *
 * @author Hiromasa Suzuki, Hirokazu Odaka
 * @date 2020-04-18
 */
class PointCharge
{
public:
  PointCharge();
  virtual ~PointCharge();

  void setTime(double t) { t_ = t; }
  double Time() const { return t_; }

  void setPosition(const vector3_t& position) { position_ = position; }
  vector3_t Position() const { return position_; }

  void setGoal(VGoalRegion* p) { goal_ = p; }

  void setEffectiveQ(double v) { effectiveQ_ = v; }
  double EffectiveQ() const { return effectiveQ_; }

  void setEndingTime(double v) { endingTime_ = v; }
  double EndingTime() const { return endingTime_; }

  void setPixelSeparations(double x, double y);

  void setField(SemiconductorModel* field) { field_ = field; }

  boost::multi_array<double, 2> transport(double dl);

private:
  void advance(double dl);
  void integrateSignals();

private:
  static constexpr int PixelExtenstion = 3;
  double t_ = 0.0;
  vector3_t position_;
  double t0_ = 0.0;
  vector3_t position0_;
  double effectiveQ_ = 1.0;
  double endingTime_ = 0.0;
  boost::multi_array<double, 2> signals_;
  boost::multi_array<vector3_t, 2> shifts_;
  SemiconductorModel* field_ = nullptr;
  VGoalRegion* goal_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PointCharge_H */
