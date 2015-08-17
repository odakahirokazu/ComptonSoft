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

// ReadDataFile_SpW2.hh
// 2007-10-02  Hirokazu Odaka 
// 2007-11-02  Hirokazu Odaka 
// 2008-08-31  Hirokazu Odaka 

#ifndef COMPTONSOFT_ReadDataFile_SpW2_H
#define COMPTONSOFT_ReadDataFile_SpW2_H 1

#include "ReadDataFile.hh"

#include <fstream>
#include <string>

namespace comptonsoft {

class ReadDataFile_SpW2 : public ReadDataFile
{
  DEFINE_ANL_MODULE(ReadDataFile_SpW2, 3.2);
public:
  ReadDataFile_SpW2();
  ~ReadDataFile_SpW2() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_bgnrun();
  anl::ANLStatus mod_ana();

  int DeltaTime() const { return m_DeltaTime; }

private:
  const static int READ_BUF_SIZE = 16384;
  const static int HEADER_SIZE = 32;
  const static int DATA_HEADER_LENGTH = 4;

  std::ifstream m_fin;
  int m_ReadPacketSize;
  int m_DeltaTime;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadDataFile_SpW2_H */
