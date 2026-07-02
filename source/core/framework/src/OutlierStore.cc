/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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

#include "OutlierStore.hh"
#include <algorithm>
#include <numeric>

namespace comptonsoft
{

OutlierStore::OutlierStore() = default;

OutlierStore::OutlierStore(int capacity_low, int capacity_high)
  : capacity_low_(capacity_low),
    capacity_high_(capacity_high),
    xs_(capacity_low_+capacity_high_, 0.0)
{
}

OutlierStore::~OutlierStore() = default;

void OutlierStore::set_capacities(int low, int high)
{
  capacity_low_ = low;
  capacity_high_ = high;
  xs_.assign(capacity_low_+capacity_high_, 0.0);
}

void OutlierStore::propose(const double x)
{
  if (num() < capacity()) {
    const int i = num();
    xs_[i] = x;
    ++num_;
    if (num() == capacity()) {
      std::sort(xs_.begin(), xs_.end());
    }
  }
  else {
    if (capacity_low()!=0) {
      double& x_low = xs_[capacity_low()-1];
      if (x < x_low) {
        x_low = x;
        std::sort(xs_.begin(), xs_.begin()+capacity_low());
      }
    }
    if (capacity_high()!=0) {
      double& x_high = xs_[capacity_low()];
      if (x_high < x) {
        x_high = x;
        std::sort(xs_.begin()+capacity_low(), xs_.end());
      }
    }
  }
}

double OutlierStore::sum() const
{
  return std::accumulate(xs_.begin(), xs_.begin()+num(), 0.0);
}

double OutlierStore::sum2() const
{
  return std::accumulate(xs_.begin(), xs_.begin()+num(), 0.0,
                         [](double s, double x){ return s+x*x; });
}

} /* namespace comptonsoft */
