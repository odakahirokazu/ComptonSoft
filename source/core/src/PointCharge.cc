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

#include "PointCharge.hh"

namespace comptonsoft {

PointCharge::PointCharge()
  : signals_(boost::extents[PixelExtenstion][PixelExtenstion]),
    shifts_(boost::extents[PixelExtenstion][PixelExtenstion])
{
}

PointCharge::~PointCharge() = default;

void PointCharge::setPixelSeparations(const double x, const double y)
{
  shifts_[0][0] = vector3_t(-x,   -y, 0.0);
  shifts_[0][1] = vector3_t(-x,  0.0, 0.0);
  shifts_[0][2] = vector3_t(-x,   +y, 0.0);
  shifts_[1][0] = vector3_t(0.0,  -y, 0.0);
  shifts_[1][1] = vector3_t(0.0, 0.0, 0.0);
  shifts_[1][2] = vector3_t(0.0,  +y, 0.0);
  shifts_[2][0] = vector3_t(+x,   -y, 0.0);
  shifts_[2][1] = vector3_t(+x,  0.0, 0.0);
  shifts_[2][2] = vector3_t(+x,   +y, 0.0);
}

boost::multi_array<double, 2> PointCharge::transport(const double dl)
{
  {
    double* p = signals_.origin();
    for (std::size_t i=0; i<signals_.num_elements(); ++i) {
      *p = 0.0;
      ++p;
    }
  }

  while (true) {
    advance(dl);
    if (t_ > endingTime_) { break; }
    if (goal_->isReached(position_)) { break; }
  }
  return signals_;
}

void PointCharge::advance(const double dl)
{
  t0_ = t_;
  position0_ = position_;

  const vector3_t velocity = field_->ChargeVelocity(position_);
  const double v = velocity.mag();
  const double tau = field_->Lifetime();
  const double dt = (v != 0.0) ? (dl/v) : tau;
  
  t_ += dt;
  position_ += velocity*dt;
  position_ += field_->Diffusion(dt);
  
  integrateSignals();
  effectiveQ_ *= std::exp(-dt/tau);
}

void PointCharge::integrateSignals()
{
  for (int i=0; i<PixelExtenstion; i++) {
    for (int j=0; j<PixelExtenstion; j++) {
      const vector3_t r1 = position_  + shifts_[i][j];
      const vector3_t r0 = position0_ + shifts_[i][j];
      const double wp1 = field_->WeightingPotential(r1);
      const double wp0 = field_->WeightingPotential(r0);
      signals_[i][j] += effectiveQ_*(wp1-wp0);
    }
  }
}

} /* namespace comptonsoft */
