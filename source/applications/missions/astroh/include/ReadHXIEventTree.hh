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
  DEFINE_ANL_MODULE(ReadHXIEventTree, 0.1);
public:
  ReadHXIEventTree();
  ~ReadHXIEventTree() = default;
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  
private:
  std::vector<std::string> m_Files;
  std::vector<std::string> m_Trees;

  TChain* m_EventTree;
  Long64_t m_NumEvents;
  Long64_t m_Index;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadHXIEventTree_H */
