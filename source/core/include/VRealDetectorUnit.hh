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
#include <iostream>
#include <cstdint>
#include "DetectorHit_sptr.hh"

namespace comptonsoft {

class VChannelMap;
class MultiChannelData;
class FrameData;

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

  void setName(const std::string& name) override
  { name_ = name; };
  std::string getName() const override
  { return name_; }
  std::string getNamePrefix() const override;
  
  void setID(int i) override
  { ID_ = i; }
  int getID() const override
  { return ID_; }
  
  void setInstrumentID(int i) override
  { instrumentID_ = i; };
  int getInstrumentID() const override
  { return instrumentID_; };

  void setSize(double x, double y, double z) override
  { sizeX_ = x; sizeY_ = y; sizeZ_ = z; }
  double getSizeX() const override { return sizeX_; }
  double getSizeY() const override { return sizeY_; }
  double getSizeZ() const override { return sizeZ_; }
  
  void setWidth(double x, double y) override
  { sizeX_ = x; sizeY_ = y; }
  double getWidthX() const override { return sizeX_; }
  double getWidthY() const override { return sizeY_; }

  void setThickness(double r) override { sizeZ_ = r; }
  double getThickness() const override { return sizeZ_; }
  
  void setOffset(double x, double y) override
  { offsetX_ = x; offsetY_ = y; }
  double getOffsetX() const override { return offsetX_; }
  double getOffsetY() const override { return offsetY_; }
  void setPixelPitch(double x, double y) override
  { pixelPitchX_ = x; pixelPitchY_ = y; }
  double getPixelPitchX() const override { return pixelPitchX_; }
  double getPixelPitchY() const override { return pixelPitchY_; }
  
  void setNumPixel(int x, int y) override
  { numPixelX_ = x; numPixelY_ = y; }
  double getNumPixelX() const override { return numPixelX_; }
  double getNumPixelY() const override { return numPixelY_; }
  
  void setCenterPosition(double x, double y, double z) override
  { centerPosition_.set(x, y, z); }
  void setCenterPosition(const vector3_t& v) override
  { centerPosition_ = v; }
  vector3_t getCenterPosition() const override
  { return centerPosition_; }
  double getCenterPositionX() const override
  { return centerPosition_.x(); }
  double getCenterPositionY() const override
  { return centerPosition_.y(); }
  double getCenterPositionZ() const override
  { return centerPosition_.z(); }
  
  void setXAxisDirection(double x, double y, double z) override
  {
    directionX_.set(x, y, z);
    directionX_ = directionX_.unit();
  }
  void setXAxisDirection(const vector3_t& v) override
  { directionX_ = v.unit(); }
  vector3_t getXAxisDirection() const override
  { return directionX_; }
  double getXAxisDirectionX() const override
  { return directionX_.x(); }
  double getXAxisDirectionY() const override
  { return directionX_.y(); }
  double getXAxisDirectionZ() const override
  { return directionX_.z(); }
  
  void setYAxisDirection(double x, double y, double z) override
  {
    directionY_.set(x, y, z);
    directionY_ = directionY_.unit();
  }
  void setYAxisDirection(const vector3_t& v) override
  { directionY_ = v.unit(); }
  vector3_t getYAxisDirection() const override
  { return directionY_; }
  double getYAxisDirectionX() const override
  { return directionY_.x(); }
  double getYAxisDirectionY() const override
  { return directionY_.y(); }
  double getYAxisDirectionZ() const override
  { return directionY_.z(); }

  void setZAxisDirection(double x, double y, double z) override
  {
    directionZ_.set(x, y, z);
    directionZ_ = directionZ_.unit();
  }
  void setZAxisDirection(const vector3_t& v) override
  { directionZ_ = v.unit(); }
  vector3_t getZAxisDirection() const override
  { return directionZ_; }
  double getZAxisDirectionX() const override
  { return directionZ_.x(); }
  double getZAxisDirectionY() const override
  { return directionZ_.y(); }
  double getZAxisDirectionZ() const override
  { return directionZ_.z(); }

  /**
   * methods related to multi channel data.
   */
  void registerMultiChannelData(std::unique_ptr<MultiChannelData>&& mcd);
  int NumberOfMultiChannelData() const { return MCDVector_.size(); }
  const MultiChannelData* getMultiChannelData(int i) const { return MCDVector_[i].get(); }
  MultiChannelData* getMultiChannelData(int i) { return MCDVector_[i].get(); }

  /**
   * set an channel table to this detector object.
   */
  void setChannelMap(const std::shared_ptr<const VChannelMap>& v)
  { channelMap_ = v; }
  std::shared_ptr<const VChannelMap> getChannelMap() const
  { return channelMap_; }
  
  vector3_t Position(int pixelX, int pixelY) const override;
  void Position(int pixelX, int pixelY,
                double* x, double* y, double* z) const override;
  vector3_t Position(const PixelID& pixel) const override;
  void Position(const PixelID& pixel,
                double* x, double* y, double* z) const override;

  vector3_t LocalPosition(int pixelX, int pixelY) const override;
  void LocalPosition(int pixelX, int pixelY,
                     double* x, double* y, double* z) const override;
  vector3_t LocalPosition(const PixelID& pixel) const override;
  void LocalPosition(const PixelID& pixel,
                     double* x, double* y, double* z) const override;

  vector3_t PositionWithDepth(int pixelX, int pixelY, double localz) const override;
  vector3_t PositionWithDepth(const PixelID& pixel, double localz) const override;

  vector3_t LocalPositionWithDepth(int pixelX, int pixelY, double localz) const override;
  vector3_t LocalPositionWithDepth(const PixelID& pixel, double localz) const override;

  vector3_t LocalPositionError() const;
  vector3_t PositionError(const vector3_t& localError) const;
  
  PixelID findPixel(double localx, double localy) const override;

  PixelID ChannelToPixel(int section, int channel) const override;
  PixelID ChannelToPixel(const SectionChannelPair& sectionChannel) const override
  {
    return ChannelToPixel(sectionChannel.Section(), sectionChannel.Channel());
  }
  PixelID ChannelToStripPair(int hit1Section, int hit1Channel,
                             int hit2Section, int hit2Channel) const override;
  PixelID ChannelToStripPair(const SectionChannelPair& hit1Channel,
                             const SectionChannelPair& hit2Channel) const override
  {
    return ChannelToStripPair(hit1Channel.Section(), hit1Channel.Channel(),
                              hit2Channel.Section(), hit2Channel.Channel());
  }

  SectionChannelPair PixelToChanelID(const PixelID& pixel) const override;

  void setBottomSideElectrode(ElectrodeSide v) override
  { bottomSideElectrode_ = v; }
  ElectrodeSide BottomSideElectrode() const override
  { return bottomSideElectrode_; }
  ElectrodeSide UpSideElectrode() const override
  {
    return (isUpSideAnode() ? ElectrodeSide::Anode :
            isUpSideCathode() ? ElectrodeSide::Cathode :
            ElectrodeSide::Undefined);
  }

  bool isBottomSideAnode() const override
  { return BottomSideElectrode()==ElectrodeSide::Anode; }
  bool isBottomSideCathode() const override
  { return BottomSideElectrode()==ElectrodeSide::Cathode; }
  bool isUpSideAnode() const override
  { return isBottomSideCathode(); }
  bool isUpSideCathode() const override
  { return isBottomSideAnode(); }

  bool setReconstructionMode(int mode)
  {
    reconstructionMode_ = mode;
    if (mode==0) { return true; }
    return setReconstructionDetails(mode);
  }
  int ReconstructionMode() const { return reconstructionMode_; }
  void setClusteringOn(bool v=true) { clusteringOn_ = v; }
  bool isClusteringOn() const { return clusteringOn_; }

  void initializeEvent() override;
  virtual void selectHits();
  void discriminateHits();
  void reconstructHits();

  void recalculateEPI();
  void assignReadoutInfo();

  void insertDetectorHit(DetectorHit_sptr hit) override
  { detectorHits_.push_back(hit); }
  int NumberOfDetectorHits() const { return detectorHits_.size(); }
  DetectorHit_sptr getDetectorHit(int i) { return detectorHits_[i]; }
  
  void insertReconstructedHit(DetectorHit_sptr hit)
  { reconstructedHits_.push_back(hit); }
  int NumberOfReconstructedHits() const { return reconstructedHits_.size(); }
  DetectorHit_sptr getReconstructedHit(int i) { return reconstructedHits_[i]; };

  virtual void printDetectorParameters(std::ostream& os) const;

  /**
   * methods related to frame data
   */
  void registerFrameData(std::unique_ptr<FrameData>&& frame);
  bool hasFrameData() const { return (frame_.get()!=nullptr); }
  const FrameData* getFrameData() const { return frame_.get(); }
  FrameData* getFrameData() { return frame_.get(); }

protected:
  virtual bool setReconstructionDetails(int /* mode */) { return true; }
  virtual void reconstruct(const DetectorHitVector& hitSignals,
                           DetectorHitVector& hitsReconstructed) = 0;
  virtual void cluster(DetectorHitVector& hits) const;

private:
  std::string name_;
  int ID_;
  int instrumentID_;
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
  vector3_t directionZ_;
  
  ElectrodeSide bottomSideElectrode_;

  int reconstructionMode_;
  bool clusteringOn_;

  std::vector<std::unique_ptr<MultiChannelData>> MCDVector_;
  std::shared_ptr<const VChannelMap> channelMap_;
  std::vector<DetectorHit_sptr> detectorHits_;
  std::vector<DetectorHit_sptr> reconstructedHits_;

  std::unique_ptr<FrameData> frame_;

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
vector3_t VRealDetectorUnit::PositionWithDepth(const PixelID& pixel, double localz) const
{
  return PositionWithDepth(pixel.X(), pixel.Y(), localz);
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

inline
vector3_t VRealDetectorUnit::LocalPositionWithDepth(const PixelID& pixel, double localz) const
{
  return LocalPositionWithDepth(pixel.X(), pixel.Y(), localz);
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VRealetectorUnit_H */
