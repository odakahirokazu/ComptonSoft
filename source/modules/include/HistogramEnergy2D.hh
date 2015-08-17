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

#ifndef COMPTONSOFT_HistogramEnergy2D_H
#define COMPTONSOFT_HistogramEnergy2D_H 1

#include "VCSModule.hh"

class TH2;

namespace comptonsoft {

class DetectorGroupManager;
class EventReconstruction;

class HistogramEnergy2D : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramEnergy2D, 3.0)
public:
  HistogramEnergy2D();
  ~HistogramEnergy2D() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
private:
  const DetectorGroupManager* detectorGroupManager_;
  const EventReconstruction* eventReconstruction_;

  TH2* hist_all_;
  std::vector<TH2*> hist_vec_;

  int numBins_;
  double energy0_;
  double energy1_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramEnergy2D_H */
