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

#ifndef COMPTONSOFT_WriteEventTree_H
#define COMPTONSOFT_WriteEventTree_H 1

#include "VCSModule.hh"
#include <memory>

class TTree;

namespace anlgeant4 {
class InitialInformation;
}

namespace comptonsoft {

class EventTreeIOWithInitialInfo;
class CSHitCollection;

/**
 * 
 * @author Hirokazu Odaka
 * @date 2015-10-11
 */
class WriteEventTree : public VCSModule
{
  DEFINE_ANL_MODULE(WriteEventTree, 2.1);
public:
  WriteEventTree();
  ~WriteEventTree() = default;
  
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;

private:
  const CSHitCollection* hitCollection_;
  const anlgeant4::InitialInformation* initialInfo_;
  TTree* tree_;
  std::unique_ptr<EventTreeIOWithInitialInfo> treeIO_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_WriteEventTree_H */
