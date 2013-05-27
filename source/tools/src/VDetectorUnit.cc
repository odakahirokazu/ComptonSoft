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

// VDetectorUnit class
// VDetectorUnit.hh
//
// 2007-12-xx Hirokazu Odaka
// 2008-07-29 Hirokazu Odaka
//

#include "VDetectorUnit.hh"
#include "HXISGDFlagDef.hh"

namespace comptonsoft {

VDetectorUnit::VDetectorUnit()
{
}


VDetectorUnit::~VDetectorUnit()
{
}


std::string VDetectorUnit::getNamePrefix() const
{
  std::string name = getName();
  size_t c = name.find(':');
  if (c == std::string::npos) {
    return name;
  }
  return std::string(name, 0, c);
}


vector3_t VDetectorUnit::Position(int pixelX, int pixelY) const
{
  vector3_t pos;
  vector3_t dirx = m_DirectionX;
  vector3_t diry = m_DirectionY;
  
  pos = m_CenterPosition - 0.5*m_WidthX*dirx - 0.5*m_WidthY*diry;
  pos += m_OffsetX*dirx + m_OffsetY*diry;
  pos += (0.5+pixelX)*m_PixelPitchX*dirx + (0.5+pixelY)*m_PixelPitchY*diry;

  return pos;
}


vector3_t VDetectorUnit::LocalPosition(int pixelX, int pixelY) const
{
  vector3_t pos;
  vector3_t dirx(1., 0., 0.);
  vector3_t diry(0., 1., 0.);
  
  pos = - 0.5*m_WidthX*dirx - 0.5*m_WidthY*diry;
  pos += m_OffsetX*dirx + m_OffsetY*diry;
  pos += (0.5+pixelX)*m_PixelPitchX*dirx + (0.5+pixelY)*m_PixelPitchY*diry;

  return pos;
}


StripPair
VDetectorUnit::StripXYByLocalPosition(double localx, double localy) const
{
  int xstrip = comptonsoft::NO_STRIP;
  int ystrip = comptonsoft::NO_STRIP;

  if (m_PixelPitchX <= 0.0 && m_PixelPitchY <= 0.0) {
    return StripPair(xstrip, ystrip);
  }
  
  double detOriginX = -0.5 * m_WidthX + m_OffsetX;
  double detOriginY = -0.5 * m_WidthY + m_OffsetY;

  double x = localx - detOriginX;
  double y = localy - detOriginY;

  if (x<0 || y<0) {
    xstrip = comptonsoft::GUARD_RING;
    ystrip = comptonsoft::GUARD_RING;
  }
  else {
    xstrip = static_cast<int>(x/m_PixelPitchX);
    ystrip = static_cast<int>(y/m_PixelPitchY);
    
    if (xstrip >= m_NumPixelX || ystrip >= m_NumPixelY) {
      xstrip = comptonsoft::GUARD_RING;
      ystrip = comptonsoft::GUARD_RING;
    }
  }
  
  return StripPair(xstrip, ystrip);
}

}
