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

#ifndef COMPTONSOFT_SimDetectorUnitNanoGRAMS_hh
#define COMPTONSOFT_SimDetectorUnitNanoGRAMS_hh 1
#include <limits>
#include <list>
#include "LArTPCDeviceSimulation.hh"
#include "RealDetectorUnitNanoGRAMS.hh"
#include "SimDetectorUnitLArTPCPixel.hh"
namespace comptonsoft {
class SimDetectorUnitNanoGRAMS : public RealDetectorUnitNanoGRAMS, public LArTPCDeviceSimulation
{
public:
  SimDetectorUnitNanoGRAMS();
  virtual ~SimDetectorUnitNanoGRAMS();

  void prepareForTimingProcess() override;
  void makeDetectorHitsAtTime(double time_triggered, int time_group) override;

private:
  void initializeEvent() override;

  double ChargeCollectionEfficiency(const PixelID& pixel, double x, double y, double z) const override;
  double WeightingPotential(const PixelID& pixel, double x, double y, double z);
  void buildWPMap() override;
  void buildWPMap(int nx, int ny, int nz, double pixel_factor = 1.0) override;
  void buildCCEMap() override;
  void buildCCEMap(int nx, int ny, int nz, double pixel_factor = 1.0) override;
  TH3D* getCCEMap() { return CCEMap_; }
  void setCCEMap(TH3D* h3) { CCEMap_ = h3; }
  TH3D* getWPMap() { return WPMap_; }
  void setWPMap(TH3D* h3) { WPMap_ = h3; }

  bool isCCEMapPrepared() override { return (getCCEMap() != 0); }
  bool isWPMapPrepared() { return (getWPMap() != 0); }
  bool isUsingSymmetry() const { return usingSymmetry_; }

protected:
  virtual DetectorHit_sptr generateHit(const DetectorHit& rawhit, const PixelID& pixel);
  void reconstruct(const DetectorHitVector& hitSignals, DetectorHitVector& hitsReconstructed) override;

  void mergeHits(std::list<DetectorHit_sptr>& hits) override;

  bool checkRange(const PixelID& pixel) const override;
  int IndexOfTable(const PixelID& pixel) const override;
  int SizeOfTable() const override;
  PixelID TableIndexToPixelID(int index) const override;
  void checkBranchingGamma();

private:
  void simulatePulseHeights() override;
  bool usingSymmetry_ = true;
  TH3D* CCEMap_;
  TH3D* WPMap_;
  double driftTimeFromDepth(double z) const;

  double arrivalTime(const DetectorHit& hit) const;
  int FECOfPixel(const PixelID& pixel) const;

  void mergeHitsIfArrivalCoincident(double time_width, std::list<DetectorHit_sptr>& hits);

  double eventStartTime_ = std::numeric_limits<double>::quiet_NaN();
};

inline bool SimDetectorUnitNanoGRAMS::checkRange(const PixelID& pixel) const
{
  return (0 <= pixel.X() && pixel.X() < getNumPixelX() && 0 <= pixel.Y() && pixel.Y() < getNumPixelY());
}

inline int SimDetectorUnitNanoGRAMS::IndexOfTable(const PixelID& pixel) const
{
  return pixel.X() + getNumPixelX() * pixel.Y();
}

inline int SimDetectorUnitNanoGRAMS::SizeOfTable() const
{
  return getNumPixelX() * getNumPixelY();
}

inline PixelID SimDetectorUnitNanoGRAMS::TableIndexToPixelID(int index) const
{
  const int nx = getNumPixelX();
  return PixelID(index % nx, index / nx);
}

// using SimDetectorUnitNanoGRAMS = SimDetectorUnitNanoGRAMS;
} // namespace comptonsoft
#endif