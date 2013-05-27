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

#ifndef COMPTONSOFT_PhaseSpaceVector_H
#define COMPTONSOFT_PhaseSpaceVector_H 1

namespace comptonsoft {

class PhaseSpaceVector
{
public:
  PhaseSpaceVector()
    : m_Energy(0.0),
      m_DirectionX(0.0), m_DirectionY(0.0), m_DirectionZ(-1.0),
      m_Time(0.0),
      m_PositionX(0.0), m_PositionY(0.0), m_PositionZ(0.0)
  {}

  PhaseSpaceVector(double energy,
                   double dirx, double diry, double dirz)
    : m_Energy(energy),
      m_DirectionX(dirx), m_DirectionY(diry), m_DirectionZ(dirz),
      m_Time(0.0),
      m_PositionX(0.0), m_PositionY(0.0), m_PositionZ(0.0)
  {}

  PhaseSpaceVector(double energy,
                   double dirx, double diry, double dirz,
                   double time0,
                   double posx, double posy, double posz)
    : m_Energy(energy),
      m_DirectionX(dirx), m_DirectionY(diry), m_DirectionZ(dirz),
      m_Time(time0),
      m_PositionX(posx), m_PositionY(posy), m_PositionZ(posz)
  {}

  void setEnergy(double v) { m_Energy = v; }
  void setDirection(double x, double y, double z)
  { m_DirectionX = x; m_DirectionY = y; m_DirectionZ = z; }
  
  void setTime(double v) { m_Time = v; }
  void setPosition(double x, double y, double z)
  { m_PositionX = x; m_PositionY = y; m_PositionZ = z; }

  double Energy() const { return m_Energy; }
  double DirectionX() const { return m_DirectionX; }
  double DirectionY() const { return m_DirectionY; }
  double DirectionZ() const { return m_DirectionZ; }
  double Time() const { return m_Time; }
  double PositionX() const { return m_PositionX; }
  double PositionY() const { return m_PositionY; }
  double PositionZ() const { return m_PositionZ; }

private:
  double m_Energy;
  double m_DirectionX;
  double m_DirectionY;
  double m_DirectionZ;
  double m_Time;
  double m_PositionX;
  double m_PositionY;
  double m_PositionZ;
};

}

#endif /* COMPTONSOFT_PhaseSpaceVector_H */
