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

#ifndef COMPTONSOFT_CSTypes_H
#define COMPTONSOFT_CSTypes_H 1

#include <tuple>
#include <CLHEP/Vector/TwoVector.h>
#include <CLHEP/Vector/ThreeVector.h>
#include <CLHEP/Vector/LorentzVector.h>

namespace comptonsoft {

typedef CLHEP::Hep2Vector vector2_t;
typedef CLHEP::Hep3Vector vector3_t;
typedef CLHEP::HepLorentzVector vector4_t;

enum class ElectrodeSide { Undefined, Anode, Cathode };

enum class DetectorType : int { PixelDetector=1, DoubleSidedStripDetector=2, Scintillator=3 };

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CSTypes_H */
