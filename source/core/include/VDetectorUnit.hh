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

#ifndef COMPTONSOFT_VDetectorUnit_H
#define COMPTONSOFT_VDetectorUnit_H 1

#include <vector>
#include <string>
#include "CSTypes.hh"
#include "PixelID.hh"
#include "ChannelID.hh"
#include "DetectorHit_sptr.hh"

namespace comptonsoft {

/**
 * A virtual class of a detector unit to access inforamtion on the detector
 * geometry and readout configuration.
 *
 * @author Hirokazu Odaka
 * @date 2008-08-22
 * @date 2014-10-04 | all methods are pure virtual.
 */
class VDetectorUnit
{
public:
  VDetectorUnit();
  virtual ~VDetectorUnit();

public:
  virtual void setName(const std::string& name) = 0;
  virtual std::string getName() const = 0;
  virtual std::string getNamePrefix() const = 0;
  
  virtual void setID(int i) = 0;
  virtual int getID() const = 0;
  
  virtual void setSize(double x, double y, double z) = 0;
  virtual double getSizeX() const = 0;
  virtual double getSizeY() const = 0;
  virtual double getSizeZ() const = 0;
  
  virtual void setWidth(double x, double y) = 0;
  virtual double getWidthX() const = 0;
  virtual double getWidthY() const = 0;

  virtual void setThickness(double r) = 0;
  virtual double getThickness() const = 0;
  
  virtual void setOffset(double x, double y) = 0;
  virtual double getOffsetX() const = 0;
  virtual double getOffsetY() const = 0;
  
  virtual void setPixelPitch(double x, double y) = 0;
  virtual double getPixelPitchX() const = 0;
  virtual double getPixelPitchY() const = 0;
  
  virtual void setNumPixel(int x, int y) = 0;
  virtual double getNumPixelX() const = 0;
  virtual double getNumPixelY() const = 0;
  
  virtual void setCenterPosition(double x, double y, double z) = 0;
  virtual void setCenterPosition(const vector3_t& v) = 0;
  virtual vector3_t getCenterPosition() const = 0;
  virtual double getCenterPositionX() const = 0;
  virtual double getCenterPositionY() const = 0;
  virtual double getCenterPositionZ() const = 0;
  
  virtual void setDirectionX(double x, double y, double z) = 0;
  virtual void setDirectionX(const vector3_t& v) = 0;
  virtual vector3_t getDirectionX() const = 0;
  virtual double getDirectionXX() const = 0;
  virtual double getDirectionXY() const = 0;
  virtual double getDirectionXZ() const = 0;
  
  virtual void setDirectionY(double x, double y, double z) = 0;
  virtual void setDirectionY(const vector3_t& v) = 0;
  virtual vector3_t getDirectionY() const = 0;
  virtual double getDirectionYX() const = 0;
  virtual double getDirectionYY() const = 0;
  virtual double getDirectionYZ() const = 0;
  
  /** 
   * calculate global position of the pixel center.
   * @param pixelX the pixel ID along the x axis of the detector
   * @param pixelY the pixel ID along the y axis of the detector
   * @return global position of the pixel ceneter
   */
  virtual vector3_t Position(int pixelX, int pixelY) const = 0;

  /**
   * calculate global position of the pixel center.
   * @param pixelX the pixel ID along the x axis of the detector
   * @param pixelY the pixel ID along the y axis of the detector
   * @param[out] x x-component of global position of the pixel ceneter
   * @param[out] y y-component of global position of the pixel ceneter
   * @param[out] z z-component of global position of the pixel ceneter
   */
  virtual void Position(int pixelX, int pixelY,
                        double* x, double* y, double* z) const = 0;

  /**
   * calculate global position of the pixel center.
   * @param pixel the pixel ID in @link PixelID @endlink
   * @return global position of the pixel ceneter
   */
  virtual vector3_t Position(const PixelID& pixel) const = 0;

  /**
   * calculate global position of the pixel center.
   * @param pixel the pixel ID in @link PixelID @endlink
   * @param[out] x x-component of global position of the pixel ceneter
   * @param[out] y y-component of global position of the pixel ceneter
   * @param[out] z z-component of global position of the pixel ceneter
   */
  virtual void Position(const PixelID& pixel,
                        double* x, double* y, double* z) const = 0;

  virtual vector3_t LocalPosition(int pixelX, int pixelY) const = 0;
  virtual void LocalPosition(int pixelX, int pixelY,
                             double* x, double* y, double* z) const = 0;
  virtual vector3_t LocalPosition(const PixelID& pixel) const = 0;
  virtual void LocalPosition(const PixelID& pixel,
                             double* x, double* y, double* z) const = 0;
  virtual PixelID findPixel(double localx, double localy) const = 0;

  /**
   * convert a readout channel into a pixel ID.
   * @param section section index of the hit.
   * @param channel channel index of the hit.
   */
  virtual PixelID ChannelToPixel(int section, int channel) const = 0;

  /**
   * convert a readout channel into a pixel ID.
   * @param channel channel ID of the hit.
   */
  virtual PixelID ChannelToPixel(const ChannelID& channel) const = 0;

  /**
   * convert a readout channel into a stirp ID pair.
   * @param hit1Section section index of the first hit of double-sided detector.
   * @param hit1Channel channel index of the first hit of double-sided detector.
   * @param hit2Section section index of the second hit of double-sided detector.
   * @param hit2Channel channel index of the second hit of double-sided detector.
   * @return pixel ID.
   */
  virtual PixelID ChannelToStripPair(int hit1Section, int hit1Channel,
                                     int hit2Section, int hit2Channel) const = 0;
  virtual PixelID ChannelToStripPair(const ChannelID& hit1Channel,
                                     const ChannelID& hit2Channel) const = 0;
  
  /**
   * convert a pixel ID into readout channel ID.
   * @param pixel pixel ID.
   * @return detector channel.
   * @return pixel ID.
   */
  virtual ChannelID PixelToChanelID(const PixelID& pixel) const = 0;

  virtual void setBottomSideElectrode(ElectrodeSide v) = 0;
  virtual ElectrodeSide BottomSideElectrode() const = 0;

  virtual void initializeEvent() = 0;
  virtual void insertDetectorHit(DetectorHit_sptr hit) = 0;

  virtual bool isBottomSideAnode() const = 0;
  virtual bool isBottomSideCathode() const = 0;
  virtual bool isUpSideAnode() const = 0;
  virtual bool isUpSideCathode() const = 0;

private:
  VDetectorUnit(const VDetectorUnit&) = delete;
  VDetectorUnit(VDetectorUnit&&) = delete;
  VDetectorUnit& operator=(const VDetectorUnit&) = delete;
  VDetectorUnit& operator=(VDetectorUnit&&) = delete;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VDetectorUnit_H */
