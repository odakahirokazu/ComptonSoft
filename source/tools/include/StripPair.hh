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

#ifndef COMPTONSOFT_StripPair_H
#define COMPTONSOFT_StripPair_H 1

namespace comptonsoft {

/**
 * A class of a pair of strip/pixel IDs
 * @author Hirokazu Odaka
 */

class StripPair
{
public:
  StripPair(int x0=-1, int y0=-1 ) : x(x0), y(y0) {};
  
  int X() const { return x; }
  int Y() const { return y; }
  void set(int x0, int y0) { x=x0; y=y0; }
  void setX(int x0) { x=x0; }
  void setY(int y0) { y=y0; }

  StripPair Combine(const StripPair& sp)
  {
    int newx = (x>=0) ? x : sp.X();
    int newy = (y>=0) ? y : sp.Y();
    return StripPair(newx, newy);
  }
  
  bool operator==(const StripPair& r) const
  {
    return (x==r.x && y==r.y);
  }

  bool operator!=(const StripPair& r) const
  {
    return !(x==r.x && y==r.y);
  }

private:
  int x;
  int y;
};

}

#endif /* COMPTONSOFT_StripPair_H */
