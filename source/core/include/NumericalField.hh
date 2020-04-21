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

#ifndef COMPTONSOFT_NumericalField_H
#define COMPTONSOFT_NumericalField_H 1

#include <vector>
#include <boost/multi_array.hpp>
#include "CSTypes.hh"

namespace comptonsoft {

/**
 * A field fucntion with a numerical table
 *
 * @author Hirokazu Odaka
 * @date 2020-04-18
 */
class NumericalField
{
public:
  NumericalField() = default;
  virtual ~NumericalField();

  void defineGrids(const std::vector<double>& xs,
                   const std::vector<double>& ys,
                   const std::vector<double>& zs);
  void setZeros();
  
  std::size_t num_x() const { return xs_.size(); }
  std::size_t num_y() const { return ys_.size(); }
  std::size_t num_z() const { return zs_.size(); }
  double get_x(std::size_t i) const { return xs_[i]; }
  double get_y(std::size_t i) const { return ys_[i]; }
  double get_z(std::size_t i) const { return zs_[i]; }
  vector3_t get_position(std::size_t ix,
                         std::size_t iy,
                         std::size_t iz) const
  {
    return vector3_t(get_x(ix), get_y(iy), get_z(iz));
  }

  void set_field_value(std::size_t ix,
                       std::size_t iy,
                       std::size_t iz,
                       double v)
  {
    field_[ix][iy][iz] = v;
  }

  double get_field_value(std::size_t ix,
                         std::size_t iy,
                         std::size_t iz) const
  {
    return field_[ix][iy][iz];
  }

  double field(double x, double y, double z) const;

  double field(const vector3_t& position) const
  {
    return field(position.x(), position.y(), position.z());
  }

private:
  std::vector<double> xs_;
  std::vector<double> ys_;
  std::vector<double> zs_;
  boost::multi_array<double, 3> field_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NumericalField_H */
