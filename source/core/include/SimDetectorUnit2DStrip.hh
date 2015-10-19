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
  ~SimDetectorUnit2DStrip();

  void initializeEvent();
  
  void setUsingSymmetry(bool val) { usingSymmetry_ = val; }
  bool isUsingSymmetry() const { return usingSymmetry_; }
  
  void setCCEMapXStrip(TH2D* h3) { CCEMapXStrip_ = h3; }
  TH2D* getCCEMapXStrip() { return CCEMapXStrip_; }
  void setCCEMapYStrip(TH2D* h3) { CCEMapYStrip_ = h3; }
  TH2D* getCCEMapYStrip() { return CCEMapYStrip_; }

  void setWPMapXStrip(TH2D* h3) { WPMapXStrip_ = h3; }
  TH2D* getWPMapXStrip() { return WPMapXStrip_; }
  void setWPMapYStrip(TH2D* h3) { WPMapYStrip_ = h3; }
  TH2D* getWPMapYStrip() { return WPMapYStrip_; }

  bool isCCEMapPrepared();
 
  double ChargeCollectionEfficiency(const PixelID& sp,
                                    double x, double y, double z) const;
  double WeightingPotential(const PixelID& sp, double x, double y, double z);
  void buildWPMap();
  void buildWPMap(int nx, int ny, int nz, double pixel_factor=1.0);
  void buildCCEMap();
  void buildCCEMap(int nx, int ny, int nz, double pixel_factor=1.0);

  void printSimulationParameters();
  
protected:
  virtual DetectorHit_sptr generateHit(const DetectorHit& rawhit,
                                       const PixelID& sp);

  bool checkRange(const PixelID& sp) const;
  int IndexOfTable(const PixelID& sp) const;
  int SizeOfTable() const;
  PixelID TableIndexToPixelID(int index) const;

private:
  void simulatePulseHeights();
  
private:
  bool usingSymmetry_;
  TH2D* CCEMapXStrip_;
  TH2D* CCEMapYStrip_;
  TH2D* WPMapXStrip_;
  TH2D* WPMapYStrip_;
};

inline bool SimDetectorUnit2DStrip::checkRange(const PixelID& sp) const
{
  return ( ( sp.Y()==-1 && sp.X()>=0 && sp.X()<getNumPixelX() ) 
	   || ( sp.X()==-1 && sp.Y()>=0 && sp.Y()<getNumPixelY() ) );
}

inline int SimDetectorUnit2DStrip::IndexOfTable(const PixelID& sp) const
{
  return (sp.Y()<0) ? sp.X() : (getNumPixelX()+sp.Y());
}

inline int SimDetectorUnit2DStrip::SizeOfTable() const
{
  return getNumPixelX()+getNumPixelY();
}

inline PixelID SimDetectorUnit2DStrip::TableIndexToPixelID(int index) const
{
  const int nx = getNumPixelX();
  if (index < nx) {
    return PixelID(index, PixelID::Undefined);
  }
  else {
    return PixelID(PixelID::Undefined, index-nx);
  }
  return PixelID(PixelID::Undefined, PixelID::Undefined);
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SimDetectorUnit2DStrip_H */
