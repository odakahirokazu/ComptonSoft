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

#include "DetectorReadoutModule.hh"

namespace comptonsoft {

DetectorReadoutModule::DetectorReadoutModule()
  : ID_(0)
{
}

DetectorReadoutModule::~DetectorReadoutModule() = default;

void DetectorReadoutModule::push(int detectorID, int section)
{
  sections_.push_back(DetectorChannelID(detectorID,
                                        section,
                                        ChannelID::Undefined));
}

void DetectorReadoutModule::clear()
{
  sections_.clear();
}

DetectorChannelID DetectorReadoutModule::ReadoutSection(int index) const
{
  return sections_[index];
}

} /* namespace comptonsoft */
