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

#include "VCSModule.hh"

class TGraph;

namespace comptonsoft {

class EventReconstruction;

class ComptonModeFilter : public VCSModule
{
  DEFINE_ANL_MODULE(ComptonModeFilter, 1.1);
public:
  ComptonModeFilter();
  ~ComptonModeFilter() = default;

  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

  double theta_cut_limit(double energy) const;
  double theta_cut_limit2(double energy) const;
  
private:
  void create_armcut_curves();

  EventReconstruction* m_EventReconstruction;
  TGraph* m_ARMCutCurveSS;
  TGraph* m_ARMCutCurveSC;
  TGraph* m_ARMCutCurveCC;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ComptonModeFilter_H */
