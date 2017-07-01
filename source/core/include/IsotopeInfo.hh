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

#ifndef COMPTONSOFT_IsotopeInfo_H
#define COMPTONSOFT_IsotopeInfo_H 1

#include <cstdint>

namespace comptonsoft {

/**
 * Isotope class.
 *
 * @author Hirokazu Odaka
 * @date 2012-02-06
 * @date 2016-05-08
 * @date 2017-07-26 | add property floating level
 */
class IsotopeInfo
{
public:
  static int64_t makeID(int z, int a, double energy, int floating_level=0);

public:
  IsotopeInfo();
  explicit IsotopeInfo(int64_t isotopeID);
  IsotopeInfo(int z, int a, double energy, int floating_level=0);
  ~IsotopeInfo();
  
  IsotopeInfo(const IsotopeInfo&) = default;
  IsotopeInfo(IsotopeInfo&&) = default;
  IsotopeInfo& operator=(const IsotopeInfo&) = default;
  IsotopeInfo& operator=(IsotopeInfo&&) = default;
  
  int Z() const { return Z_; }
  int A() const { return A_; }
  double Energy() const { return energy_; }
  int FloatingLevel() const { return floating_level_; }
  int Counts() const { return counts_; }
  double Rate() const { return rate_; }

  void setFloatingLevel(int v) { floating_level_ = v; }
  
  void setCounts(int v) { counts_ = v; }
  void add1() { counts_++; }

  void setRate(double v) { rate_ = v; }
  void addRate(double v) { rate_ += v; }

  int64_t IsotopeID() const;

private:
  int Z_;
  int A_;
  double energy_;
  int floating_level_;
  int counts_;
  double rate_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_IsotopeInfo_H */
