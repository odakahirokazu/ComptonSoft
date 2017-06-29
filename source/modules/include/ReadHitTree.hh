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

#ifndef COMPTONSOFT_ReadHitTree_H
#define COMPTONSOFT_ReadHitTree_H 1

#include "VCSModule.hh"
#include "InitialInformation.hh"

#include <vector>
#include <string>
#include <cstdint>
#include "DetectorHit_sptr.hh"

class TChain;

namespace comptonsoft {

class CSHitCollection;
class HitTreeIOWithInitialInfo;

/**
 * @author Hitokazu Odaka
 * @date 2014-11-30
 */
class ReadHitTree : public VCSModule, public anlgeant4::InitialInformation
{
  DEFINE_ANL_MODULE(ReadHitTree, 2.0);
public:
  ReadHitTree();
  ~ReadHitTree();
  
  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_init() override;
  anl::ANLStatus mod_ana() override;

protected:
  virtual void insertHit(const DetectorHit_sptr& hit);
  
private:
  std::vector<std::string> fileList_;
  bool trustNumHits_;

  TChain* hittree_;
  int64_t numEntries_ = 0;
  int64_t entryIndex_ = 0;

  CSHitCollection* hitCollection_;
  std::unique_ptr<HitTreeIOWithInitialInfo> treeIO_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadHitTree_H */
