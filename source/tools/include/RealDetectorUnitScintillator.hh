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

#ifndef COMPTONSOFT_RealDetectorUnitScintillator_H
#define COMPTONSOFT_RealDetectorUnitScintillator_H 1

#include "RealDetectorUnit.hh"

namespace comptonsoft {

/**
 * A class of a scintillator unit.
 * @author Hirokazu Odaka
 * @date 2008-09-17
 */

class RealDetectorUnitScintillator : public RealDetectorUnit
{
public:
  RealDetectorUnitScintillator();
  virtual ~RealDetectorUnitScintillator();

  virtual std::string Type() { return std::string("DetectorScintillator"); }

  virtual int analyzeHits();

  void setSize(double x, double y, double z)
  {
    setWidth(x, y);
    setThickness(z);
    setPixelPitch(x, y);
  }

  double getSizeX() { return getWidthX(); }
  double getSizeY() { return getWidthY(); }
  double getSizeZ() { return getThickness(); }

private:
  void determinePosition();
};

}

#endif /* COMPTONSOFT_RealDetectorUnitScintillator_H */
