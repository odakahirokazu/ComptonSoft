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

#include "XrayEventCollection.hh"

using namespace anlnext;

namespace comptonsoft
{

XrayEventCollection::XrayEventCollection()
{
}

XrayEventCollection::~XrayEventCollection() = default;

ANLStatus XrayEventCollection::mod_begin_run()
{
  initializeEvent();
  return AS_OK;
}

ANLStatus XrayEventCollection::mod_analyze()
{
  initializeEvent();
  return AS_OK;
}

ANLStatus XrayEventCollection::mod_end_run()
{
  initializeEvent();
  return AS_OK;
}

void XrayEventCollection::initializeEvent()
{
  events_.clear();
}

} /* namespace comptonsoft */
