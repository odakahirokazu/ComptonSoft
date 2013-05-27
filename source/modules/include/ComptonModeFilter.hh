/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka, Shin Watanabe                      *
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

#ifndef COMPTONSOFT_ComptonModeFilter_H
#define COMPTONSOFT_ComptonModeFilter_H 1

#include "TGraph.h"

#include "VCSModule.hh"

#include "EventReconstruction.hh"

namespace comptonsoft {

class ComptonModeFilter : public VCSModule
{
  DEFINE_ANL_MODULE(ComptonModeFilter, 1.0);
public:
  ComptonModeFilter();
  ~ComptonModeFilter() {}

  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

  double theta_cut_limit(double ene) const;
  double theta_cut_limit2(double ene) const;
  
private:
  void create_armcut_curves();

  EventReconstruction* EventReconstruction_ptr;
  TGraph* armcut_curve_ss;
  TGraph* armcut_curve_sc;
  TGraph* armcut_curve_cc;
};

}

#endif /* COMPTONSOFT_ComptonModeFilter_H */
