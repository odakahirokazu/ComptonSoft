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

#ifndef COMPTONSOFT_SimDetectorUnit2DStrip_H
#define COMPTONSOFT_SimDetectorUnit2DStrip_H 1

#include "TH2.h"

#include "RealDetectorUnit2DStrip.hh"
#include "DeviceSimulation.hh"

namespace comptonsoft {

/**
 * A class of a 2D-strip semicondutor unit including device simulations.
 * @author Hirokazu Odaka
 * @date 2008-08-25
 * @date 2009-05-20
 * @date 2009-10-26
 */

class SimDetectorUnit2DStrip
  : public RealDetectorUnit2DStrip, public DeviceSimulation
{

public:
  SimDetectorUnit2DStrip();
  virtual ~SimDetectorUnit2DStrip();

  virtual std::string Type() { return std::string("SimDetector2DStrip"); }
  
  void setUpsideXStrip(bool val=true) { m_UpsideXStrip = val; }
  void setUpsideYStrip(bool val=true) { m_UpsideXStrip = !val; }
  bool UpsideXStrip() { return m_UpsideXStrip; }
  bool UpsideYStrip() { return !m_UpsideXStrip; }

  void setThresholdEnergyCathode(double val);
  void setThresholdEnergyAnode(double val);
  double ThresholdEnergyCathode();
  double ThresholdEnergyAnode();

  void setBadChannelXStripSideAll(int val);
  void setBadChannelYStripSideAll(int val);

  void setNoiseFWHMXStripSideAll(double param0, double param1, double param2);
  void setNoiseFWHMYStripSideAll(double param0, double param1, double param2);

  void setGainCorrectionFunctionXStripSideAll(const TSpline* val);
  void setGainCorrectionFunctionYStripSideAll(const TSpline* val);

  void setGainCorrectionFactorXStripSideAll(double val);
  void setGainCorrectionFactorYStripSideAll(double val);

  void setUseSymmetry(bool val) { m_UseSymmetry = val; }
  bool UseSymmetry() { return m_UseSymmetry; }
  
  void setCCEMapXStrip(TH2D* h3) { m_CCEMapXStrip = h3; }
  TH2D* getCCEMapXStrip() { return m_CCEMapXStrip; }
  void setCCEMapYStrip(TH2D* h3) { m_CCEMapYStrip = h3; }
  TH2D* getCCEMapYStrip() { return m_CCEMapYStrip; }

  void setWPMapXStrip(TH2D* h3) { m_WPMapXStrip = h3; }
  TH2D* getWPMapXStrip() { return m_WPMapXStrip; }
  void setWPMapYStrip(TH2D* h3) { m_WPMapYStrip = h3; }
  TH2D* getWPMapYStrip() { return m_WPMapYStrip; }

  virtual bool isCCEMapPrepared();
 
  virtual double ChargeCollectionEfficiency(const StripPair& sp,
                                            double x, double y, double z);
  double WeightingPotential(const StripPair& sp, double x, double y, double z);
  virtual void buildWPMap();
  virtual void buildWPMap(int nx, int ny, int nz, double pixel_factor=1.0);
  virtual void buildCCEMap();
  virtual void buildCCEMap(int nx, int ny, int nz, double pixel_factor=1.0);

  virtual void printSimParam();
  
protected:
  virtual void simulatePulseHeights();
 
  DetectorHit_sptr generateHit(const DetectorHit& rawhit,
                               const StripPair& sp);

  bool RangeCheck(const StripPair& sp);
  int IndexOfTable(const StripPair& sp);
  int SizeOfTable();

private:
  bool m_UpsideXStrip;
  bool m_UseSymmetry;
  TH2D* m_CCEMapXStrip;
  TH2D* m_CCEMapYStrip;
  TH2D* m_WPMapXStrip;
  TH2D* m_WPMapYStrip;
};


inline bool SimDetectorUnit2DStrip::RangeCheck(const StripPair& sp)
{
  return ( ( sp.Y()==-1 && sp.X()>=0 && sp.X()<getNumPixelX() ) 
	   || ( sp.X()==-1 && sp.Y()>=0 && sp.Y()<getNumPixelY() ) );
}


inline int SimDetectorUnit2DStrip::IndexOfTable(const StripPair& sp)
{
  return (sp.Y()==-1) ? sp.X() : (getNumPixelX()+sp.Y());
}


inline int SimDetectorUnit2DStrip::SizeOfTable()
{
  return getNumPixelX()+getNumPixelY();
}

}

#endif /* COMPTONSOFT_SimDetectorUnit2DStrip_H */
