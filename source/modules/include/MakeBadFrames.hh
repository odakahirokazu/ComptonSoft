/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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
 * MakeBadFrames.
 *
 * @author Tsubasa Tamba
 * @date 2019-07-24
 * @date 2020-04-01 | Hirokazu Odaka | use module-result | v1.1
 *
 */

#ifndef COMPTONSOFT_MakeBadFrames_H
#define COMPTONSOFT_MakeBadFrames_H 1

#include "VCSModule.hh"
#include "FrameData.hh"

namespace comptonsoft {

class MakeBadFrames : public VCSModule
{
  DEFINE_ANL_MODULE(MakeBadFrames, 1.1);
  // ENABLE_PARALLEL_RUN();
public:
  MakeBadFrames();
  
public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  void calculateStatistics();

private:
  int detectorID_ = 0;
  double thresholdSigma_;
  std::vector<double> rawFrameMedian_;
  std::vector<int> badFrames_;
  double average_ = 0.0;
  double sigma_ = 0.0;
  FrameData* frame_ = nullptr;
};

} /* namespace comptonsoft*/

#endif /* COMPTONSOFT_MakeBadFrames_H */
