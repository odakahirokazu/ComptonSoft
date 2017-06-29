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

#ifndef COMPTONSOFT_GainFunctionSpline_H
#define COMPTONSOFT_GainFunctionSpline_H 1

#include "VGainFunction.hh"
#include <vector>

class TSpline;

namespace comptonsoft {

/**
 * A class of a gain correction function using spline functions.
 *
 * @author Hirokazu Odaka
 * @date 2014-09-12
 */
class GainFunctionSpline : public VGainFunction
{
public:
  explicit GainFunctionSpline(std::size_t n);
  virtual ~GainFunctionSpline();

  GainFunctionSpline(const GainFunctionSpline&) = default;
  GainFunctionSpline(GainFunctionSpline&&) = default;
  GainFunctionSpline& operator=(const GainFunctionSpline& r) = default;
  GainFunctionSpline& operator=(GainFunctionSpline& rr) = default;
  
  double RangeMin(std::size_t index) const override;
  double RangeMax(std::size_t index) const override;
  double eval(std::size_t index, double x) const override;
  void set(std::size_t index, const TSpline* func);

private:
  std::vector<const TSpline*> func_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_GainFunctionSpline_H */
