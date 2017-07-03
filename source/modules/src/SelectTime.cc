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

using namespace anl;

namespace comptonsoft
{

SelectTime::SelectTime()
  : m_ReadDataModule(0), m_Time0(0), m_Time1(0)
{
}

ANLStatus SelectTime::mod_startup()
{
  register_parameter(&m_Time0, "time_start");
  register_parameter(&m_Time1, "time_end");
  
  return AS_OK;
}

ANLStatus SelectTime::mod_init()
{
  GetModuleNC("ReadDataFile", &m_ReadDataModule);
  EvsDef("SelectTime:OK");

  return AS_OK;
}

ANLStatus SelectTime::mod_ana()
{
  const int t = m_ReadDataModule->Time();
  if (t < m_Time0 || m_Time1 < t) {
    return AS_SKIP;
  }

  EvsSet("SelectTime:OK");

  return AS_OK;
}

} /* namespace comptonsoft */
