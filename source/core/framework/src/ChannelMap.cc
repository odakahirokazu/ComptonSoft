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

#include "ChannelMap.hh"

namespace comptonsoft {

ChannelMap::ChannelMap(std::size_t num_sections, std::size_t num_channels,
                       std::size_t num_x, std::size_t num_y)
  : VChannelMap(num_sections, num_channels)
{
  tableToChannel_.resize(num_x);
  for (auto& v: tableToChannel_) { v.resize(num_y); }
}

ChannelMap::~ChannelMap() = default;

} /* namespace comptonsoft */
