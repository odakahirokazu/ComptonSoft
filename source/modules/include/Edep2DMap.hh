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

#ifndef COMPTONSOFT_Edep2DMap_H
#define COMPTONSOFT_Edep2DMap_H 1

#include "VCSModule.hh"

#include "globals.hh"

#include "EventReconstruction.hh"

#include "TH1.h"
#include "TH2.h"

namespace comptonsoft {

class Edep2DMap : public VCSModule
{
  DEFINE_ANL_MODULE(Edep2DMap, 1.1)
public:
  Edep2DMap();
  ~Edep2DMap() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
private:
  EventReconstruction* EventReconstruction_ptr;

  TVector3 PlaneNormal;
  TVector3 PlanePoint;

  TH2D* hist_all;
  TH2D* hist_all_nf;
  std::vector<TH2D*> hist_vec;
  std::vector<TH2D*> hist_nf_vec;

  int NumBinEnergy;

  double RangeEnergy1;
  double RangeEnergy2;
};

}

#endif /* COMPTONSOFT_Edep2DMap_H */


