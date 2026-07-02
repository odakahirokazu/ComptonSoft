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

#ifndef COMPTONSOFT_SortEventTreeWithTime_H
#define COMPTONSOFT_SortEventTreeWithTime_H 1

#include "VCSModule.hh"
#include "InitialInformation.hh"

#include <list>
#include <vector>
#include <string>
#include <cstdint>
#include "DetectorHit_sptr.hh"
#include <memory>

class TChain;
class TTree;

namespace comptonsoft {

class CSHitCollection;
class EventTreeIOWithInitialInfo;

/**
 * @author Tsubasa Tamba
 * @date 2014-06-07
 */
class SortEventTreeWithTime : public VCSModule, public anlgeant4::InitialInformation
{
  DEFINE_ANL_MODULE(SortEventTreeWithTime, 1.0);
public:
  SortEventTreeWithTime();
  ~SortEventTreeWithTime();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;

protected:
  virtual void insertHit(const DetectorHit_sptr& hit);
  
private:
  std::vector<std::string> fileList_;

  TChain* tree_;
  int64_t numEntries_ = 0;
  int64_t entryIndex_ = 0;

  CSHitCollection* hitCollection_;
  std::unique_ptr<EventTreeIOWithInitialInfo> treeIO_;
  std::list<std::vector<DetectorHit_sptr>> eventList_;
  std::list<std::vector<DetectorHit_sptr>>::iterator eventIter_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SortEventTreeWithTime_H */
