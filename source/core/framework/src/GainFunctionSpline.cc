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

#include "GainFunctionSpline.hh"
#include "TSpline.h"

namespace comptonsoft
{

GainFunctionSpline::GainFunctionSpline()
{
}

GainFunctionSpline::~GainFunctionSpline() = default;
  
double GainFunctionSpline::RangeMin() const
{
  return func_->GetXmin();
}

double GainFunctionSpline::RangeMax() const
{
  return func_->GetXmax();
}

double GainFunctionSpline::eval(double x) const
{
  return func_->Eval(x);
}

void GainFunctionSpline::set(const TSpline* func)
{
  func_ = func;
}

} /* namespace comptonsoft */
