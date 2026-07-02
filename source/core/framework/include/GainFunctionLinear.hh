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

#ifndef COMPTONSOFT_GainFunctionLinear_H
#define COMPTONSOFT_GainFunctionLinear_H 1

#include "VGainFunction.hh"
#include <vector>

namespace comptonsoft {

/**
 * A class of a gain correction function using a linear function.
 *
 * @author Hirokazu Odaka
 * @date 2014-09-12
 * @date 2020-03-26 | adapt a change of VGainFunction
 */
class GainFunctionLinear : public VGainFunction
{
public:
  GainFunctionLinear();
  GainFunctionLinear(double c0, double c1);
  virtual ~GainFunctionLinear();
  
  GainFunctionLinear(const GainFunctionLinear&) = default;
  GainFunctionLinear(GainFunctionLinear&&) = default;
  GainFunctionLinear& operator=(const GainFunctionLinear&) = default;
  GainFunctionLinear& operator=(GainFunctionLinear&&) = default;

  double RangeMin() const override;
  double RangeMax() const override;

  double eval(double x) const override;
  void set(double c0, double c1);
  
private:
  double c0_;
  double c1_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_GainFunctionLinear_H */
