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

#ifndef COMPTONSOFT_VoxelID_H
#define COMPTONSOFT_VoxelID_H 1

namespace comptonsoft {

/**
 * Class of a voxel, a pixel or a pair of strips.
 *
 * @author Hirokazu Odaka
 * @date 2014-11-11 | as PixelID
 * @date 2022-04-24 | change to Voxel ID
 */
class VoxelID
{
public:
  constexpr static int Undefined = -1;
  constexpr static int OnMergin = -2;
  constexpr static int OnGuardRing = OnMergin;
  
public:
  VoxelID(int x=Undefined, int y=Undefined, int z=Undefined);
  ~VoxelID();
  VoxelID(const VoxelID&) = default;
  VoxelID(VoxelID&&) = default;
  VoxelID& operator=(const VoxelID&) = default;
  VoxelID& operator=(VoxelID&&) = default;
  
  int X() const { return x_; }
  int Y() const { return y_; }
  int Z() const { return z_; }
  void set(int x, int y, int z) { x_=x; y_=y; z_=z; }
  void set(int x, int y) { x_=x; y_=y; z_=Undefined; }
  void setX(int v) { x_ = v; }
  void setY(int v) { y_ = v; }
  void setZ(int v) { z_ = v; }

  VoxelID combine(const VoxelID& pixel)
  {
    int new_x = (x_>=0) ? x_ : pixel.X();
    int new_y = (y_>=0) ? y_ : pixel.Y();
    int new_z = (z_>=0) ? z_ : pixel.Z();
    return VoxelID(new_x, new_y, new_z);
  }
  
  bool operator==(const VoxelID& r) const
  {
    return (x_==r.x_ && y_==r.y_ && z_==r.z_);
  }

  bool operator!=(const VoxelID& r) const
  {
    return !(x_==r.x_ && y_==r.y_ && z_==r.z_);
  }

  bool isVoxel() const
  {
    return x_>=0 && y_>=0 && z_>=0;
  }

  bool isPixel() const
  {
    return x_>=0 && y_>=0 && z_==VoxelID::Undefined;
  }

  bool isStrip() const
  {
    return isXStrip() || isYStrip();
  }

  bool isXStrip() const
  {
    return x_>=0 && y_==VoxelID::Undefined && z_==VoxelID::Undefined;
  }

  bool isYStrip() const
  {
    return y_>=0 && x_==VoxelID::Undefined && z_==VoxelID::Undefined;
  }

private:
  int x_ = Undefined;
  int y_ = Undefined;
  int z_ = Undefined;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VoxelID_H */
