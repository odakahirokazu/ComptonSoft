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

// CSHitCollection.hh
// 2008-08-28 Hirokazu Odaka
// 2011-04-22 Hirokazu Odaka

#ifndef COMPTONSOFT_CSHitCollection_H
#define COMPTONSOFT_CSHitCollection_H 1

#include <vector>

#include "BasicModule.hh"
#include "DetectorHit_sptr.hh"
#include "HXISGDFlagDef.hh"

namespace comptonsoft {

class CSHitCollection : public anl::BasicModule
{
  DEFINE_ANL_MODULE(CSHitCollection, 1.0);
public:
  CSHitCollection() {}
  ~CSHitCollection() {}

  std::vector<comptonsoft::DetectorHit_sptr>& GetHits() { return hitvec_notg; }
  std::vector<comptonsoft::DetectorHit_sptr>& GetHits(int time_group);
   
  void InsertHit(comptonsoft::DetectorHit_sptr hit);
  void ClearHits() { GetHits().clear(); }
  void ClearHits(int time_group) { GetHits(time_group).clear(); }

  int NumHits() { return hitvec_notg.size(); }
  int NumHits(int time_group);

  int NumTimeGroup() { return hitvec_tg.size(); }

  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_endrun();

private:
  //std::vector<DetectorHit*> hitvec;
  std::vector< std::vector<comptonsoft::DetectorHit_sptr> > hitvec_tg;
  std::vector<comptonsoft::DetectorHit_sptr> hitvec_notg;
};


inline
std::vector<comptonsoft::DetectorHit_sptr>& CSHitCollection::GetHits(int time_group)
{
  if (time_group == comptonsoft::NOTIMEGROUP) { return hitvec_notg; }
  
  return hitvec_tg[time_group];
}


inline
int CSHitCollection::NumHits(int time_group)
{
  if (time_group == comptonsoft::NOTIMEGROUP) { return hitvec_notg.size(); }
  
  return hitvec_tg[time_group].size();
}

}

#endif /* COMPTONSOFT_CSHitCollection_H */
