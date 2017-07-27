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

#include "ReadDataFile.hh"

using namespace anlnext;

namespace comptonsoft
{

ReadDataFile::ReadDataFile()
  : m_EventID(0), m_Time(0)
{
  add_alias("ReadDataFile");
}

ANLStatus ReadDataFile::mod_define()
{
  register_parameter(&m_FileList, "file_list");
  return AS_OK;
}

ANLStatus ReadDataFile::mod_initialize()
{
  m_FileIterator = m_FileList.begin();
  return AS_OK;
}

bool ReadDataFile::checkFiles()
{
  for (auto& filename: m_FileList) {
    std::ifstream fin;
    fin.open( filename.c_str() );
    if (!fin) {
      std::cout << "ReadDataFile: cannot open " << filename << std::endl;
      return false;
    }
    fin.close();
  }
  return true;
}

} /* namespace comptonsoft */
