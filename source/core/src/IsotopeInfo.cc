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

/**
 * @file IsotopeInfo.cc
 * @brief source file of class IsotopeInfo
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#include "IsotopeInfo.hh"
#include "AstroUnits.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

int64_t IsotopeInfo::makeID(int z, int a, double energy, int floating_level)
{
  /* Isotope ID :xxyyyzzzzzzzzzww *
   * Z = xx                       *
   * A = yyy                      *
   * E = zzz zzz zzz [eV]         *
   * floating level = ww
   */

  int64_t id = 0L;
  id += (static_cast<int64_t>(z) % 100L);
  id *= 1000L;
  id += (static_cast<int64_t>(a) % 1000L);
  id *= 1000000000L;
  id += (static_cast<int64_t>(energy/unit::eV) % 1000000000L);
  id *= 100L;
  id += (static_cast<int64_t>(floating_level) % 100L);
  return id;
}

IsotopeInfo::IsotopeInfo()
  : Z_(0), A_(0), energy_(0.0), floating_level_(0),
    counts_(0), rate_(0.0)
{
}

IsotopeInfo::IsotopeInfo(int64_t isotopeID)
  : Z_(0), A_(0), energy_(0.0), floating_level_(0),
    counts_(0), rate_(0.0)
{
  int64_t id = isotopeID;
  floating_level_ = id % 100L;
  id /= 100L;
  energy_ = (id % 1000000000L) * unit::eV;
  id /= 1000000000L;
  A_ = id % 1000L;
  id /= 1000L;
  Z_ = id;
}

IsotopeInfo::IsotopeInfo(int z, int a, double energy, int floating_level)
  : Z_(z), A_(a), energy_(energy), floating_level_(floating_level),
    counts_(0), rate_(0.0)
{
}

IsotopeInfo::~IsotopeInfo() = default;

int64_t IsotopeInfo::IsotopeID() const
{
  return IsotopeInfo::makeID(Z_, A_, energy_, floating_level_);
}

} /* namespace comptonsoft */
