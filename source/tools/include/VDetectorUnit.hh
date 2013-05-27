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
#include <boost/utility.hpp>
#include "cs_globals.hh"
#include "StripPair.hh"

namespace comptonsoft {


/**
 * A virtual class of a detector unit. This class contains geometrical inforamtion of a detector.
 *
 * @author Hirokazu Odaka
 * @date 2008-08-22
 */
class VDetectorUnit : private boost::noncopyable
{
public:
  VDetectorUnit();
  virtual ~VDetectorUnit();

public:
  virtual std::string Type() = 0;
  
  void setName(const std::string& name)
  { m_Name = name; }
  std::string getName() const { return m_Name; }
  std::string getNamePrefix() const;
  
  void setID(int i) { m_ID = i; }
  int getID() const { return m_ID; }
  
  void setWidth(double x, double y)
  { m_WidthX = x; m_WidthY = y; }
  double getWidthX() const { return m_WidthX; }
  double getWidthY() const { return m_WidthY; }

  void setThickness(double r) { m_Thickness = r; }
  double getThickness() const { return m_Thickness; }
  
  void setOffset(double x, double y)
  { m_OffsetX = x; m_OffsetY = y; }
  double getOffsetX() const { return m_OffsetX; }  
  double getOffsetY() const { return m_OffsetY; }  
  
  void setPixelPitch(double x, double y)
  { m_PixelPitchX = x; m_PixelPitchY = y; }
  double getPixelPitchX() const { return m_PixelPitchX; }  
  double getPixelPitchY() const { return m_PixelPitchY; }  
  
  void setNumPixel(int x, int y)
  { m_NumPixelX = x; m_NumPixelY = y; }
  double getNumPixelX() const { return m_NumPixelX; }  
  double getNumPixelY() const { return m_NumPixelY; }  
  
  void setCenterPosition(double x, double y, double z)
  { m_CenterPosition.set(x, y, z); }
  void setCenterPosition(const vector3_t& v) { m_CenterPosition = v; } 
  vector3_t getCenterPosition() const { return m_CenterPosition; }
  double getCenterPositionX() const { return m_CenterPosition.x(); }
  double getCenterPositionY() const { return m_CenterPosition.y(); }
  double getCenterPositionZ() const { return m_CenterPosition.z(); }
  
  void setDirectionX(double x, double y, double z) 
  {
    m_DirectionX.set(x, y, z);
    m_DirectionX = m_DirectionX.unit();
  }
  void setDirectionX(const vector3_t& v) { m_DirectionX = v.unit(); } 
  vector3_t getDirectionX() const { return m_DirectionX; }
  double getDirectionXX() const { return m_DirectionX.x(); }
  double getDirectionXY() const { return m_DirectionX.y(); }
  double getDirectionXZ() const { return m_DirectionX.z(); }
  
  void setDirectionY(double x, double y, double z) 
  { 
    m_DirectionY.set(x, y, z);
    m_DirectionY = m_DirectionY.unit();
  } 
  void setDirectionY(const vector3_t& v) { m_DirectionY = v.unit(); } 
  vector3_t getDirectionY() const { return m_DirectionY; }
  double getDirectionYX() const { return m_DirectionY.x(); }
  double getDirectionYY() const { return m_DirectionY.y(); }
  double getDirectionYZ() const { return m_DirectionY.z(); }
  
  /** 
   * calculate global position of the pixel center.
   * @param pixelX the pixel ID along the x axis of the detector
   * @param pixelY the pixel ID along the y axis of the detector
   * @return global position of the pixel ceneter
   */
  vector3_t Position(int pixelX, int pixelY) const;

  /**
   * calculate global position of the pixel center.
   * @param pixelX the pixel ID along the x axis of the detector
   * @param pixelY the pixel ID along the y axis of the detector
   * @param[out] x x-component of global position of the pixel ceneter
   * @param[out] y y-component of global position of the pixel ceneter
   * @param[out] z z-component of global position of the pixel ceneter
   */
  void Position(int pixelX, int pixelY,
		double* x, double* y, double* z) const;

  /**
   * calculate global position of the pixel center.
   * @param sp the pixel ID in @link StripPair @endlink
   * @return global position of the pixel ceneter
   */
  vector3_t Position(const StripPair& sp) const;

  /**
   * calculate global position of the pixel center.
   * @param sp the pixel ID in @link StripPair @endlink
   * @param[out] x x-component of global position of the pixel ceneter
   * @param[out] y y-component of global position of the pixel ceneter
   * @param[out] z z-component of global position of the pixel ceneter
   */
  void Position(const StripPair& sp,
		double* x, double* y, double* z) const;

  vector3_t LocalPosition(int pixelX, int pixelY) const;
  void LocalPosition(int pixelX, int pixelY,
		     double* x, double* y, double* z) const;
  vector3_t LocalPosition(const StripPair& sp) const;
  void LocalPosition(const StripPair& sp,
		     double* x, double* y, double* z) const;
  
  StripPair StripXYByLocalPosition(double localx, double localy) const;

private:
  std::string m_Name;
  int m_ID;
  
  double m_WidthX;
  double m_WidthY;
  double m_Thickness;
  double m_OffsetX;
  double m_OffsetY;
  double m_PixelPitchX;
  double m_PixelPitchY;
  int m_NumPixelX;
  int m_NumPixelY;
  
  vector3_t m_CenterPosition;
  vector3_t m_DirectionX;
  vector3_t m_DirectionY;
};


inline void VDetectorUnit::Position(int pixelX, int pixelY,
				    double* x, double* y, double* z) const
{
  vector3_t pos = Position(pixelX, pixelY);
  *x = pos.x();
  *y = pos.y();
  *z = pos.z();
}


inline
vector3_t VDetectorUnit::Position(const StripPair& sp) const
{
  return Position(sp.X(), sp.Y());
}


inline
void VDetectorUnit::Position(const StripPair& sp,
			     double* x, double* y, double* z) const
{
  Position(sp.X(), sp.Y(), x, y, z);
}


inline
void VDetectorUnit::LocalPosition(int pixelX, int pixelY,
				  double* x, double* y, double* z) const
{
  vector3_t pos = LocalPosition(pixelX, pixelY);
  *x = pos.x();
  *y = pos.y();
  *z = pos.z();
}


inline
vector3_t VDetectorUnit::LocalPosition(const StripPair& sp) const
{
  return LocalPosition(sp.X(), sp.Y());
}


inline
void VDetectorUnit::LocalPosition(const StripPair& sp,
				  double* x, double* y, double* z) const
{
  LocalPosition(sp.X(), sp.Y(), x, y, z);
}

}

#endif /* COMPTONSOFT_VDetectorUnit_H */
