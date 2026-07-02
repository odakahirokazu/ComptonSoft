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

#include "PhaseSpaceVector.hh"

namespace comptonsoft {

PhaseSpaceVector::PhaseSpaceVector()
  : energy_(0.0),
    directionX_(0.0), directionY_(0.0), directionZ_(-1.0),
    time_(0.0),
    positionX_(0.0), positionY_(0.0), positionZ_(0.0)
{
}

PhaseSpaceVector::PhaseSpaceVector(double energy,
                                   double dirx, double diry, double dirz)
  : energy_(energy),
    directionX_(dirx), directionY_(diry), directionZ_(dirz),
    time_(0.0),
    positionX_(0.0), positionY_(0.0), positionZ_(0.0)
{
}

PhaseSpaceVector::PhaseSpaceVector(double energy,
                                   double dirx, double diry, double dirz,
                                   double time0,
                                   double posx, double posy, double posz)
  : energy_(energy),
    directionX_(dirx), directionY_(diry), directionZ_(dirz),
    time_(time0),
    positionX_(posx), positionY_(posy), positionZ_(posz)
{
}

PhaseSpaceVector::~PhaseSpaceVector() = default;

} /* namespace comptonsoft */
