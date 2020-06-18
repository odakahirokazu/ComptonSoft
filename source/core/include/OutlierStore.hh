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

#ifndef COMPTONSOFT_OutlierStore_H
#define COMPTONSOFT_OutlierStore_H 1

#include <vector>

namespace comptonsoft
{

/**
 * A class of an outlier store for basic statistics
 *
 * @author Hirokazu Odaka
 * @date 2020-05-28
 */
class OutlierStore
{
public:
  OutlierStore();
  OutlierStore(int capacity_low, int capacity_high);
  ~OutlierStore();

  void set_capacities(int low, int high);
  int capacity_low() const { return capacity_low_; }
  int capacity_high() const { return capacity_high_; }
  int capacity() const { return capacity_low_ + capacity_high_; }
  
  int num() const { return num_; }
  double sum() const;
  double sum2() const;
  void propose(double x);
  
private:
  int capacity_low_ = 0;
  int capacity_high_ = 0;
  int num_ = 0;
  std::vector<double> xs_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_OutlierStore_H */
