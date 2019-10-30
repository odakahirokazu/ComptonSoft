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

/**
 * ReadXrayEventTree
 *
 * @author Hirokazu Odaka
 * @date 2019-10-30
 */

#ifndef COMPTONSOFT_ReadXrayEventTree_H
#define COMPTONSOFT_ReadXrayEventTree_H 1

#include <anlnext/BasicModule.hh>

class TChain;

namespace comptonsoft {

class XrayEventCollection;
class XrayEventTreeIO;

class ReadXrayEventTree : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ReadXrayEventTree, 1.1);
  // ENABLE_PARALLEL_RUN();
public:
  ReadXrayEventTree();
  
protected:
  ReadXrayEventTree(const ReadXrayEventTree&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  std::vector<std::string> fileList_;
  int eventSize_ = 1;

  TChain* tree_;
  int64_t numEntries_ = 0;
  int64_t entryIndex_ = 0;

  XrayEventCollection* collection_ = nullptr;
  std::unique_ptr<XrayEventTreeIO> treeIO_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadXrayEventTree_H */
