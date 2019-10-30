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

#include "WriteXrayEventTree.hh"
#include "TTree.h"
#include "InitialInformation.hh"
#include "XrayEvent.hh"
#include "XrayEventTreeIO.hh"
#include "AnalyzeFrame.hh"

using namespace anlnext;

namespace comptonsoft
{

WriteXrayEventTree::WriteXrayEventTree()
  : treeIO_(new XrayEventTreeIO)
{
}

ANLStatus WriteXrayEventTree::mod_initialize()
{
  VCSModule::mod_initialize();

  get_module("XrayEventCollection", &collection_);
  define_evs("WriteXrayEventTree:Fill");

  tree_ = new TTree("xetree", "xetree");
  treeIO_->setTree(tree_);
  treeIO_->defineBranches();
  
  return AS_OK;
}

ANLStatus WriteXrayEventTree::mod_analyze()
{
  const int n = treeIO_->fillEvents(collection_->getEvents());
  if (n>0) {
    set_evs("WriteXrayEventTree:Fill");
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
