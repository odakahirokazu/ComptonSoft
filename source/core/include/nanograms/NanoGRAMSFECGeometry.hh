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

#ifndef COMPTONSOFT_NanoGRAMSFECGeometry_H
#define COMPTONSOFT_NanoGRAMSFECGeometry_H 1

#include <array>
#include <cstdint>
#include <utility>
#include <vector>

#include "NanoGRAMSConstants.hh"

namespace comptonsoft
{
namespace grams
{

using PixelADU  = std::array<double,  NUM_CH_EACH_VATA>;
using PixelMask = std::array<uint8_t, NUM_CH_EACH_VATA>;

struct Config;

struct AnodeChannelTopology
{
  std::array<std::array<std::pair<int, int>, NUM_CH_EACH_VATA>, NUM_VATA> section_grid_of_channel{};
  std::array<std::array<std::pair<int, int>, NUM_CH_EACH_VATA>, NUM_VATA> anode_grid_of_channel{};
  std::array<std::array<std::vector<int>, NUM_CH_EACH_VATA>, NUM_VATA> section_cross_neighbors{};
  std::array<std::array<std::vector<int>, NUM_CH_EACH_VATA>, NUM_VATA> section_diag_neighbors{};
  std::array<std::array<std::vector<std::pair<int, int>>, NUM_CH_EACH_VATA>, NUM_VATA> cross_section_neighbors{};
  std::array<std::array<std::vector<std::pair<int, int>>, NUM_CH_EACH_VATA>, NUM_VATA> diag_section_neighbors{};
  std::array<std::vector<int>, NUM_VATA> periphery{};
};

AnodeChannelTopology buildAnodeChannelTopology();
PixelMask buildFECMask(const Config& cfg, int fec);

} /* namespace grams */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSFECGeometry_H */
