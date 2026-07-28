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

#include "SelectTime.hh"
#include "ReadDataFile.hh"

using namespace anlnext;

namespace comptonsoft
{

SelectTime::SelectTime()
  : read_data_module_(0), time_start_(0), time_end_(0)
{
}

ANLStatus SelectTime::mod_define()
{
  define_parameter("time_start", &mod_class::time_start_);
  define_parameter("time_end", &mod_class::time_end_);

  return AS_OK;
}

ANLStatus SelectTime::mod_initialize()
{
  get_module_NC("ReadDataFile", &read_data_module_);
  define_evs("SelectTime:OK");

  return AS_OK;
}

ANLStatus SelectTime::mod_analyze()
{
  const int t = read_data_module_->Time();
  if (t < time_start_ || time_end_ < t) {
    return AS_SKIP;
  }

  set_evs("SelectTime:OK");

  return AS_OK;
}

} /* namespace comptonsoft */
