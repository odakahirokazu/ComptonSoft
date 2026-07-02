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

#ifndef COMPTONSOFT_VGoalRegion_H
#define COMPTONSOFT_VGoalRegion_H 1

#include "CSTypes.hh"

namespace comptonsoft {

/**
 * A virtual class of a goal region for charge transport
 *
 * @author Hirokazu Odaka
 * @date 2020-04-18
 */
class VGoalRegion
{
public:
  VGoalRegion() = default;
  virtual ~VGoalRegion();

  virtual bool isReached(const vector3_t&) = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VGoalRegion_H */
