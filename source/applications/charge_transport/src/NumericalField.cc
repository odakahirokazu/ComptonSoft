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

#include "NumericalField.hh"
#include <algorithm>
#include <tuple>

namespace {

std::tuple<std::size_t, std::size_t>
find_index_pair(double a, const std::vector<double>& as);

} /* unnamed namespace */

namespace comptonsoft {

NumericalField::~NumericalField() = default;

void NumericalField::
defineGrids(const std::vector<double>& xs,
            const std::vector<double>& ys,
            const std::vector<double>& zs)
{
  xs_ = xs;
  ys_ = ys;
  zs_ = zs;
  const std::size_t nx = xs_.size();
  const std::size_t ny = ys_.size();
  const std::size_t nz = zs_.size();
  field_.resize(boost::extents[nx][ny][nz]);
}

void NumericalField::setZeros()
{
  double* p = field_.origin();
  for (std::size_t i=0; i<field_.num_elements(); ++i) {
    *p = 0.0;
    ++p;
  }
}

double NumericalField::field(const double x,
                             const double y,
                             const double z) const
{
  std::size_t ix0, ix1, iy0, iy1, iz0, iz1;
  std::tie(ix0, ix1) = find_index_pair(x, xs_);
  std::tie(iy0, iy1) = find_index_pair(y, ys_);
  std::tie(iz0, iz1) = find_index_pair(z, zs_);
  const double x0 = get_x(ix0); const double x1 = get_x(ix1);
  const double y0 = get_y(iy0); const double y1 = get_y(iy1);
  const double z0 = get_z(iz0); const double z1 = get_z(iz1);
  const double rx = (ix0==ix1) ? 0.5 : (x-x0)/(x1-x0);
  const double ry = (iy0==iy1) ? 0.5 : (y-y0)/(y1-y0);
  const double rz = (iz0==iz1) ? 0.5 : (z-z0)/(z1-z0);
  const double v000 = get_field_value(ix0, iy0, iz0);
  const double v001 = get_field_value(ix0, iy0, iz1);
  const double v010 = get_field_value(ix0, iy1, iz0);
  const double v011 = get_field_value(ix0, iy1, iz1);
  const double v100 = get_field_value(ix1, iy0, iz0);
  const double v101 = get_field_value(ix1, iy0, iz1);
  const double v110 = get_field_value(ix1, iy1, iz0);
  const double v111 = get_field_value(ix1, iy1, iz1);
  const double c000 = (1.0-rx)*(1.0-ry)*(1.0-rz);
  const double c001 = (1.0-rx)*(1.0-ry)*     rz;
  const double c010 = (1.0-rx)*     ry *(1.0-rz);
  const double c011 = (1.0-rx)*     ry *     rz;
  const double c100 =      rx *(1.0-ry)*(1.0-rz);
  const double c101 =      rx *(1.0-ry)*     rz;
  const double c110 =      rx *     ry *(1.0-rz);
  const double c111 =      rx *     ry *     rz;
  const double v = c000*v000 + c001*v001 + c010*v010 + c011*v011 + c100*v100 + c101*v101 + c110*v110 + c111*v111;
  return v;
}

} /* namespace comptonsoft */

namespace {

std::tuple<std::size_t, std::size_t> find_index_pair(const double a, const std::vector<double>& as)
{
  using iter_t = std::vector<double>::const_iterator;
  const iter_t it = std::upper_bound(as.begin(), as.end(), a);

  if (it==as.begin()) {
    return std::make_tuple(0, 0);
  }
  else if (it==as.end()) {
    const std::size_t n = as.size();
    return std::make_tuple(n-1, n-1);
  }

  const std::size_t i1 = it-as.begin();
  return std::make_tuple(i1-1, i1);
}

} /* unnamed namespace */
