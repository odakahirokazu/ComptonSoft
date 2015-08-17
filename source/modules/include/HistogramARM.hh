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

#ifndef COMPTONSOFT_HistogramARM_H
#define COMPTONSOFT_HistogramARM_H 1

#include "VCSModule.hh"
#include <vector>

class TH1;

namespace comptonsoft {

class DetectorGroupManager;
class EventReconstruction;

class HistogramARM : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramARM, 3.0);
public:
  HistogramARM();
  ~HistogramARM() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
protected:
  const DetectorGroupManager* detectorGroupManager_;
  const EventReconstruction* eventReconstruction_;

  TH1* hist_all_;
  std::vector<TH1*> hist_vec_;

private:
  int numBins_;
  double range0_;
  double range1_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramARM_H */
