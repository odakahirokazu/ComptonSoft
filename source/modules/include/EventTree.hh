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

#ifndef COMPTONSOFT_EventTree_H
#define COMPTONSOFT_EventTree_H 1

#include "VCSModule.hh"
#include "TTree.h"

namespace comptonsoft {

class ReadDataFile;

class EventTree : public VCSModule
{
  DEFINE_ANL_MODULE(EventTree, 2.0);
public:
  EventTree();
  ~EventTree() {}

  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_exit();

private:
  const ReadDataFile* m_ReadDataModule;
  TTree* m_EventTree;
  std::vector<short int*> m_ADCBufferVec;
  std::vector<float*> m_PHABufferVec;
  int m_Time;
  ULong64_t m_EventId;
};

}

#endif /* COMPTONSOFT_EventTree_H */
