/*************************************************************************
 *                                                                       *
 * Copyright (c) 2013 Hirokazu Odaka                                     *
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

#ifndef COMPTONSOFT_ReadHXIEventTree_H
#define COMPTONSOFT_ReadHXIEventTree_H 1

#include <vector>
#include "VCSModule.hh"
#include "InitialInformation.hh"
#include "TChain.h"

namespace comptonsoft {

/**
 * ANL module to read HXI event ROOT files.
 *
 * @author Hirokazu Odaka
 * @date 2013-12-16
 */
class ReadHXIEventTree : public VCSModule, public anlgeant4::InitialInformation
{
  DEFINE_ANL_MODULE(ReadHXIEventTree, 0.0);
public:
  ReadHXIEventTree();
  ~ReadHXIEventTree() {}
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  
private:
  std::vector<std::string> file_names;
  std::vector<std::string> tree_names;

  TChain* event_tree;
  Long64_t num_events;
  Long64_t index;
};

}

#endif /* COMPTONSOFT_ReadHXIEventTree_H */
