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

#ifndef COMPTONSOFT_AzimuthDistribution_H
#define COMPTONSOFT_AzimuthDistribution_H 1

#include <vector>

#include "VCSModule.hh"

#include "globals.hh"

#include "TH1.h"
#include "TVector3.h"

#include "EventReconstruction.hh"

namespace comptonsoft {

class AzimuthDistribution : public VCSModule
{
  DEFINE_ANL_MODULE(AzimuthDistribution, 2.0);
public:
  AzimuthDistribution();
  ~AzimuthDistribution() {}
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
private:
  EventReconstruction* EventReconstruction_ptr;

  TVector3 SourcePos;

  TH1D* hist_phi_All;
  TH1D* hist_phi_All_nf;

  std::vector<TH1D*> hist_vec;
  std::vector<TH1D*> hist_nf_vec;

  int NumBin;
  double RangePhi1;
  double RangePhi2;

  double ThetaLD;
  double ThetaUD;
};

}

#endif /* COMPTONSOFT_AzimuthDistribution_H */


