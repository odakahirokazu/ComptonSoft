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

#include "RealDetectorUnitFactory.hh"
#include "RealDetectorUnit2DPixel.hh"
#include "RealDetectorUnit2DStrip.hh"
#include "RealDetectorUnitScintillator.hh"
#include "MultiChannelData.hh"

namespace comptonsoft {

VRealDetectorUnit* RealDetectorUnitFactory::createDetectorUnit2DPixel()
{
  return new RealDetectorUnit2DPixel;
}

VRealDetectorUnit* RealDetectorUnitFactory::createDetectorUnit2DStrip()
{
  return new RealDetectorUnit2DStrip;
}

VRealDetectorUnit* RealDetectorUnitFactory::createDetectorUnitScintillator()
{
  return new RealDetectorUnitScintillator;
}

} /* namespace comptonsoft */
