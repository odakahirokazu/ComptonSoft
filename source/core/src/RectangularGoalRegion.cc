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

#include "RectangularGoalRegion.hh"

namespace comptonsoft {

void RectangularGoalRegion::
addRegion(double x0, double x1, double y0, double y1, double z0, double z1)
{
  regions_.push_back(std::make_tuple(x0, x1, y0, y1, z0, z1));
}

bool RectangularGoalRegion::isReached(const vector3_t& position)
{
  const double x = position.x();
  const double y = position.y();
  const double z = position.z();
  
  for (const region_t& r: regions_) {
    const double rx0 = std::get<0>(r); const double rx1 = std::get<1>(r);
    const double ry0 = std::get<2>(r); const double ry1 = std::get<3>(r);
    const double rz0 = std::get<4>(r); const double rz1 = std::get<5>(r);
    if (rx0<=x && x<=rx1 && ry0<=y && y<=ry1 && rz0<=z && z<=rz1) {
      return true;
    }
  }
  return false;
}

} /* namespace comptonsoft */
