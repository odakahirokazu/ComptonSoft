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

#ifndef COMPTONSOFT_SimDetectorUnit2DPixel_H
#define COMPTONSOFT_SimDetectorUnit2DPixel_H 1

#include "RealDetectorUnit2DPixel.hh"
#include "DeviceSimulation.hh"

class TH3D;

namespace comptonsoft {

/**
 * A class of a 2D-pad semicondutor unit including device simulations.
 * @author Hirokazu Odaka
 * @date 2008-08-25
 * @date 2009-10-26
 * @date 2014-11-20
 */
class SimDetectorUnit2DPixel
  : public RealDetectorUnit2DPixel, public DeviceSimulation
{
public:
  SimDetectorUnit2DPixel();
  virtual ~SimDetectorUnit2DPixel();

  void initializeEvent() override;

  void setUsingSymmetry(bool val) { usingSymmetry_ = val; }
  bool isUsingSymmetry() const { return usingSymmetry_; }

  void setCCEMap(TH3D* h3) { CCEMap_ = h3; }
  TH3D* getCCEMap() { return CCEMap_; }

  void setWPMap(TH3D* h3) { WPMap_ = h3; }
  TH3D* getWPMap() { return WPMap_; }
 
  double ChargeCollectionEfficiency(const PixelID& pixel,
                                    double x, double y, double z) const override;
  double WeightingPotential(const PixelID& pixel, double x, double y, double z);
  void buildWPMap() override;
  void buildWPMap(int nx, int ny, int nz, double pixel_factor=1.0) override;
  void buildCCEMap() override;
  void buildCCEMap(int nx, int ny, int nz, double pixel_factor=1.0) override;

  bool isCCEMapPrepared() override;

  void printSimulationParameters(std::ostream& os) const override;

protected:
  virtual DetectorHit_sptr generateHit(const DetectorHit& rawhit,
                                       const PixelID& pixel);

  bool checkRange(const PixelID& pixel) const override;
  int IndexOfTable(const PixelID& pixel) const override;
  int SizeOfTable() const override;
  PixelID TableIndexToPixelID(int index) const override;

private:
  void simulatePulseHeights() override;

private:
  bool usingSymmetry_;
  TH3D* CCEMap_;
  TH3D* WPMap_;
};

inline bool SimDetectorUnit2DPixel::checkRange(const PixelID& pixel) const
{
  return ( 0<=pixel.X() && pixel.X()<getNumPixelX()
	   && 0<=pixel.Y() && pixel.Y()<getNumPixelY() );
}

inline int SimDetectorUnit2DPixel::IndexOfTable(const PixelID& pixel) const
{
  return pixel.X() + getNumPixelX()*pixel.Y();
}

inline int SimDetectorUnit2DPixel::SizeOfTable() const
{
  return getNumPixelX()*getNumPixelY();
}

inline PixelID SimDetectorUnit2DPixel::TableIndexToPixelID(int index) const
{
  const int nx = getNumPixelX();
  return PixelID(index%nx, index/nx);
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SimDetectorUnit2DPixel_H */
