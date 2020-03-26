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

#ifndef COMPTONSOFT_VGainFunction_H
#define COMPTONSOFT_VGainFunction_H 1

#include <cstddef>

namespace comptonsoft {

/**
 * A virtual class of a gain correction function.
 *
 * @author Hirokazu Odaka
 * @date 2014-09-12
 * @date 2020-03-26 | of a single channel (previously of a channel array)
 */
class VGainFunction
{
public:
  VGainFunction();
  virtual ~VGainFunction();

  VGainFunction(const VGainFunction&) = default;
  VGainFunction(VGainFunction&&) = default;
  VGainFunction& operator=(const VGainFunction&) = default;
  VGainFunction& operator=(VGainFunction&&) = default;
  
  virtual double RangeMin() const = 0;
  virtual double RangeMax() const = 0;
  virtual double eval(double x) const = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VGainFunction_H */
