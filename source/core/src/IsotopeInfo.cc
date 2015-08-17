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

namespace comptonsoft {

IsotopeInfo::IsotopeInfo()
  : Z_(0), A_(0), energy_(0.0), counts_(0), rate_(0.0)
{
}

IsotopeInfo::IsotopeInfo(int z, int a, double energy)
  : Z_(z), A_(a), energy_(energy), counts_(0), rate_(0.0)
{
}

IsotopeInfo::~IsotopeInfo() = default;

} /* namespace comptonsoft */
