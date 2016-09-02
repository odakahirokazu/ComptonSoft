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

#include "ChannelID.hh"
#include <boost/format.hpp>

namespace comptonsoft
{

SectionChannelPair::SectionChannelPair(int section, int channel)
  : section_(section), channel_(channel)
{
}

SectionChannelPair::~SectionChannelPair() = default;

DetectorBasedChannelID::DetectorBasedChannelID(int detector, int section, int channel)
  : detector_(detector), sc_(section, channel)
{
}

DetectorBasedChannelID::~DetectorBasedChannelID() = default;

std::string DetectorBasedChannelID::toString() const
{
  if (Channel()==ChannelID::Undefined) {
    return (boost::format("d%04d_%04d")%Detector()%Section()).str();
    if (Section()==ChannelID::Undefined) {
      return (boost::format("d%04d")%Detector()).str();
    }
  }
  return (boost::format("d%04d_%04d_%04d")%Detector()%Section()%Channel()).str();
}

ReadoutBasedChannelID::ReadoutBasedChannelID(int readoutModule, int section, int channel)
  : readoutModule_(readoutModule), sc_(section, channel)
{
}

ReadoutBasedChannelID::~ReadoutBasedChannelID() = default;

std::string ReadoutBasedChannelID::toString() const
{
  if (Channel()==ChannelID::Undefined) {
    return (boost::format("r%04d_%04d")%ReadoutModule()%Section()).str();
    if (Section()==ChannelID::Undefined) {
      return (boost::format("r%04d")%ReadoutModule()).str();
    }
  }
  return (boost::format("r%04d_%04d_%04d")%ReadoutModule()%Section()%Channel()).str();
}

} /* nemespace comptonsoft */
