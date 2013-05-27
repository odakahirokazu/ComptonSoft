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

#ifndef COMPTONSOFT_CalcARM_H
#define COMPTONSOFT_CalcARM_H 1

#include <vector>

#include "VCSModule.hh"
#include "cs_globals.hh"

#include "EventReconstruction.hh"

class TH1;

namespace comptonsoft {


class CalcARM : public VCSModule
{
  DEFINE_ANL_MODULE(CalcARM, 2.0);
public:
  CalcARM();
  ~CalcARM() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_com();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
protected:
  EventReconstruction* EventReconstruction_ptr;

  TH1* hist_arm_All;
  TH1* hist_arm_All_nf;
  std::vector<TH1*> hist_vec;
  std::vector<TH1*> hist_nf_vec;

private:
  int NumBin;
  double RangeTheta1;
  double RangeTheta2;
};

}

#endif /* COMPTONSOFT_CalcARM_H */


