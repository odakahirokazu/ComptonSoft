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

#ifndef COMPTONSOFT_SimDetectorUnit3DVoxel_H
#define COMPTONSOFT_SimDetectorUnit3DVoxel_H 1

#include "RealDetectorUnit3DVoxel.hh"
#include "DeviceSimulation.hh"

class TH3D;

namespace comptonsoft {

/**
 * A class of a 3D-voxel detector unit including device simulations.
 * @author Hirokazu Odaka
 * @date 2022-04-27
 */
class SimDetectorUnit3DVoxel
  : public RealDetectorUnit3DVoxel, public DeviceSimulation
{
public:
  SimDetectorUnit3DVoxel();
  virtual ~SimDetectorUnit3DVoxel();

  void initializeEvent() override;

  void printSimulationParameters(std::ostream& os) const override;

protected:
  virtual DetectorHit_sptr generateHit(const DetectorHit& rawhit,
                                       const VoxelID& voxel);

  bool checkRange(const VoxelID& voxel) const override;
  int IndexOfTable(const VoxelID& voxel) const override;
  int SizeOfTable() const override;
  VoxelID TableIndexToPixelID(int index) const override;

private:
  void simulatePulseHeights() override;
};

inline bool SimDetectorUnit3DVoxel::checkRange(const VoxelID& voxel) const
{
  return ( 0<=voxel.X() && voxel.X()<getNumVoxelX()
           && 0<=voxel.Y() && voxel.Y()<getNumVoxelY()
           && 0<=voxel.Z() && voxel.Z()<getNumVoxelZ() );
}

inline int SimDetectorUnit3DVoxel::IndexOfTable(const VoxelID&) const
{
  return 0;
}

inline int SimDetectorUnit3DVoxel::SizeOfTable() const
{
  return 1;
}

inline VoxelID SimDetectorUnit3DVoxel::TableIndexToPixelID(int) const
{
  return VoxelID();
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SimDetectorUnit3DVoxel_H */
