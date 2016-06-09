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

#include "IsotopeInfo.hh"
#include "G4SystemOfUnits.hh"

namespace comptonsoft {

int64_t IsotopeInfo::makeID(int z, int a, double energy)
{
  /* Isotope ID :xxyyyzzzzzzzzz *
   * Z = xx                     *
   * A = yyy                    *
   * E = zzz zzz zzz [eV]       */

  int64_t id = 0L;
  id += static_cast<int64_t>(z) * 1000000000L * 1000L;
  id += static_cast<int64_t>(a) * 1000000000L;
  id += static_cast<int64_t>(energy/eV) % 1000000000L;
  return id;
}

IsotopeInfo::IsotopeInfo()
  : Z_(0), A_(0), energy_(0.0), counts_(0), rate_(0.0)
{
}

IsotopeInfo::IsotopeInfo(int64_t isotopeID)
  : Z_(0), A_(0), energy_(0.0), counts_(0), rate_(0.0)
{
  Z_ = isotopeID / (1000000000L * 1000L);
  A_ = (isotopeID % (1000000000L * 1000L)) / 1000000000L;
  energy_ = (isotopeID % 1000000000L) * eV;
}

IsotopeInfo::IsotopeInfo(int z, int a, double energy)
  : Z_(z), A_(a), energy_(energy), counts_(0), rate_(0.0)
{
}

IsotopeInfo::~IsotopeInfo() = default;

int64_t IsotopeInfo::IsotopeID() const
{
  return IsotopeInfo::makeID(Z_, A_, energy_);
}

} /* namespace comptonsoft */
