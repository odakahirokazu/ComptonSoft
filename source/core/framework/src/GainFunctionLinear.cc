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

#include "GainFunctionLinear.hh"
#include <limits>

namespace comptonsoft
{

GainFunctionLinear::GainFunctionLinear()
  : c0_(0.0), c1_(1.0)
{
}

GainFunctionLinear::GainFunctionLinear(double c0, double c1)
  : c0_(c0), c1_(c1)
{
}

GainFunctionLinear::~GainFunctionLinear() = default;

double GainFunctionLinear::RangeMin() const
{
  return -std::numeric_limits<double>::max();
}

double GainFunctionLinear::RangeMax() const
{
  return +std::numeric_limits<double>::max();
}

double GainFunctionLinear::eval(double x) const
{
  return c0_ + c1_*x;
}

void GainFunctionLinear::set(double c0, double c1)
{
  c0_ = c0;
  c1_ = c1;
}

} /* namespace comptonsoft */
