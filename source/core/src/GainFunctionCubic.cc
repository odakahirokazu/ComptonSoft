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

#include "GainFunctionCubic.hh"
#include <limits>

namespace comptonsoft
{

GainFunctionCubic::GainFunctionCubic()
  : c0_(0.0), c1_(1.0), c2_(0.0), c3_(0.0)
{
}

GainFunctionCubic::GainFunctionCubic(double c0, double c1, double c2, double c3)
  : c0_(c0), c1_(c1), c2_(c2), c3_(c3)
{
}

GainFunctionCubic::~GainFunctionCubic() = default;

double GainFunctionCubic::RangeMin() const
{
  return -std::numeric_limits<double>::max();
}

double GainFunctionCubic::RangeMax() const
{
  return +std::numeric_limits<double>::max();
}

double GainFunctionCubic::eval(double x) const
{
  return c0_ + c1_*x + c2_*x*x + c3_*x*x*x;
}

void GainFunctionCubic::set(double c0, double c1, double c2, double c3)
{
  c0_ = c0;
  c1_ = c1;
  c2_ = c2;
  c3_ = c3;
}

} /* namespace comptonsoft */
