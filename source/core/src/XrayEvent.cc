/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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

#include "XrayEvent.hh"

#include <iostream>
#include <cmath>

namespace comptonsoft
{

XrayEvent::XrayEvent(int size)
  : size_(size),
    data_(boost::extents[size][size])
{
}

XrayEvent::~XrayEvent() = default;

void XrayEvent::copyFrom(const image_t& image, int ix, int iy)
{
  const int size = EventSize();
  const int halfSize = size/2;
  const int ix0 = ix-halfSize;
  const int iy0 = iy-halfSize;

  ix_ = ix;
  iy_ = iy;

  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      data_[i][j] = image[ix0+i][iy0+j];
    }
  }
  centerPH_ = data_[halfSize][halfSize];
}

void XrayEvent::reduce()
{
  const int size = EventSize();
  const int halfSize = size/2;

  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      const double v = data_[i][j];
      if (v < SplitThreshold()) {
        data_[i][j] = 0.0;
      }
    }
  }

  int rank = 0;
  int weight = 0;
  double value = 0.0;
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      const double v = data_[i][j];
      if (v > 0.0) {
        weight++;
        value += v;
        const int dx = std::abs(i-halfSize);
        const int dy = std::abs(j-halfSize);
        const int thisRank = std::max(dx, dy);
        if (rank < thisRank) {
          rank = thisRank;
        }
      }
    }
  }

  rank_ = rank;
  weight_ = weight;
  value_ = value;

  angle_ = calculateEventAngle();
}

double XrayEvent::calculateEventAngle() const
{
  const int size = EventSize();
  const int center = size/2;
  double x(0.0), y(0.0);
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      if (!(i==center && j==center)) {
        const double epi = data_[i][j];
        const double x1 = static_cast<double>(i-center);
        const double y1 = static_cast<double>(j-center);
        x += x1*epi;
        y += y1*epi;
      }
    }
  }
  return std::atan2(y, x);
}

} /* namespace comptonsoft */
