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
 * HistogramFramePedestal
 *
 * @author Hirokazu Odaka
 * @date 2020-06-17
 */

#ifndef COMPTONSOFT_HistogramFramePedestal_H
#define COMPTONSOFT_HistogramFramePedestal_H 1

#include "VCSModule.hh"

class TH2;

namespace comptonsoft {

class XrayEventCollection;

class HistogramFramePedestal : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramFramePedestal, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  HistogramFramePedestal();
  
protected:
  HistogramFramePedestal(const HistogramFramePedestal&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_end_run() override;

  void drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames) override;

private:
  int detectorID_ = 0;
  int meanNumBins_ = 1;
  double meanMin_ = 0.0;
  double meanMax_ = 0.0;
  int sigmaNumBins_ = 1;
  double sigmaMin_ = 0.0;
  double sigmaMax_ = 0.0;

  std::string outputName_;
  
  TH2* histogram_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramFramePedestal_H */
