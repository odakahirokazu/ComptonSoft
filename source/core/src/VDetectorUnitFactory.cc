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

#include "VDetectorUnitFactory.hh"

namespace comptonsoft {

VDetectorUnitFactory::~VDetectorUnitFactory() = default;

VRealDetectorUnit* VDetectorUnitFactory::createDetectorUnit(const std::string& type)
{
  VRealDetectorUnit* detector = nullptr;
  if (type=="Detector2DPad") {
    detector = createDetectorUnit2DPixel();
  }
  else if (type=="Detector2DStrip") {
    detector = createDetectorUnit2DStrip();
  }
  else if (type=="DetectorScintillator") {
    detector = createDetectorUnitScintillator();
  }

  return detector;
}

} /* namespace comptonsoft */
