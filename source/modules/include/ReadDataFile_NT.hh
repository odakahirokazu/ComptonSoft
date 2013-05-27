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

// ReadDataFile_NT.hh
// 2007-10-02  Hirokazu Odaka 
// 2007-11-02  Hirokazu Odaka 
// 2008-08-31  Hirokazu Odaka 
// 2009-09-21  Hirokazu Odaka 
// 2011-11-20  Hirokazu Odaka : file vector

#ifndef COMPTONSOFT_ReadDataFile_NT_H
#define COMPTONSOFT_ReadDataFile_NT_H 1

#include "ReadDataFile.hh"

#include <fstream>
#include <string>
#include <vector>

class TChain;

namespace comptonsoft {

class ReadDataFile_NT : public ReadDataFile
{
  DEFINE_ANL_MODULE(ReadDataFile_NT, 3.1);
public:
  ReadDataFile_NT() {}
  ~ReadDataFile_NT() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_exit();

private:
  TChain* m_Tree;
  std::vector<std::string> m_FileNameVector;
  unsigned int m_NEvent;
  unsigned short int** m_ADC;
  int m_UnixTime;
  int m_NumASIC;
  // unsigned int m_LiveTime;
  // unsigned int m_IntegralLiveTime;
};

}

#endif /* COMPTONSOFT_ReadDataFile_NT_H */
