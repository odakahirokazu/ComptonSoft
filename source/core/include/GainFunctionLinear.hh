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
 * A class of a gain correction function using linear functions.
 *
 * @author Hirokazu Odaka
 * @date 2014-09-12
 */
class GainFunctionLinear : public VGainFunction
{
public:
  GainFunctionLinear(std::size_t n, double c1);
  ~GainFunctionLinear();
  
  GainFunctionLinear(const GainFunctionLinear&) = default;
  GainFunctionLinear(GainFunctionLinear&&) = default;
  GainFunctionLinear& operator=(const GainFunctionLinear&) = default;
  GainFunctionLinear& operator=(GainFunctionLinear&&) = default;

  double RangeMin(std::size_t) const;
  double RangeMax(std::size_t) const;

  double eval(std::size_t index, double x) const;
  void set(std::size_t index, double c0, double c1);
  
private:
  std::vector<double> c0_;
  std::vector<double> c1_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_GainFunctionLinear_H */
