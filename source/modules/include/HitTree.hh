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

// 2008-08-28 Hirokazu Odaka

#ifndef COMPTONSOFT_HitTree_H
#define COMPTONSOFT_HitTree_H 1

#include "VCSModule.hh"

#include "TTree.h"

namespace comptonsoft {

class CSHitCollection;
class ReadDataFile;


class HitTree : public VCSModule
{
  DEFINE_ANL_MODULE(HitTree, 1.2);
public:
  HitTree();
  ~HitTree() {}
  
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

protected:
  CSHitCollection* hit_collection;
  const ReadDataFile* m_ReadDataModule;
  TTree* hittree;

  int eventid;
  int seqnum;
  int totalhit;
  double ini_energy;

  double realposx;
  double realposy;
  double realposz;

  double localposx;
  double localposy;
  double localposz;

  double posx;
  double posy;
  double posz;

  double edep;
  double e_pha;
  double e_pi;
  
  double time1;

  unsigned int process;
  
  int grade;
  
  int detid;
  int stripx;
  int stripy;
  int chip;
  int channel;

  int time_group;

  unsigned int flag;
};

}

#endif /* COMPTONSOFT_HitTree_H */
