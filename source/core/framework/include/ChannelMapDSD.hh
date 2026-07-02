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

#ifndef COMPTONSOFT_ChannelMapDSD_H
#define COMPTONSOFT_ChannelMapDSD_H 1

#include "VChannelMap.hh"

namespace comptonsoft {

/**
 * Readout channel map class.
 *
 * @author Hirokazu Odaka
 * @date 2015-02-08 | derived from VChannelMap.
 */
class ChannelMapDSD : public VChannelMap
{
public:
  ChannelMapDSD(std::size_t num_sections, std::size_t num_channels,
                std::size_t num_x, std::size_t num_y);
  virtual ~ChannelMapDSD();

  ChannelMapDSD(const ChannelMapDSD&) = default;
  ChannelMapDSD(ChannelMapDSD&&) = default;
  ChannelMapDSD& operator=(const ChannelMapDSD&) = default;
  ChannelMapDSD& operator=(ChannelMapDSD&&) = default;

  std::size_t NumX() const override
  {
    return tableToChannelX_.size();
  }

  std::size_t NumY() const override
  {
    return tableToChannelY_.size();
  }

  /** 
   * map a section index and channel index to a strip pair.
   * @param section section index (or chip index).
   * @param channel channel index.
   * @param pixel mapped pixel or strip pair.
   */
  void set(int section, int index, const PixelID& pixel) override;

  /** 
   * map a section index and channel index to a strip pair
   * @param section section index (or chip index).
   * @param channel channel index.
   * @param x mapped pixel/strip index along x-axis.
   * @param y mapped pixel/strip index along y-axis.
   */
  void set(int section, int index, int x, int y) override;

  SectionChannelPair getSectionChannel(int x, int y) const override;
  SectionChannelPair getSectionChannel(const PixelID& pixel) const override;

private:
  std::vector<SectionChannelPair> tableToChannelX_;
  std::vector<SectionChannelPair> tableToChannelY_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ChannelMapDSD_H */
