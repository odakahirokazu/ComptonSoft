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
 * SetDynamicPedestals.
 *
 * @author Taihei Watanabe
 * @date 2021-04-02
 */

#ifndef COMPTONSOFT_SetDynamicPedestals_H
#define COMPTONSOFT_SetDynamicPedestals_H 1

#include "VCSModule.hh"
#include <deque>
#include <boost/multi_array.hpp>
#include "CSTypes.hh"

namespace comptonsoft {

class SetDynamicPedestals : public VCSModule
{
  DEFINE_ANL_MODULE(SetDynamicPedestals, 1.1);
  // ENABLE_PARALLEL_RUN();
public:
  SetDynamicPedestals();
  
public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  int detectorID_ = 0;
  int frameStoreCapacity_ = 1;
  std::deque<image_t> frameStore_;
  image_t frameSum_;
  int nx_ = 1;
  int ny_ = 1;
  FrameData* frameData_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SetDynamicPedestals_H */
