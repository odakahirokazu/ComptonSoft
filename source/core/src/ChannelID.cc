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

ChannelID::ChannelID(int section, int index)
  : section_(section), index_(index)
{
}

ChannelID::~ChannelID() = default;

DetectorChannelID::DetectorChannelID(int detector, int section, int index)
  : detector_(detector), channel_(section, index)
{
}

DetectorChannelID::~DetectorChannelID() = default;

std::string DetectorChannelID::toString() const
{
  if (Index()==ChannelID::Undefined) {
    return (boost::format("d%04d_%04d")%Detector()%Section()).str();
    if (Section()==ChannelID::Undefined) {
      return (boost::format("d%04d")%Detector()).str();
    }
  }
  return (boost::format("d%04d_%04d_%04d")%Detector()%Section()%Index()).str();
}

ReadoutChannelID::ReadoutChannelID(int readoutModule, int section, int index)
  : readoutModule_(readoutModule), channel_(section, index)
{
}

ReadoutChannelID::~ReadoutChannelID() = default;

std::string ReadoutChannelID::toString() const
{
  if (Index()==ChannelID::Undefined) {
    return (boost::format("r%04d_%04d")%ReadoutModule()%Section()).str();
    if (Section()==ChannelID::Undefined) {
      return (boost::format("r%04d")%ReadoutModule()).str();
    }
  }
  return (boost::format("r%04d_%04d_%04d")%ReadoutModule()%Section()%Index()).str();
}

} /* nemespace comptonsoft */
