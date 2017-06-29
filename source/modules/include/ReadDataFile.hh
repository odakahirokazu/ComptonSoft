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

#ifndef COMPTONSOFT_ReadDataFile_H
#define COMPTONSOFT_ReadDataFile_H 1

#include "VCSModule.hh"
#include <string>
#include <vector>

namespace comptonsoft {

/**
 * Module for reading data files.
 * @author Hirokazu Odaka
 * @date 2007-10-02
 * @date 2011-04-13
 * @date 2014-11-25
 */
class ReadDataFile : public VCSModule
{
  DEFINE_ANL_MODULE(ReadDataFile, 3.2);
public:
  ReadDataFile();
  ~ReadDataFile() = default;

  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_init() override;
  anl::ANLStatus mod_ana() override
  { ++m_EventID; return anl::AS_OK; }

  int EventID() const { return m_EventID; }
  int Time() const { return m_Time; }

protected:
  void setTime(int v) { m_Time = v; }
  std::string nextFile() { return *(m_FileIterator++); }
  bool wasLastFile() const { return (m_FileIterator==m_FileList.end()); }
  bool checkFiles();
  
private:
  int m_EventID;
  int m_Time;
  std::vector<std::string> m_FileList;
  std::vector<std::string>::const_iterator m_FileIterator;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadDataFile_H */
