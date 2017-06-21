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

#ifndef COMPTONSOFT_ObservedParticle_H
#define COMPTONSOFT_ObservedParticle_H 1

#include "G4ThreeVector.hh"

namespace comptonsoft {

struct ObservedParticle
{
  int particle = 0;
  int trackid = 0;
  double time = 0.0;
  G4ThreeVector position = {0.0, 0.0, 0.0};
  double energy = 0.0;
  G4ThreeVector direction = {0.0, 0.0, 0.0};
  G4ThreeVector polarization = {0.0, 0.0, 0.0};
};

using ObservedParticle_sptr = std::shared_ptr<ObservedParticle>;

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ObservedParticle_H */
