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

#ifndef COMPTONSOFT_SimDetectorUnitLArTPCPixel_H
#define COMPTONSOFT_SimDetectorUnitLArTPCPixel_H 1

#include "LArTPCDeviceSimulation.hh"
#include "RealDetectorUnitLArTPCPixel.hh"
#include "VLArRecombinationModel.hh"

class TH3D;

namespace comptonsoft {

/**
 * A class of a LArTPC detector unit including device simulations.
 * @author Shota Arai
 * @date 2026-02-17
 */
class SimDetectorUnitLArTPCPixel
    : public RealDetectorUnitLArTPCPixel,
      public LArTPCDeviceSimulation {
public:
  SimDetectorUnitLArTPCPixel();
  virtual ~SimDetectorUnitLArTPCPixel();

  void initializeEvent() override;

  double ChargeCollectionEfficiency(const PixelID &pixel,
                                    double x, double y, double z) const override;
  double WeightingPotential(const PixelID &pixel, double x, double y, double z);
  void buildWPMap() override;
  void buildWPMap(int nx, int ny, int nz, double pixel_factor = 1.0) override;
  void buildCCEMap() override;
  void buildCCEMap(int nx, int ny, int nz, double pixel_factor = 1.0) override;
  TH3D *getCCEMap() { return CCEMap_; }
  void setCCEMap(TH3D *h3) { CCEMap_ = h3; }
  TH3D *getWPMap() { return WPMap_; }
  void setWPMap(TH3D *h3) { WPMap_ = h3; }

  bool isCCEMapPrepared() override { return (getCCEMap() != 0); }
  bool isWPMapPrepared() { return (getWPMap() != 0); }
  bool isUsingSymmetry() const { return usingSymmetry_; }

  double DistanceThreshold() const { return distanceThreshold_; }
  void setDistanceThreshold(double val) { distanceThreshold_ = val; }

protected:
  virtual DetectorHit_sptr generateHit(const DetectorHit &rawhit,
                                       const PixelID &pixel);

  void mergeHits(std::list<DetectorHit_sptr> &hits) override;

  bool checkRange(const PixelID &pixel) const override;
  int IndexOfTable(const PixelID &pixel) const override;
  int SizeOfTable() const override;
  PixelID TableIndexToPixelID(int index) const override;
  void checkBranchingGamma();

private:
  void simulatePulseHeights() override;
  bool usingSymmetry_ = true;
  TH3D *CCEMap_;
  TH3D *WPMap_;
  double distanceThreshold_ = 5.0 * CLHEP::mm;
};

inline bool SimDetectorUnitLArTPCPixel::checkRange(const PixelID &pixel) const {
  return (0 <= pixel.X() && pixel.X() < getNumPixelX() && 0 <= pixel.Y() && pixel.Y() < getNumPixelY());
}

inline int SimDetectorUnitLArTPCPixel::IndexOfTable(const PixelID &pixel) const {
  return pixel.X() + getNumPixelX() * pixel.Y();
}

inline int SimDetectorUnitLArTPCPixel::SizeOfTable() const {
  return getNumPixelX() * getNumPixelY();
}

inline PixelID SimDetectorUnitLArTPCPixel::TableIndexToPixelID(int index) const {
  const int nx = getNumPixelX();
  return PixelID(index % nx, index / nx);
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SimDetectorUnitLArTPCPixel_H */
