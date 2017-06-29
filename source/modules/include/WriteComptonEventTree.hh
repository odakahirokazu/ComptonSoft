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

#ifndef COMPTONSOFT_WriteComptonEventTree_H
#define COMPTONSOFT_WriteComptonEventTree_H 1

#include "VCSModule.hh"

class TTree;

namespace anlgeant4 {
class InitialInformation;
}

namespace comptonsoft {

class EventReconstruction;
class ComptonEventTreeIOWithInitialInfo;

class WriteComptonEventTree : public VCSModule
{
  DEFINE_ANL_MODULE(WriteComptonEventTree, 2.0);
public:
  WriteComptonEventTree();
  ~WriteComptonEventTree();

  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_init() override;
  anl::ANLStatus mod_his() override;
  anl::ANLStatus mod_ana() override;

protected:
  TTree * cetree_;
  
private:
  EventReconstruction* eventReconstruction_;
  const anlgeant4::InitialInformation* initialInfo_;
  std::unique_ptr<ComptonEventTreeIOWithInitialInfo> treeIO_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_WriteComptonEventTree_H */
