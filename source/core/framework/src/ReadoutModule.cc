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

#include "ReadoutModule.hh"

namespace comptonsoft {

ReadoutModule::ReadoutModule()
  : ID_(0)
{
}

ReadoutModule::~ReadoutModule() = default;

void ReadoutModule::push(int detectorID, int detectorSection)
{
  sections_.push_back(DetectorBasedChannelID(detectorID,
                                             detectorSection,
                                             ChannelID::Undefined));
}

void ReadoutModule::clear()
{
  sections_.clear();
}

DetectorBasedChannelID ReadoutModule::getSection(int index) const
{
  return sections_[index];
}

} /* namespace comptonsoft */
