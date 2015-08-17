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

#include "ChannelMapDSD.hh"

namespace comptonsoft {

ChannelMapDSD::ChannelMapDSD(std::size_t num_sections, std::size_t num_channels,
                             std::size_t num_x, std::size_t num_y)
  : VChannelMap::VChannelMap(num_sections, num_channels)
{
  tableToChannelX_.resize(num_x);
  tableToChannelY_.resize(num_y);
}

ChannelMapDSD::~ChannelMapDSD() = default;

void ChannelMapDSD::set(int section, int index, const PixelID& pixel)
{
  VChannelMap::set(section, index, pixel);
  if (pixel.isXStrip()) {
    tableToChannelX_[pixel.X()] = ChannelID{section, index};
  }
  else if (pixel.isYStrip()) {
    tableToChannelY_[pixel.Y()] = ChannelID{section, index};
  }
}

void ChannelMapDSD::set(int section, int index, int x, int y)
{
  VChannelMap::set(section, index, x, y);
  if (x >= 0) {
    tableToChannelX_[x] = ChannelID{section, index};
  }
  else if (y >= 0) {
    tableToChannelY_[y] = ChannelID{section, index};
  }
}

ChannelID ChannelMapDSD::getChannel(int x, int y) const
{
  if (x >= 0) {
    return tableToChannelX_[x];
  }
  else if (y >= 0) {
    return tableToChannelY_[y];
  }
  return ChannelID();
}

ChannelID ChannelMapDSD::getChannel(const PixelID& pixel) const
{
  if (pixel.isXStrip()) {
    return tableToChannelX_[pixel.X()];
  }
  else if (pixel.isYStrip()) {
    return tableToChannelY_[pixel.Y()];
  }
  return ChannelID();
}

} /* namespace comptonsoft */
