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
 * @date 2014-11-21
 */
class SimDetectorUnitScintillator
  : public RealDetectorUnitScintillator, public DeviceSimulation
{
public:
  SimDetectorUnitScintillator();
  ~SimDetectorUnitScintillator();

  std::string Type() { return std::string("SimDetectorScintillator"); }
  void initializeEvent();

  double ChargeCollectionEfficiency(const PixelID& ,
                                    double , double , double ) const
  { return 1.0; }

protected:
  bool checkRange(const PixelID& sp) const;
  int IndexOfTable(const PixelID& sp) const;
  int SizeOfTable() const;
  PixelID TableIndexToPixelID(int index) const;

private:
  void simulatePulseHeights();
};

inline bool SimDetectorUnitScintillator::checkRange(const PixelID& sp) const
{
  return ( sp.X()==0 && sp.Y()==0 );
}

inline int SimDetectorUnitScintillator::IndexOfTable(const PixelID& ) const
{
  return 0;
}

inline int SimDetectorUnitScintillator::SizeOfTable() const
{
  return 1;
}

inline PixelID SimDetectorUnitScintillator::TableIndexToPixelID(int) const
{
  return PixelID(0, 0);
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SimDetectorUnitScintillator_H */
