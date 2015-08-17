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

#ifndef COMPTONSOFT_DetectorHit_sptr_H
#define COMPTONSOFT_DetectorHit_sptr_H 1

#include <memory>
#include <vector>

namespace comptonsoft {

class DetectorHit;

typedef std::shared_ptr<DetectorHit> DetectorHit_sptr;
typedef std::shared_ptr<const DetectorHit> const_DetectorHit_sptr;
typedef std::vector<DetectorHit_sptr> DetectorHitVector;

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DetectorHit_sptr_H */
