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

#ifndef COMPTONSOFT_SimDetectorUnit2DPad_H
#define COMPTONSOFT_SimDetectorUnit2DPad_H 1

#include "RealDetectorUnit2DPad.hh"
#include "DeviceSimulation.hh"

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

class SimDetectorUnit2DPad
  : public RealDetectorUnit2DPad, public DeviceSimulation
{
public:
  SimDetectorUnit2DPad();
  virtual ~SimDetectorUnit2DPad();

  virtual std::string Type() { return std::string("SimDetector2DPad"); }

  void setUpsideReadElectrode(bool val) { m_UpsideReadElectrode = val; }
  bool UpsideReadElectrode() { return m_UpsideReadElectrode; }

  void setThresholdEnergyCathode(double val);
  void setThresholdEnergyAnode(double val);
  double ThresholdEnergyCathode();
  double ThresholdEnergyAnode();

  void setUseSymmetry(bool val) { m_UseSymmetry = val; }
  bool UseSymmetry() { return m_UseSymmetry; }

  void setCCEMap(TH3D* h3) { m_CCEMap = h3; }
  TH3D* getCCEMap() { return m_CCEMap; }

  void setWPMap(TH3D* h3) { m_WPMap = h3; }
  TH3D* getWPMap() { return m_WPMap; }
 
  virtual double ChargeCollectionEfficiency(const StripPair& sp,
                                            double x, double y, double z);
  double WeightingPotential(const StripPair& sp, double x, double y, double z);
  virtual void buildWPMap();
  virtual void buildWPMap(int nx, int ny, int nz, double pixel_factor=1.0);
  virtual void buildCCEMap();
  virtual void buildCCEMap(int nx, int ny, int nz, double pixel_factor=1.0);

  virtual bool isCCEMapPrepared();

  virtual void printSimParam();

protected:
  virtual void simulatePulseHeights();
  virtual DetectorHit_sptr generateHit(const DetectorHit& rawhit,
                                       const StripPair& sp);

  virtual bool RangeCheck(const StripPair& sp);
  virtual int IndexOfTable(const StripPair& sp);
  virtual int SizeOfTable();

private:
  bool m_UpsideReadElectrode;
  bool m_UseSymmetry;
  TH3D* m_CCEMap;
  TH3D* m_WPMap;
};


inline bool SimDetectorUnit2DPad::RangeCheck(const StripPair& sp)
{
  return ( 0<=sp.X() && sp.X()<getNumPixelX()
	   && 0<=sp.Y() && sp.Y()<getNumPixelY() );
}


inline int SimDetectorUnit2DPad::IndexOfTable(const StripPair& sp)
{
  return sp.X() + getNumPixelX()*sp.Y();
}


inline int SimDetectorUnit2DPad::SizeOfTable()
{
  return getNumPixelX()*getNumPixelY();
}

}

#endif /* COMPTONSOFT_SimDetectorUnit2DPad_H */
