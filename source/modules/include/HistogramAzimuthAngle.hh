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

#ifndef COMPTONSOFT_HistogramAzimuthAngle_H
#define COMPTONSOFT_HistogramAzimuthAngle_H 1

#include "VCSModule.hh"
#include <vector>

class TH1;

namespace comptonsoft {

class EventReconstruction;

class HistogramAzimuthAngle : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramAzimuthAngle, 2.1);
public:
  HistogramAzimuthAngle();
  ~HistogramAzimuthAngle() = default;
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
private:
  const EventReconstruction* eventReconstruction_;

  TH1* hist_all_;
  std::vector<TH1*> hist_vec_;

  int numBins_;
  double theta_min_;
  double theta_max_;
  double phi_origin_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramAzimuthAngle_H */
