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
 * HistogramFramePedestalSigma
 *
 * @author Taihei Watanabe
 * @date 2020-10-23
 */

#ifndef COMPTONSOFT_HistogramFramePedestalSigma_H
#define COMPTONSOFT_HistogramFramePedestalSigma_H 1

#include "VCSModule.hh"

class TH1;

namespace comptonsoft {

class XrayEventCollection;

class HistogramFramePedestalSigma : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramFramePedestalSigma, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  HistogramFramePedestalSigma();
  
protected:
  HistogramFramePedestalSigma(const HistogramFramePedestalSigma&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_analyze() override;

  void drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames) override;

private:
  int detectorID_ = 0;
  int numBins_ = 1;
  double min_ = 0.0;
  double max_ = 0.0;

  std::string outputName_;
  
  TH1* histogram_ = nullptr;
  FrameData* frame_;

  void fillInHistogram();
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramFramePedestalSigma_H */
