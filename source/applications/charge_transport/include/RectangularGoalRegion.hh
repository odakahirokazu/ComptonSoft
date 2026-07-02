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

#ifndef COMPTONSOFT_RectangularGoalRegion_H
#define COMPTONSOFT_RectangularGoalRegion_H 1

#include "VGoalRegion.hh"
#include <vector>
#include <tuple>

namespace comptonsoft {

/**
 * A class of rectangular goal regions for charge transport
 *
 * @author Hirokazu Odaka
 * @date 2020-04-18
 */
class RectangularGoalRegion : public VGoalRegion
{
public:
  RectangularGoalRegion() = default;
  void addRegion(double x0, double x1, double y0, double y1, double z0, double z1);
  bool isReached(const vector3_t& position) override;

private:
  using region_t = std::tuple<double, double, double, double, double, double>;
  std::vector<region_t> regions_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RectangularGoalRegion_H */
