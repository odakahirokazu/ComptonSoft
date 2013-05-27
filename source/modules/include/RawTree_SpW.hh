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

#ifndef COMPTONSOFT_RawTree_SpW_H
#define COMPTONSOFT_RawTree_SpW_H 1

#include "VCSModule.hh"
#include "TTree.h"

namespace comptonsoft {

class ReadDataFile_SpW2;


class RawTree_SpW : public VCSModule
{
  DEFINE_ANL_MODULE(RawTree_SpW, 1.0);
public:
  RawTree_SpW();
  ~RawTree_SpW() {}

  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_exit();

private:
  const ReadDataFile_SpW2* m_ReadDataModule;
  TTree* m_RawTree_SpW;
  std::vector<short int*> m_BufferVec;
  int m_Time;
  ULong64_t m_EventId;
  unsigned int m_DeltaT;
};

}

#endif /* COMPTONSOFT_RawTree_SpW_H */
