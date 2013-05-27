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

#ifndef COMPTONSOFT_SimDetectorUnitLite_H
#define COMPTONSOFT_SimDetectorUnitLite_H 1

#include "RealDetectorUnit2DPad.hh"
#include "BasicDeviceSimulation.hh"

class TH3D;

namespace comptonsoft {

/**
 * A class of a 2D-pad semicondutor unit including device simulations.
 * @author Hirokazu Odaka
 * @date 2008-08-25
 * @date 2009-05-20
 * @date 2009-06-01
 * @date 2009-07-09
 * @date 2009-10-26
 */

class SimDetectorUnitLite
  : public RealDetectorUnit2DPad, public BasicDeviceSimulation
{
public:
  SimDetectorUnitLite();
  virtual ~SimDetectorUnitLite();

  virtual std::string Type() { return std::string("SimDetectorLite"); }

  void setThresholdEnergyCathode(double val);
  void setThresholdEnergyAnode(double val);
  double ThresholdEnergyCathode();
  double ThresholdEnergyAnode();

protected:
  virtual void simulatePulseHeights();
  virtual DetectorHit_sptr generateHit(const DetectorHit& rawhit,
                                       const StripPair& sp);

  virtual bool RangeCheck(const StripPair& sp);
  virtual int IndexOfTable(const StripPair& );
  virtual int SizeOfTable();
};


inline bool SimDetectorUnitLite::RangeCheck(const StripPair& sp)
{
  return ( 0<=sp.X() && sp.X()<getNumPixelX()
	   && 0<=sp.Y() && sp.Y()<getNumPixelY() );
}


inline int SimDetectorUnitLite::IndexOfTable(const StripPair& )
{
  return 0;
}


inline int SimDetectorUnitLite::SizeOfTable()
{
  return 1;
}

}

#endif /* COMPTONSOFT_SimDetectorUnitLite_H */
