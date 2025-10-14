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

#ifndef COMPTONSOFT_SimDetectorUnitLArTPC_H
#define COMPTONSOFT_SimDetectorUnitLArTPC_H 1

#include "LArTPCDeviceSimulation.hh"
#include "RealDetectorUnitLArTPC.hh"
#include "VLArRecombinationModel.hh"

class TH3D;

namespace comptonsoft {

/**
 * A class of a LArTPC detector unit including device simulations.
 * @author Shota Arai
 * @date 2025-07-12
 * @date 2025-10-10 | inherited LArTPCDeviceSimulation & removed recombination caliculation feature
 */
class SimDetectorUnitLArTPC
    : public RealDetectorUnitLArTPC,
      public LArTPCDeviceSimulation {
public:
  SimDetectorUnitLArTPC();
  virtual ~SimDetectorUnitLArTPC();

  void initializeEvent() override;

  double ChargeCollectionEfficiency(const VoxelID &voxel,
                                    double x, double y, double z) const override;
  double WeightingPotential(const VoxelID &voxel, double x, double y, double z);
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

protected:
  virtual DetectorHit_sptr generateHit(const DetectorHit &rawhit,
                                       const VoxelID &voxel);

  bool checkRange(const VoxelID &voxel) const override;
  int IndexOfTable(const VoxelID &voxel) const override;
  int SizeOfTable() const override;
  VoxelID TableIndexToPixelID(int index) const override;

private:
  void simulatePulseHeights() override;
  bool usingSymmetry_ = true;
  TH3D *CCEMap_;
  TH3D *WPMap_;
};

inline bool SimDetectorUnitLArTPC::checkRange(const VoxelID &voxel) const {
  return (0 <= voxel.X() && voxel.X() < getNumVoxelX() && 0 <= voxel.Y() && voxel.Y() < getNumVoxelY() && 0 <= voxel.Z() && voxel.Z() < getNumVoxelZ());
}

inline int SimDetectorUnitLArTPC::IndexOfTable(const VoxelID &) const {
  return 0;
}

inline int SimDetectorUnitLArTPC::SizeOfTable() const {
  return 1;
}

inline VoxelID SimDetectorUnitLArTPC::TableIndexToPixelID(int) const {
  return VoxelID();
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SimDetectorUnitLArTPC_H */
