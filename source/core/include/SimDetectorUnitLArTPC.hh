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

#include "RealDetectorUnitLArTPC.hh"
#include "DeviceSimulation.hh"
#include "VLArRecombinationModel.hh"

class TH3D;

namespace comptonsoft {

/**
 * A class of a LArTPC detector unit including device simulations.
 * @author Shota Arai
 * @date 2025-07-12
 */
class SimDetectorUnitLArTPC
  : public RealDetectorUnitLArTPC, public DeviceSimulation
{
public:
  SimDetectorUnitLArTPC();
  virtual ~SimDetectorUnitLArTPC();

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
  void applyRecombinationModel(DetectorHit_sptr hit); // This should be defined in the DeviceSimulation class. But we define it here to avoid affecting the other classes.
  VLArRecombinationModel* recombinationModel_; ///< Recombination model for LArTPC.
};

inline bool SimDetectorUnitLArTPC::checkRange(const VoxelID& voxel) const
{
  return ( 0<=voxel.X() && voxel.X()<getNumVoxelX()
           && 0<=voxel.Y() && voxel.Y()<getNumVoxelY()
           && 0<=voxel.Z() && voxel.Z()<getNumVoxelZ() );
}

inline int SimDetectorUnitLArTPC::IndexOfTable(const VoxelID&) const
{
  return 0;
}

inline int SimDetectorUnitLArTPC::SizeOfTable() const
{
  return 1;
}

inline VoxelID SimDetectorUnitLArTPC::TableIndexToPixelID(int) const
{
  return VoxelID();
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SimDetectorUnitLArTPC_H */
