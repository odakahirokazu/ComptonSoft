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

#ifndef COMPTONSOFT_PixelID_H
#define COMPTONSOFT_PixelID_H 1

namespace comptonsoft {

/**
 * Class of a pixel or a pair of strips.
 *
 * @author Hirokazu Odaka
 * @date 2014-11-11
 */
class PixelID
{
public:
  constexpr static int Undefined = -1;
  constexpr static int OnMergin = -2;
  constexpr static int OnGuardRing = OnMergin;
  
public:
  PixelID(int x=Undefined, int y=Undefined);
  ~PixelID();
  PixelID(const PixelID&) = default;
  PixelID(PixelID&&) = default;
  PixelID& operator=(const PixelID&) = default;
  PixelID& operator=(PixelID&&) = default;
  
  int X() const { return x_; }
  int Y() const { return y_; }
  void set(int x, int y) { x_=x; y_=y; }
  void setX(int v) { x_ = v; }
  void setY(int v) { y_ = v; }

  PixelID combine(const PixelID& pixel)
  {
    int newx = (x_>=0) ? x_ : pixel.X();
    int newy = (y_>=0) ? y_ : pixel.Y();
    return PixelID(newx, newy);
  }
  
  bool operator==(const PixelID& r) const
  {
    return (x_==r.x_ && y_==r.y_);
  }

  bool operator!=(const PixelID& r) const
  {
    return !(x_==r.x_ && y_==r.y_);
  }

  bool isPixel() const
  {
    return x_>=0 && y_>=0;
  }

  bool isStrip() const
  {
    return isXStrip() || isYStrip();
  }

  bool isXStrip() const
  {
    return x_>=0 && y_==PixelID::Undefined;
  }

  bool isYStrip() const
  {
    return y_>=0 && x_==PixelID::Undefined;
  }

private:
  int x_ = Undefined;
  int y_ = Undefined;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PixelID_H */
