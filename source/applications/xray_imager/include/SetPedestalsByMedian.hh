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

/**
 * SetPedestalsByMedian.
 *
 * @author Tsubasa Tamba
 * @date 2019-07-24
 * @date 2020-04-01 | v1.1
 */

#ifndef COMPTONSOFT_SetPedestalsByMedian_H
#define COMPTONSOFT_SetPedestalsByMedian_H 1

#include "VCSModule.hh"
#include <boost/multi_array.hpp>

namespace comptonsoft {

using frames_t = boost::multi_array<std::vector<std::pair<int, double>>, 2>;

class SetPedestalsByMedian : public VCSModule
{
  DEFINE_ANL_MODULE(SetPedestalsByMedian, 1.1);
  // ENABLE_PARALLEL_RUN();
public:
  SetPedestalsByMedian();
  
public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;

  double calculateMedian(std::vector<std::pair<int, double>>& v);

private:
  int detectorID_ = 0;
  int frameBin_ = 1;
  frames_t frameStack_;
  int nx_ = 1;
  int ny_ = 1;
  FrameData* frameData_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SetPedestalsByMedian_H */
