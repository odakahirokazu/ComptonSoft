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

#ifndef COMPTONSOFT_VRealetectorUnit_H
#define COMPTONSOFT_VRealetectorUnit_H 1

#include "VDetectorUnit.hh"
#include <vector>
#include <memory>
#include "DetectorHit_sptr.hh"

namespace comptonsoft {

class VChannelMap;
class MultiChannelData;

/**
 * A virtual class of a detector unit.
 * This class contains the inforamtion about readout data and hits detected by this detector.
 * @author Hirokazu Odaka
 * @date 2007-12-xx
 * @date 2008-07-29
 * @date 2008-08-25
 * @date 2009-07-09
 * @date 2014-10-02 | VDetectorUnit as a virtual base class.
 * @date 2015-10-11 | DetectorType as an enum class. AnalysisMode => ReconstructionMode
 */
class VRealDetectorUnit : virtual public VDetectorUnit
{
public:
  VRealDetectorUnit();
  virtual ~VRealDetectorUnit();

  virtual DetectorType Type() const = 0;
  bool checkType(DetectorType type) const { return (type==Type()); }
  bool checkType(int type) const { return checkType(static_cast<DetectorType>(type)); }

  void setName(const std::string& name) { name_ = name; };
  std::string getName() const { return name_; }
  std::string getNamePrefix() const;
  
  void setID(int i) { ID_ = i; }
  int getID() const { return ID_; }
  
  void setSize(double x, double y, double z)
  { sizeX_ = x; sizeY_ = y; sizeZ_ = z; }
  double getSizeX() const { return sizeX_; }
  double getSizeY() const { return sizeY_; }
  double getSizeZ() const { return sizeZ_; }
  
  void setWidth(double x, double y)
  { sizeX_ = x; sizeY_ = y; }
  double getWidthX() const { return sizeX_; }
  double getWidthY() const { return sizeY_; }

  void setThickness(double r) { sizeZ_ = r; }
  double getThickness() const { return sizeZ_; }
  
  void setOffset(double x, double y)
  { offsetX_ = x; offsetY_ = y; }
  double getOffsetX() const { return offsetX_; }
  double getOffsetY() const { return offsetY_; }  
  void setPixelPitch(double x, double y)
  { pixelPitchX_ = x; pixelPitchY_ = y; }
  double getPixelPitchX() const { return pixelPitchX_; }
  double getPixelPitchY() const { return pixelPitchY_; }
  
  void setNumPixel(int x, int y)
  { numPixelX_ = x; numPixelY_ = y; }
  double getNumPixelX() const { return numPixelX_; }
  double getNumPixelY() const { return numPixelY_; }
  
  void setCenterPosition(double x, double y, double z)
  { centerPosition_.set(x, y, z); }
  void setCenterPosition(const vector3_t& v) { centerPosition_ = v; }
  vector3_t getCenterPosition() const { return centerPosition_; }
  double getCenterPositionX() const { return centerPosition_.x(); }
  double getCenterPositionY() const { return centerPosition_.y(); }
  double getCenterPositionZ() const { return centerPosition_.z(); }
  
  void setDirectionX(double x, double y, double z) 
  {
    directionX_.set(x, y, z);
    directionX_ = directionX_.unit();
  }
  void setDirectionX(const vector3_t& v) { directionX_ = v.unit(); } 
  vector3_t getDirectionX() const { return directionX_; }
  double getDirectionXX() const { return directionX_.x(); }
  double getDirectionXY() const { return directionX_.y(); }
  double getDirectionXZ() const { return directionX_.z(); }
  
  void setDirectionY(double x, double y, double z) 
  { 
    directionY_.set(x, y, z);
    directionY_ = directionY_.unit();
  } 
  void setDirectionY(const vector3_t& v) { directionY_ = v.unit(); } 
  vector3_t getDirectionY() const { return directionY_; }
  double getDirectionYX() const { return directionY_.x(); }
  double getDirectionYY() const { return directionY_.y(); }
  double getDirectionYZ() const { return directionY_.z(); }

  /**
   * register multi channel data.
   */
  void registerMultiChannelData(MultiChannelData* data);

  /**
   * set an channel table to this detector object.
   */
  void setChannelMap(const std::shared_ptr<const VChannelMap>& v)
  { channelMap_ = v; }
  
  vector3_t Position(int pixelX, int pixelY) const;
  void Position(int pixelX, int pixelY,
                double* x, double* y, double* z) const;
  vector3_t Position(const PixelID& pixel) const;
  void Position(const PixelID& pixel,
                double* x, double* y, double* z) const;

  vector3_t LocalPosition(int pixelX, int pixelY) const;
  void LocalPosition(int pixelX, int pixelY,
                     double* x, double* y, double* z) const;
  vector3_t LocalPosition(const PixelID& pixel) const;
  void LocalPosition(const PixelID& pixel,
                     double* x, double* y, double* z) const;
  
  PixelID findPixel(double localx, double localy) const;

  int NumberOfMultiChannelData() const { return MCDVector_.size(); }
  MultiChannelData* getMultiChannelData(int i) { return MCDVector_[i].get(); }
  
  PixelID ChannelToPixel(int section, int channel) const;
  PixelID ChannelToPixel(const ChannelID& channel) const
  {
    return ChannelToPixel(channel.Section(), channel.Index());
  }
  PixelID ChannelToStripPair(int hit1Group, int hit1Channel,
                             int hit2Group, int hit2Channel) const;
  PixelID ChannelToStripPair(const ChannelID& hit1Channel,
                             const ChannelID& hit2Channel) const
  {
    return ChannelToStripPair(hit1Channel.Section(), hit1Channel.Index(),
                              hit2Channel.Section(), hit2Channel.Index());
  }

  ChannelID PixelToChanelID(const PixelID& pixel) const;

  void setBottomSideElectrode(ElectrodeSide v)
  { bottomSideElectrode_ = v; }
  ElectrodeSide BottomSideElectrode() const
  { return bottomSideElectrode_; }
  ElectrodeSide UpSideElectrode() const
  {
    return (isUpSideAnode() ? ElectrodeSide::Anode :
            isUpSideCathode() ? ElectrodeSide::Cathode :
            ElectrodeSide::Undefined);
  }

  bool isBottomSideAnode() const
  { return BottomSideElectrode()==ElectrodeSide::Anode; }
  bool isBottomSideCathode() const
  { return BottomSideElectrode()==ElectrodeSide::Cathode; }
  bool isUpSideAnode() const
  { return isBottomSideCathode(); }
  bool isUpSideCathode() const
  { return isBottomSideAnode(); }

  bool setReconstructionMode(int mode)
  {
    reconstructionMode_ = mode;
    return setReconstructionDetails(mode);
  }
  int ReconstructionMode() const { return reconstructionMode_; }
  void setClusteringOn(bool v=true) { clusteringOn_ = v; }
  bool isClusteringOn() const { return clusteringOn_; }

  virtual void initializeEvent();
  virtual void selectHits();
  void discriminateHits();
  void reconstructHits();

  void recalculateEPI();
  void assignReadoutInfo();

  void insertDetectorHit(DetectorHit_sptr hit)
  { detectorHits_.push_back(hit); }
  int NumberOfDetectorHits() const { return detectorHits_.size(); }
  DetectorHit_sptr getDetectorHit(int i) { return detectorHits_[i]; }
  
  void insertReconstructedHit(DetectorHit_sptr hit)
  { reconstructedHits_.push_back(hit); }
  int NumberOfReconstructedHits() const { return reconstructedHits_.size(); }
  DetectorHit_sptr getReconstructedHit(int i) { return reconstructedHits_[i]; };

protected:
  virtual bool setReconstructionDetails(int /* mode */) { return true; }
  virtual void reconstruct(const DetectorHitVector& hitSignals,
                           DetectorHitVector& hitsReconstructed) = 0;
  virtual void cluster(DetectorHitVector& hits) const;

private:
  std::string name_;
  int ID_;
  double sizeX_;
  double sizeY_;
  double sizeZ_;
  double offsetX_;
  double offsetY_;
  double pixelPitchX_;
  double pixelPitchY_;
  int numPixelX_;
  int numPixelY_;
  vector3_t centerPosition_;
  vector3_t directionX_;
  vector3_t directionY_;
  
  ElectrodeSide bottomSideElectrode_;

  int reconstructionMode_;
  bool clusteringOn_;

  std::vector<std::unique_ptr<MultiChannelData>> MCDVector_;
  std::shared_ptr<const VChannelMap> channelMap_;
  std::vector<DetectorHit_sptr> detectorHits_;
  std::vector<DetectorHit_sptr> reconstructedHits_;

private:
  VRealDetectorUnit(const VRealDetectorUnit&) = delete;
  VRealDetectorUnit(VRealDetectorUnit&&) = delete;
  VRealDetectorUnit& operator=(const VRealDetectorUnit&) = delete;
  VRealDetectorUnit& operator=(VRealDetectorUnit&&) = delete;
};

inline void VRealDetectorUnit::Position(int pixelX, int pixelY,
                                        double* x, double* y, double* z) const
{
  vector3_t pos = Position(pixelX, pixelY);
  *x = pos.x();
  *y = pos.y();
  *z = pos.z();
}

inline
vector3_t VRealDetectorUnit::Position(const PixelID& pixel) const
{
  return Position(pixel.X(), pixel.Y());
}

inline
void VRealDetectorUnit::Position(const PixelID& pixel,
                                 double* x, double* y, double* z) const
{
  Position(pixel.X(), pixel.Y(), x, y, z);
}

inline
void VRealDetectorUnit::LocalPosition(int pixelX, int pixelY,
                                      double* x, double* y, double* z) const
{
  vector3_t pos = LocalPosition(pixelX, pixelY);
  *x = pos.x();
  *y = pos.y();
  *z = pos.z();
}

inline
vector3_t VRealDetectorUnit::LocalPosition(const PixelID& pixel) const
{
  return LocalPosition(pixel.X(), pixel.Y());
}

inline
void VRealDetectorUnit::LocalPosition(const PixelID& pixel,
                                      double* x, double* y, double* z) const
{
  LocalPosition(pixel.X(), pixel.Y(), x, y, z);
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VRealetectorUnit_H */
