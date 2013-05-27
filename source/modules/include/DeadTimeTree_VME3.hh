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

#ifndef COMPTONSOFT_DeadTimeTree_VME3_H
#define COMPTONSOFT_DeadTimeTree_VME3_H 1

#include "VCSModule.hh"

#include "TTree.h"

namespace comptonsoft {

class ReadDataFile_VME3;

class DeadTimeTree_VME3 : public VCSModule
{
  DEFINE_ANL_MODULE(DeadTimeTree_VME3, 1.0)
public:
  DeadTimeTree_VME3();
  ~DeadTimeTree_VME3() {}

  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

private:
  const ReadDataFile_VME3* m_ReadDataModule;
  TTree* m_DeadTimeTree;
  int m_time;
  double m_deadtime;
};

}

#endif /* COMPTONSOFT_DeadTimeTree_VME3_H */
