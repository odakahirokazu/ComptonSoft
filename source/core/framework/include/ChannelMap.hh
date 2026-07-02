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

#ifndef COMPTONSOFT_ChannelMap_H
#define COMPTONSOFT_ChannelMap_H 1

#include "VChannelMap.hh"

namespace comptonsoft {

/**
 * Readout channel map class.
 *
 * @author Hirokazu Odaka
 * @date 2008-08-28
 * @date 2014-09-18 | bidirectional mapping.
 * @date 2015-02-08 | derived from VChannalMap.
 */
class ChannelMap : public VChannelMap
{
public:
  ChannelMap(std::size_t num_sections, std::size_t num_channels,
             std::size_t num_x, std::size_t num_y);
  virtual ~ChannelMap();

  ChannelMap(const ChannelMap&) = default;
  ChannelMap(ChannelMap&&) = default;
  ChannelMap& operator=(const ChannelMap&) = default;
  ChannelMap& operator=(ChannelMap&&) = default;

  std::size_t NumX() const override
  {
    return tableToChannel_.size();
  }
  std::size_t NumY() const override
  {
    return tableToChannel_[0].size();
  }

  /** 
   * map a section index and channel index to a strip pair.
   * @param section section index (or chip index).
   * @param channel channel index.
   * @param pixel mapped pixel or strip pair.
   */
  void set(int section, int channel, const PixelID& pixel) override
  {
    VChannelMap::set(section, channel, pixel);
    tableToChannel_[pixel.X()][pixel.Y()] = SectionChannelPair{section, channel};
  }

  /** 
   * map a section index and channel index to a strip pair
   * @param section section index (or chip index).
   * @param channel channel index.
   * @param x mapped pixel/strip index along x-axis.
   * @param y mapped pixel/strip index along y-axis.
   */
  void set(int section, int channel, int x, int y) override
  {
    VChannelMap::set(section, channel, x, y);
    tableToChannel_[x][y] = SectionChannelPair{section, channel};
  }

  SectionChannelPair getSectionChannel(int x, int y) const override
  { return tableToChannel_[x][y]; }

  SectionChannelPair getSectionChannel(const PixelID& pixel) const override
  { return tableToChannel_[pixel.X()][pixel.Y()]; }

private:
  std::vector<std::vector<SectionChannelPair>> tableToChannel_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ChannelMap_H */
