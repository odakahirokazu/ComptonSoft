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

#ifndef COMPTONSOFT_EvaluateEventRateForNanoGRAMS_hh
#define COMPTONSOFT_EvaluateEventRateForNanoGRAMS_hh 1
#include <anlnext/BasicModule.hh>
#include "VCSModule.hh"

namespace comptonsoft {
class CSHitCollection;

class EvaluateEventRateForNanoGRAMS : public VCSModule
{
DEFINE_ANL_MODULE(EvaluateEventRateForNanoGRAMS, 1.0);
ENABLE_PARALLEL_RUN();
public:
  EvaluateEventRateForNanoGRAMS();
  virtual ~EvaluateEventRateForNanoGRAMS() override;

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  
  double calculateEventRate(long totalEvents, long num_samples, int min_hits);
  
private:
  std::vector<int> numHits_;
  static constexpr int MAX_HITS = 4;
  const CSHitCollection* hitCollection_;
  int timeGroup_;
  
  double intensity_;
  double deadTimeNoDetection_;
  double deadTimeDetection_;
  double detectionEfficiency_;
  double noiseRate_;
  double maxDriftTime_;
};
} // namespace comptonsoft
#endif //COMPTONSOFT_EvaluateEventRateForNanoGRAMS_hh