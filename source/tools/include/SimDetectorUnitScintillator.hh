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

#ifndef COMPTONSOFT_SimDetectorUnitScintillator_H
#define COMPTONSOFT_SimDetectorUnitScintillator_H 1

#include "RealDetectorUnitScintillator.hh"
#include "DeviceSimulation.hh"

namespace comptonsoft {

/**
 * A class of a scintillator unit including device simulations.
 * @author Hirokazu Odaka
 * @date 2008-08-25
 */

class SimDetectorUnitScintillator
  : public RealDetectorUnitScintillator, public DeviceSimulation
{

public:
  SimDetectorUnitScintillator();
  virtual ~SimDetectorUnitScintillator();

  virtual std::string Type() { return std::string("SimDetectorScintillator"); }

  virtual double ChargeCollectionEfficiency(const StripPair& ,
                                            double , double , double )
  { return 1.0; }

protected:
  virtual void simulatePulseHeights();
  
  bool RangeCheck(const StripPair& sp);
  int IndexOfTable(const StripPair& sp);
  int SizeOfTable();
};


inline bool SimDetectorUnitScintillator::RangeCheck(const StripPair& sp)
{
  return ( sp.X()==0 && sp.Y()==0 );
}


inline int SimDetectorUnitScintillator::IndexOfTable(const StripPair& )
{
  return 0;
}


inline int SimDetectorUnitScintillator::SizeOfTable()
{
  return 1;
}

}

#endif /* COMPTONSOFT_SimDetectorUnitScintillator_H */
