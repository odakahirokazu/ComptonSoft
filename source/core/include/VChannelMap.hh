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

#ifndef COMPTONSOFT_VChannelMap_H
#define COMPTONSOFT_VChannelMap_H 1

#include <cstddef>
#include <vector>
#include "CSTypes.hh"
#include "PixelID.hh"
#include "ChannelID.hh"

namespace comptonsoft {

/**
 * Virtual class of readout channel map.
 *
 * @author Hirokazu Odaka
 * @date 2015-02-08
 */
class VChannelMap
{
public:
  VChannelMap(std::size_t num_sections, std::size_t num_channels);
  virtual ~VChannelMap();

  VChannelMap(const VChannelMap&) = default;
  VChannelMap(VChannelMap&&) = default;
  VChannelMap& operator=(const VChannelMap&) = default;
  VChannelMap& operator=(VChannelMap&&) = default;

  std::size_t NumSections() const
  { return tableToPixel_.size(); }
  std::size_t NumChannels(std::size_t section) const
  { return tableToPixel_[section].size(); }
  
  virtual std::size_t NumX() const = 0;
  virtual std::size_t NumY() const = 0;

  /** 
   * map a section index and channel index to a strip pair.
   * @param section section index (or chip index).
   * @param channel channel index.
   * @param pixel mapped pixel or strip pair.
   */
  virtual void set(int section, int channel, const PixelID& pixel)
  {
    tableToPixel_[section][channel] = pixel;
  }

  /** 
   * map a section index and channel index to a strip pair
   * @param section section index (or chip index).
   * @param channel channel index.
   * @param x mapped pixel/strip index along x-axis.
   * @param y mapped pixel/strip index along y-axis.
   */
  virtual void set(int section, int channel, int x, int y)
  {
    tableToPixel_[section][channel].set(x,y);
  }

  /**
   * get a mapped strip index pair
   * @param section section index (or chip index)
   * @param channel channel index
   * @return sp mapped strip ID pair
   */
  PixelID getPixel(int section, int channel) const
  { return tableToPixel_[section][channel]; }

  PixelID getPixel(const SectionChannelPair& section_channel) const
  {
    return tableToPixel_[section_channel.Section()][section_channel.Channel()];
  }

  virtual SectionChannelPair getSectionChannel(int x, int y) const = 0;
  virtual SectionChannelPair getSectionChannel(const PixelID& pixel) const = 0;

private:
  std::vector<std::vector<PixelID>> tableToPixel_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VChannelMap_H */
