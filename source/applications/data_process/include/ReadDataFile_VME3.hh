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

// ReadDataFile_VME3.hh
// 2007-10-02  Hirokazu Odaka 
// 2007-11-02  Hirokazu Odaka 
// 2008-xx-xx  read dead time by Aono
// 2008-08-31  Hirokazu Odaka 

#ifndef COMPTONSOFT_ReadDataFile_VME3_H
#define COMPTONSOFT_ReadDataFile_VME3_H 1

#include "ReadDataFile.hh"

#include <fstream>
#include <string>

namespace comptonsoft {

class ReadDataFile_VME3 : public ReadDataFile
{
  DEFINE_ANL_MODULE(ReadDataFile_VME3, 3.2);
public:
  ReadDataFile_VME3();
  ~ReadDataFile_VME3() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  
  unsigned short int DeadTime() const { return m_DeadTime; }

private:
  const static int READ_BUF_SIZE = 16384;
  const static int HEADER_SIZE = 16;
  const static int FOOTER_SIZE = 4;
  const static int DATA_HEADER_LENGTH = 1;
  const static int DATA_FOOTER_LENGTH = 1;

  std::ifstream m_fin;
  int m_ReadPacketSize;
  unsigned short int m_DeadTime;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadDataFile_VME3_H */
