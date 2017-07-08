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

#ifndef COMPTONSOFT_CSHitCollection_H
#define COMPTONSOFT_CSHitCollection_H 1

#include "BasicModule.hh"
#include <cstdint>
#include <vector>
#include "DetectorHit_sptr.hh"

namespace comptonsoft {
/**
 * @author Hirokazu Odaka
 * @date 2008-08-28
 * @date 2014-11-22
 * @date 2015-11-12
 */
class CSHitCollection : public anl::BasicModule
{
  DEFINE_ANL_MODULE(CSHitCollection, 1.2);
public:
  CSHitCollection();
  ~CSHitCollection();

  std::vector<DetectorHit_sptr>& getHits(int timeGroup=0)
  { return hitsVector_[timeGroup]; }

  const std::vector<DetectorHit_sptr>& getHits(int timeGroup=0) const
  { return hitsVector_[timeGroup]; }
  
  virtual void initializeEvent();
  
  void insertHit(const DetectorHit_sptr& hit);
  int NumberOfTimeGroups() const { return hitsVector_.size(); }

  anl::ANLStatus mod_begin_run() override;
  anl::ANLStatus mod_analyze() override;
  anl::ANLStatus mod_end_run() override;

private:
  std::vector<std::vector<comptonsoft::DetectorHit_sptr>> hitsVector_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CSHitCollection_H */
