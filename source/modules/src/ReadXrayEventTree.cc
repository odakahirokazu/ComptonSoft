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

#include "ReadXrayEventTree.hh"

#include <TChain.h>
#include "XrayEvent.hh"
#include "XrayEventCollection.hh"
#include "XrayEventTreeIO.hh"

using namespace anlnext;

namespace comptonsoft {

ReadXrayEventTree::ReadXrayEventTree()
  : eventSize_(5),
    treeIO_(new XrayEventTreeIO)
{
}

ANLStatus ReadXrayEventTree::mod_define()
{
  define_parameter("file_list", &mod_class::fileList_);
  define_parameter("event_size", &mod_class::eventSize_);
  
  return AS_OK;
}

ANLStatus ReadXrayEventTree::mod_initialize()
{
  get_module_NC("XrayEventCollection", &collection_);
  
  tree_ = new TChain("xetree");
  for (const std::string& filename: fileList_) {
    tree_->Add(filename.c_str());
  }

  treeIO_->setEventSize(eventSize_);
  treeIO_->setTree(tree_);
  treeIO_->setBranchAddresses();

  numEntries_ = tree_->GetEntries();
  std::cout << "Number of entries: " << numEntries_ << std::endl;

  return AS_OK;
}

ANLStatus ReadXrayEventTree::mod_begin_run()
{
  if (numEntries_ == 0) { return AS_OK; }

  return AS_OK;
}

ANLStatus ReadXrayEventTree::mod_analyze()
{
  if (entryIndex_ == numEntries_) {
    return AS_QUIT;
  }

  auto events = treeIO_->retrieveEvents(entryIndex_);
  for (auto& event: events) {
    collection_->insertEvent(event);
  }

  return AS_OK;
}

} /* namespace comptonsoft */
