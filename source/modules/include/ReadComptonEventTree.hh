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

#ifndef COMPTONSOFT_ReadComptonEventTree_H
#define COMPTONSOFT_ReadComptonEventTree_H 1

#include "EventReconstruction.hh"
#include "InitialInformation.hh"

#include <vector>
#include <string>

class TChain;

namespace comptonsoft {

class ComptonEventTreeIOWithInitialInfo;

/*
 * Read Compton event tree.
 * @author Hirokazu Odaka
 * @date 2008-12-15
 * @date 2015-03-01
 */
class ReadComptonEventTree : public EventReconstruction, public anlgeant4::InitialInformation
{
  DEFINE_ANL_MODULE(ReadComptonEventTree, 3.1);
public:
  ReadComptonEventTree();
  ~ReadComptonEventTree() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
   
private:
  std::vector<std::string> fileList_;
  TChain* cetree_;
  int64_t numEntries_ = 0;
  int64_t entryIndex_ = 0;

  std::unique_ptr<ComptonEventTreeIOWithInitialInfo> treeIO_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadComptonEventTree_H */
