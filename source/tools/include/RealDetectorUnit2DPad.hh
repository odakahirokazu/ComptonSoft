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

#ifndef COMPTONSOFT_RealDetectorUnit2DPad_H
#define COMPTONSOFT_RealDetectorUnit2DPad_H 1

#include "RealDetectorUnit.hh"

namespace comptonsoft {

/**
 * A class of a 2D-pad semicondutor detector unit.
 * @author Hirokazu Odaka
 * @date 2008-08-25
 */

class RealDetectorUnit2DPad : public RealDetectorUnit
{
public:
  RealDetectorUnit2DPad();
  virtual ~RealDetectorUnit2DPad();

  virtual std::string Type() { return std::string("Detector2DPad"); }

  virtual int analyzeHits();

private:
  void determinePosition();
  virtual void cluster();
};

}

#endif /* COMPTONSOFT_RealDetectorUnit2DPad_H */
