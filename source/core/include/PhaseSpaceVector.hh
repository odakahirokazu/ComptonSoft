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

/**
 * Class of a vector in a phase space.
 *
 * @author Hirokazu Odaka
 */
class PhaseSpaceVector
{
public:
  PhaseSpaceVector();
  PhaseSpaceVector(double energy,
                   double dirx, double diry, double dirz);
  PhaseSpaceVector(double energy,
                   double dirx, double diry, double dirz,
                   double time0,
                   double posx, double posy, double posz);
  ~PhaseSpaceVector();

  PhaseSpaceVector(const PhaseSpaceVector&) = default;
  PhaseSpaceVector(PhaseSpaceVector&&) = default;
  PhaseSpaceVector& operator=(const PhaseSpaceVector&) = default;
  PhaseSpaceVector& operator=(PhaseSpaceVector&&) = default;

  void setEnergy(double v) { energy_ = v; }
  void setDirection(double x, double y, double z)
  { directionX_ = x; directionY_ = y; directionZ_ = z; }
  
  void setTime(double v) { time_ = v; }
  void setPosition(double x, double y, double z)
  { positionX_ = x; positionY_ = y; positionZ_ = z; }

  double Energy() const { return energy_; }
  double DirectionX() const { return directionX_; }
  double DirectionY() const { return directionY_; }
  double DirectionZ() const { return directionZ_; }
  double Time() const { return time_; }
  double PositionX() const { return positionX_; }
  double PositionY() const { return positionY_; }
  double PositionZ() const { return positionZ_; }

private:
  double energy_;
  double directionX_;
  double directionY_;
  double directionZ_;
  double time_;
  double positionX_;
  double positionY_;
  double positionZ_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PhaseSpaceVector_H */
